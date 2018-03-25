/*
* Created by Mark Johnson on 1/4/2018.
*/

#ifndef VEHO_UNIFY_SAME_FILTER_CALLBACKS_PASS_HPP
#define VEHO_UNIFY_SAME_FILTER_CALLBACKS_PASS_HPP

#include <boost/mp11/integer_sequence.hpp>
#include <boost/mp11/tuple.hpp>
#include <boost/mp11/list.hpp>

#include <veho/facet/receiver/optimization/pass_traits.hpp>

#include <veho/detail/tuple_utils.hpp>

namespace veho {
    namespace facet {
        namespace receiver {
            namespace optimization {
                namespace normalization {
                    namespace detail {
                        template <typename Callbacks>
                        class callback_unifier {
                        private:
                            template <typename... Ts>
                            struct invoker {
                                constexpr explicit invoker(Ts&& ... ts) : ts(
                                        std::forward_as_tuple(std::forward<Ts>(ts)...)) {}

                                template <typename F>
                                constexpr void operator()(const F& f) {
                                    invoke(f, boost::mp11::make_index_sequence<sizeof...(Ts)>());
                                }

                                template <typename F, std::size_t... Is>
                                constexpr void invoke(const F& f, boost::mp11::index_sequence<Is...>) {
                                    f(std::forward<std::tuple_element<Is, decltype(ts)>>(std::get<Is>(ts))...);
                                };

                                std::tuple<Ts...> ts;
                            };

                        public:
                            constexpr explicit callback_unifier(Callbacks&& callbacks) : callbacks(callbacks) {}

                            template <typename... Ts>
                            constexpr void operator()(const Ts& ... ts) {
                                boost::mp11::tuple_for_each(callbacks, invoker<Ts...>{std::forward<Ts>(ts)...});
                            }

                        private:
                            Callbacks callbacks;
                        };

                        template <typename TypeList, typename Callbacks, typename TupleAccumulator = std::tuple<>, typename Enable = void>
                        struct condenser;

                        template <typename TypeList, typename Tuple, typename TupleAccumulator>
                        struct condenser<TypeList, Tuple, TupleAccumulator, typename std::enable_if<(
                                boost::mp11::mp_size<TypeList>::value ==
                                0)>::type> {
                            constexpr condenser(Tuple&&, TupleAccumulator&& accumulator) : value(accumulator) {}

                            using condensed_t = TupleAccumulator;

                            condensed_t value;
                        };

                        template <typename TypeList, typename Tuple, typename TupleAccumulator>
                        struct condenser<TypeList, Tuple, TupleAccumulator, typename std::enable_if<(
                                boost::mp11::mp_size<TypeList>::value >
                                0)>::type> {

                            using next_condenser_t = condenser<
                                    boost::mp11::mp_pop_front<TypeList>,
                                    Tuple,
                                    boost::mp11::mp_push_back<TupleAccumulator, boost::mp11::mp_front<TypeList>>
                            >;

                            using condensed_t = typename next_condenser_t::condensed_t;

                            constexpr condenser(Tuple&& tuple, TupleAccumulator&& accumulator)
                                    : value(
                                    next_condenser_t(std::forward<Tuple>(tuple),
                                                     veho::detail::tuple_push_back(accumulator,
                                                                                   veho::detail::get_tuple_elem_by_type<boost::mp11::mp_front<TypeList>>(
                                                                                           tuple))).value) {}

                            condensed_t value;
                        };

                        template <typename TypeList, typename Tuple>
                        constexpr inline typename condenser<TypeList, Tuple>::condensed_t condense(Tuple&& tuple) {
                            return condenser<TypeList, Tuple>{std::forward<Tuple>(tuple), std::make_tuple()}.value;
                        };

                        template <typename CallbackTuple, bool Unify>
                        struct make_appropriate_callback_impl {
                            using callback_t = callback_unifier<CallbackTuple>;

                            constexpr explicit make_appropriate_callback_impl(CallbackTuple&& callbacks) : value(
                                    callback_t(std::forward<CallbackTuple>(callbacks))) {}

                            callback_t value;
                        };

                        template <typename CallbackTuple>
                        struct make_appropriate_callback_impl<CallbackTuple, false> {
                            using callback_t = boost::mp11::mp_front<CallbackTuple>;

                            constexpr explicit make_appropriate_callback_impl(CallbackTuple&& callbacks) : value(
                                    std::get<0>(callbacks)) {}

                            callback_t value;
                        };

                        template <typename CallbackTuple, bool Unify = (boost::mp11::mp_size<CallbackTuple>::value > 1)>
                        constexpr typename make_appropriate_callback_impl<CallbackTuple, Unify>::callback_t
                        make_appropriate_callback(CallbackTuple&& tuple) {
                            return make_appropriate_callback_impl<CallbackTuple, Unify>(
                                    std::forward<CallbackTuple>(tuple)).value;
                        };


                        template <typename TypeMap, typename Callbacks, typename CallbackAccumulator = std::tuple<>, typename TypeMapAccumulator = boost::mp11::mp_list<>, typename Enable = void>
                        struct callback_with_same_key_in_map_unifier;

                        template <typename TypeMap, typename Callbacks, typename CallbackAccumulator, typename TypeMapAccumulator>
                        struct callback_with_same_key_in_map_unifier<TypeMap, Callbacks, CallbackAccumulator, TypeMapAccumulator, typename std::enable_if<(
                                boost::mp11::mp_size<TypeMap>::value == 0)>::type> {
                            constexpr callback_with_same_key_in_map_unifier(Callbacks&&,
                                                                            CallbackAccumulator&& accumulator)
                                    : value(
                                    accumulator) {}

                            using unified_t = CallbackAccumulator;

                            using new_type_map_t = TypeMapAccumulator;

                            unified_t value;
                        };

                        template <typename TypeMap, typename Callbacks, typename CallbackAccumulator, typename TypeMapAccumulator>
                        struct callback_with_same_key_in_map_unifier<TypeMap, Callbacks, CallbackAccumulator, TypeMapAccumulator, typename std::enable_if<(
                                boost::mp11::mp_size<TypeMap>::value > 0)>::type> {
                            using curr_map_entry = boost::mp11::mp_front<TypeMap>;

                            using curr_callback_types = boost::mp11::mp_pop_front<curr_map_entry>;

                            using curr_callback_types_as_tuple = boost::mp11::mp_rename<curr_callback_types, std::tuple>;

                            using new_callback = decltype(make_appropriate_callback(
                                    std::declval<curr_callback_types_as_tuple>()));

                            using updated_accumulator =  boost::mp11::mp_push_back<CallbackAccumulator, new_callback>;

                            using updated_type_map = boost::mp11::mp_push_back<
                                    TypeMapAccumulator,
                                    boost::mp11::mp_list<boost::mp11::mp_front<curr_map_entry>, new_callback>>;

                            using next_concat_stage = callback_with_same_key_in_map_unifier<boost::mp11::mp_pop_front<TypeMap>, Callbacks, updated_accumulator, updated_type_map>;

                            using new_type_map_t = typename next_concat_stage::new_type_map_t;

                            using unified_t = typename next_concat_stage::unified_t;

                            constexpr callback_with_same_key_in_map_unifier(Callbacks&& callbacks,
                                                                            CallbackAccumulator&& accumulator)
                                    : value(next_concat_stage(std::forward<Callbacks>(callbacks),
                                                              std::forward<updated_accumulator>(
                                                                      veho::detail::tuple_push_back(accumulator,
                                                                                                    make_appropriate_callback(
                                                                                                            condense<curr_callback_types, Callbacks>(
                                                                                                                    std::forward<Callbacks>(
                                                                                                                            callbacks)))))).value) {}

                            unified_t value;
                        };
                    }

                    template <typename Controller, typename TypeMap, typename Callbacks>
                    class unify_identically_filtered_callbacks_pass {
                    private:
                        using unifier = detail::callback_with_same_key_in_map_unifier<TypeMap, Callbacks>;

                    public:
                        constexpr explicit unify_identically_filtered_callbacks_pass(Callbacks&& callbacks)
                                : new_callbacks(unifier(std::forward<Callbacks>(callbacks), std::make_tuple()).value) {}

                        using pass_category = veho::facet::receiver::optimization::normalization_pass_tag;

                        using updated_type_map_type = typename unifier::new_type_map_t;

                        using updated_callbacks_type = typename unifier::unified_t;

                        updated_callbacks_type new_callbacks;
                    };
                }
            }
        }
    }
}

#endif //VEHO_UNIFY_SAME_FILTER_CALLBACKS_PASS_HPP
