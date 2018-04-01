/*
* Created by Mark Johnson on 3/24/2018.
*/

#ifndef VEHO_RECEIVER_FACET_CAPABILITY_DATA_HPP
#define VEHO_RECEIVER_FACET_CAPABILITY_DATA_HPP

#include <utility>
#include <tuple>

#include <boost/mp11/list.hpp>
#include <boost/mp11/map.hpp>
#include <boost/mp11/integer_sequence.hpp>

#include <veho/frame.hpp>

#include <veho/detail/tuple_utils.hpp>

#include "detail/function_traits.hpp"

namespace veho {
    namespace facet {
        namespace receiver {
            namespace detail {
                template <typename T>
                struct is_frame : std::false_type {};

                template <typename Controller>
                struct is_frame<veho::frame<Controller>> : std::true_type {};

                template <typename Callback>
                struct get_callback_dependencies {
                private:
                    using callback_parameter_types = typename detail::function_traits<Callback>::parameter_type_list;

                    template <typename T>
                    using to_cv_unqualified_type = typename std::remove_const<
                            typename std::remove_reference<T>::type
                    >::type;

                    using unqualified_parameter_types = boost::mp11::mp_transform<to_cv_unqualified_type, callback_parameter_types>;

                    using callback_parameter_types_without_domain_object = boost::mp11::mp_remove_if<unqualified_parameter_types, is_frame>;

                public:
                    using type = callback_parameter_types_without_domain_object;
                };

                template <typename Controller, std::size_t Index, typename Callback>
                struct callback_wrapper {
                private:
                    template <typename ArgsTuple, template <typename...> class List, typename... RequiredArgs>
                    inline void invoke(ArgsTuple&& args, const veho::frame<Controller>& frame, List<RequiredArgs...>) {
                        callback(veho::detail::get_tuple_elem_by_type<RequiredArgs>(std::forward<ArgsTuple>(args))..., frame);
                    }

                public:
                    constexpr explicit callback_wrapper(Callback&& callback)
                            : callback(std::forward<Callback>(callback)) {}

                    constexpr static const std::size_t index = Index;

                    using callback_type = Callback;

                    using callback_dependencies = typename get_callback_dependencies<Callback>::type;

                    template <typename Dependencies>
                    inline void operator()(Dependencies&& deps, const veho::frame<Controller>& frame) {
                        invoke(std::forward<Dependencies>(deps), frame, callback_dependencies{});
                    }

                    Callback callback;
                };

                template <typename DependencySet, typename Callback>
                struct update_dependency_set_with_callback_dependencies {
                private:
                    using callback_dependencies = typename get_callback_dependencies<Callback>::type;

                    using appended_dependency_list = boost::mp11::mp_append<DependencySet, callback_dependencies>;

                    using updated_dependency_set = boost::mp11::mp_unique<appended_dependency_list>;

                public:
                    using type = updated_dependency_set;
                };
            }

            template <
                    typename Controller,
                    typename TypeMap = boost::mp11::mp_list<>,
                    typename Callbacks = std::tuple<>,
                    typename DependencySet = std::tuple<>
            >
            struct receiver_capability_data {
            private:
                template <typename Matcher>
                using compute_map_entry_if_absent = boost::mp11::mp_eval_if_c<
                        !boost::mp11::mp_map_contains<TypeMap, Matcher>::value,
                        boost::mp11::mp_list<Matcher>,
                        boost::mp11::mp_map_find, TypeMap, Matcher
                >;

                template <typename Matcher, typename Callback>
                using appended_or_inserted_into_map_type = boost::mp11::mp_map_replace<
                        TypeMap,
                        boost::mp11::mp_push_back<
                                compute_map_entry_if_absent<Matcher>,
                                Callback
                        >
                >;

                template <typename Callback>
                using update_dependencies_with_those_in = typename detail::update_dependency_set_with_callback_dependencies<DependencySet, Callback>::type;

                template <typename Matcher, typename Callback, typename NewCapabilityData>
                using pushed_back_type = receiver_capability_data<
                        Controller,
                        appended_or_inserted_into_map_type<Matcher, Callback>,
                        boost::mp11::mp_push_back<Callbacks, Callback>,
                        NewCapabilityData
                >;

                template <typename Callback>
                using make_callback_wrapper = detail::callback_wrapper<
                        Controller, boost::mp11::mp_size<Callbacks>::value, Callback
                >;

            public:
                constexpr explicit receiver_capability_data(Callbacks&& callbacks) : callbacks(
                        std::forward<Callbacks>(callbacks)) {}

                template <typename Matcher, typename Callback>
                constexpr inline pushed_back_type<Matcher, make_callback_wrapper<Callback>, update_dependencies_with_those_in<Callback>>
                push_back(Matcher&&, Callback&& callback) {
                    return pushed_back_type<Matcher, make_callback_wrapper<Callback>, update_dependencies_with_those_in<Callback>>(
                            veho::detail::tuple_push_back(
                                    std::move(static_cast<Callbacks>(callbacks)),
                                    make_callback_wrapper<Callback>(std::forward<Callback>(callback))
                            )
                    );
                }

                constexpr static const std::size_t num_callbacks = boost::mp11::mp_size<Callbacks>::value;

                using callbacks_type = Callbacks;

                using matcher_to_callback_map_type = TypeMap;

                using dependency_set_type = DependencySet;

                callbacks_type callbacks;
            };

            template <typename Controller>
            constexpr inline receiver_capability_data<Controller> make_receiver_capability_data() {
                return receiver_capability_data<Controller>(std::make_tuple());
            }
        }
    }
}
#endif //VEHO_RECEIVER_FACET_CAPABILITY_DATA_HPP
