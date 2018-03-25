/*
* Created by Mark Johnson on 1/6/2018.
*/

#ifndef VEHO_CONDENSE_CLOSEST_EXACT_FILTERS_TO_RANGE_PASS_HPP
#define VEHO_CONDENSE_CLOSEST_EXACT_FILTERS_TO_RANGE_PASS_HPP

#include <limits>
#include <type_traits>

#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/integral.hpp>
#include <boost/mp11/utility.hpp>
#include <boost/mp11/list.hpp>
#include <boost/mp11/map.hpp>

#include <veho/frame.hpp>
#include <veho/frame_matchers.hpp>

#include <veho/facet/receiver/optimization/pass_traits.hpp>

#include <veho/detail/tuple_utils.hpp>
#include <veho/detail/matcher_utils.hpp>

namespace veho {
    namespace facet {
        namespace receiver {
            namespace optimization {
                namespace reduction {
                    namespace detail {
                        template <typename TypeList,
                                std::size_t Idx = 0,
                                typename First = void, typename Second = void,
                                veho::common_frame::id_diff_type MinDiff = std::numeric_limits<veho::common_frame::id_diff_type>::max()>
                        struct get_least_diff_pair {
                            using type_at_idx = boost::mp11::mp_at_c<TypeList, Idx>;

                            using type_at_idx_plus_one = boost::mp11::mp_at_c<TypeList, Idx + 1>;

                            constexpr static const veho::common_frame::id_diff_type current_diff
                                    = type_at_idx_plus_one::id - type_at_idx::id;

                            using next_iter = boost::mp11::mp_if_c<
                                    (current_diff < MinDiff),
                                    get_least_diff_pair<TypeList,
                                            Idx + 1, type_at_idx, type_at_idx_plus_one, current_diff>,
                                    get_least_diff_pair<TypeList, Idx + 1, First, Second, MinDiff>
                            >;

                            using first = typename next_iter::first;

                            using second = typename next_iter::second;

                            constexpr static const veho::common_frame::id_diff_type difference = next_iter::difference;
                        };

                        template <typename TypeList, typename First, typename Second, veho::common_frame::id_diff_type MinDiff>
                        struct get_least_diff_pair<TypeList,
                                boost::mp11::mp_size<TypeList>::value - 1, First, Second, MinDiff> {
                            using first = First;

                            using second = Second;

                            constexpr static const veho::common_frame::id_diff_type difference = MinDiff;
                        };

                        template <typename First, typename Second>
                        struct exact_less {
                            constexpr const static bool value = First::id < Second::id;
                        };

                        template <typename TypeList>
                        struct condense_closest_exact_filter_types_to_range {
                            using exacts = boost::mp11::mp_copy_if<TypeList, veho::detail::is_exact>;

                            static_assert(
                                    boost::mp11::mp_size<exacts>::value >= 2,
                                    "There must be at least two exact frame matchers to run optimization pass `condense_closest_exact_filters_to_range_pass`"
                                    );

                            using sorted_exacts = boost::mp11::mp_sort<exacts, exact_less>;

                            using least_diff_pair = get_least_diff_pair<sorted_exacts>;

                            using new_range = veho::frame_matchers::range<least_diff_pair::first::id, least_diff_pair::second::id>;
                        };

                        template <typename Controller, typename Range, typename MinCallback, typename MaxCallback>
                        struct min_max_only_callback_unifier {
                            constexpr min_max_only_callback_unifier(MinCallback&& min_callback,
                                                                    MaxCallback&& max_callback)
                                    : min_callback(std::forward<MinCallback>(min_callback)),
                                      max_callback(std::forward<MaxCallback>(max_callback)) {}

                            void operator()(const veho::frame<Controller>& frame) {
                                switch (frame.id) {
                                    case Range::min:
                                        min_callback(frame);
                                        break;
                                    case Range::max:
                                        max_callback(frame);
                                        break;
                                    default:
                                        break;
                                }
                            }

                            MinCallback min_callback;

                            MaxCallback max_callback;
                        };

                        template <typename Controller, typename TypeMap, typename Callbacks, typename Condensation, typename TupleAccumulator, typename FrontType = boost::mp11::mp_front<boost::mp11::mp_front<TypeMap>>>
                        struct update_accumulator {

                            using current_callback = boost::mp11::mp_second<boost::mp11::mp_front<TypeMap>>;

                            constexpr update_accumulator(Callbacks&& callbacks, TupleAccumulator&& accumulator)
                                    : value(veho::detail::tuple_push_back(std::forward<TupleAccumulator>(accumulator),
                                                                          veho::detail::get_tuple_elem_by_type<current_callback>(
                                                                                  callbacks))) {}

                            using value_type = boost::mp11::mp_push_back<TupleAccumulator, current_callback>;

                            value_type value;

                        };

                        template <typename Controller, typename TypeMap, typename Callbacks, typename Condensation, typename TupleAccumulator>
                        struct update_accumulator<Controller, TypeMap, Callbacks, Condensation, TupleAccumulator, typename Condensation::least_diff_pair::first> {

                            using first_callback_t = boost::mp11::mp_second<boost::mp11::mp_map_find<TypeMap, typename Condensation::least_diff_pair::first>>;

                            using second_callback_t = boost::mp11::mp_second<boost::mp11::mp_map_find<TypeMap, typename Condensation::least_diff_pair::second>>;

                            using callback_unifier_type = min_max_only_callback_unifier<
                                    Controller,
                                    typename Condensation::new_range,
                                    first_callback_t,
                                    second_callback_t
                            >;

                            constexpr update_accumulator(Callbacks&& callbacks, TupleAccumulator&& accumulator)
                                    : value(veho::detail::tuple_push_back(std::forward<TupleAccumulator>(accumulator),
                                                                          callback_unifier_type(
                                                                                  veho::detail::get_tuple_elem_by_type<first_callback_t>(
                                                                                          callbacks),
                                                                                  veho::detail::get_tuple_elem_by_type<second_callback_t>(
                                                                                          callbacks)))) {}

                            using value_type = boost::mp11::mp_push_back<
                                    TupleAccumulator,
                                    callback_unifier_type
                            >;

                            value_type value;
                        };

                        template <typename Controller, typename TypeMap, typename Callbacks, typename Condensation, typename TupleAccumulator>
                        struct update_accumulator<Controller, TypeMap, Callbacks, Condensation, TupleAccumulator, typename Condensation::least_diff_pair::second> {

                            constexpr update_accumulator(Callbacks&&, TupleAccumulator&& accumulator) : value(
                                    std::forward<TupleAccumulator>(accumulator)) {}

                            using value_type = TupleAccumulator;

                            value_type value;
                        };


                        template <typename Controller, typename TypeMap, typename Callbacks, typename Condensation, typename TupleAccumulator = std::tuple<>,
                                std::size_t TypeMapSize = boost::mp11::mp_size<TypeMap>::value>
                        struct closest_exact_filter_values_to_range {
                            using accumulator_update = update_accumulator<Controller, TypeMap, Callbacks, Condensation, TupleAccumulator>;

                            using updated_accumulator_t = typename accumulator_update::value_type;

                            using next_step_t = closest_exact_filter_values_to_range<Controller, boost::mp11::mp_pop_front<TypeMap>, Callbacks, Condensation, updated_accumulator_t>;

                            constexpr closest_exact_filter_values_to_range(Callbacks&& callbacks,
                                                                           TupleAccumulator&& accumulator)
                                    : value(next_step_t(
                                    std::forward<Callbacks>(callbacks),
                                    accumulator_update(std::forward<Callbacks>(callbacks),
                                                       std::forward<TupleAccumulator>(accumulator)).value).value) {}

                            using new_tuple_t = typename next_step_t::new_tuple_t;

                            new_tuple_t value;
                        };

                        template <typename Controller, typename TypeMap, typename Callbacks, typename Condensation, typename TupleAccumulator>
                        struct closest_exact_filter_values_to_range<Controller, TypeMap, Callbacks, Condensation, TupleAccumulator, 0> {
                            constexpr closest_exact_filter_values_to_range(Callbacks&&, TupleAccumulator&& accumulator)
                                    : value(
                                    accumulator) {}

                            using new_tuple_t = TupleAccumulator;

                            new_tuple_t value;
                        };
                    }

                    template <typename TypeMap>
                    struct can_run_condense_closest_exact_filters_to_range_pass {
                        using matcher_list = boost::mp11::mp_map_keys<TypeMap>;

                        using exact_matchers = boost::mp11::mp_copy_if<matcher_list, veho::detail::is_exact>;

                        constexpr static const bool value = boost::mp11::mp_size<exact_matchers>::value >= 2;
                    };

                    template <typename Controller, typename TypeMap, typename Callbacks,
                            typename = typename std::enable_if<can_run_condense_closest_exact_filters_to_range_pass<TypeMap>::value>::type>
                    class condense_closest_exact_filters_to_range_pass {
                    private:
                        using condensed_filters = detail::condense_closest_exact_filter_types_to_range<boost::mp11::mp_map_keys<TypeMap>>;

                        using first_in_closest_pair = typename condensed_filters::least_diff_pair::first;

                        using second_in_closest_pair = typename condensed_filters::least_diff_pair::second;

                        using old_first_map_entry = boost::mp11::mp_map_find<TypeMap, first_in_closest_pair>;

                        using old_first_callback_type = boost::mp11::mp_second<old_first_map_entry>;

                        using old_second_callback_type = boost::mp11::mp_second<boost::mp11::mp_map_find<TypeMap, second_in_closest_pair>>;

                        using new_first_map_entry = boost::mp11::mp_list<
                                typename condensed_filters::new_range,
                                detail::min_max_only_callback_unifier<
                                        Controller,
                                        typename condensed_filters::new_range,
                                        old_first_callback_type,
                                        old_second_callback_type
                                >
                        >;

                    public:
                        constexpr explicit condense_closest_exact_filters_to_range_pass(Callbacks&& callbacks)
                                : new_callbacks(
                                detail::closest_exact_filter_values_to_range<Controller, TypeMap, Callbacks, condensed_filters>(
                                        std::forward<Callbacks>(callbacks), std::make_tuple()).value) {}

                        using pass_category = veho::facet::receiver::optimization::reduction_pass_tag;

                        using updated_type_map_type = boost::mp11::mp_map_erase<
                                boost::mp11::mp_replace<
                                        TypeMap,
                                        old_first_map_entry,
                                        new_first_map_entry
                                >,
                                second_in_closest_pair
                        >;

                        using updated_callbacks_type = typename detail::closest_exact_filter_values_to_range<Controller, TypeMap, Callbacks, condensed_filters>::new_tuple_t;

                        constexpr static const veho::common_frame::id_diff_type num_frames_wasted = condensed_filters::least_diff_pair::difference;

                        updated_callbacks_type new_callbacks;
                    };
                }
            }
        }
    }
}

#endif //VEHO_CONDENSE_CLOSEST_EXACT_FILTERS_TO_RANGE_PASS_HPP
