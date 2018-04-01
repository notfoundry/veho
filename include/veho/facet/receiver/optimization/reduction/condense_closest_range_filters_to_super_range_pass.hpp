/*
* Created by Mark Johnson on 1/20/2018.
*/

#ifndef VEHO_CONDENSE_CLOSEST_RANGE_FILTERS_TO_SUPER_RANGE_PASS_HPP
#define VEHO_CONDENSE_CLOSEST_RANGE_FILTERS_TO_SUPER_RANGE_PASS_HPP

#include <limits>
#include <type_traits>
#include <utility>

#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/map.hpp>
#include <boost/mp11/integral.hpp>
#include <boost/mp11/utility.hpp>
#include <boost/mp11/list.hpp>

#include <veho/frame_matchers.hpp>
#include <veho/frame.hpp>

#include <veho/detail/matcher_utils.hpp>
#include <veho/detail/tuple_utils.hpp>

#include <veho/facet/receiver/optimization/pass_traits.hpp>

namespace veho {
    namespace facet {
        namespace receiver {
            namespace optimization {
                namespace reduction {
                    namespace detail {
                        template <typename LowerRange, typename UpperRange>
                        struct range_pair {
                            using lower_range = LowerRange;

                            using upper_range = UpperRange;

                            constexpr static const veho::common_frame::id_diff_type difference = upper_range::min - lower_range::max;
                        };

                        template <typename ReferenceUpperRange,
                                typename RangeList,
                                typename LowerRange,
                                veho::common_frame::id_diff_type LastDifference = ReferenceUpperRange::min - LowerRange::max,
                                std::size_t RangeListSize = boost::mp11::mp_size<RangeList>::value>
                        struct find_least_diff_range_pair_inner {
                        private:
                            using lower_range_candidate = boost::mp11::mp_front<RangeList>;

                            template <typename NewLowerRange>
                            using make_next_iteration_step = find_least_diff_range_pair_inner<
                                    ReferenceUpperRange,
                                    boost::mp11::mp_pop_front<RangeList>,
                                    NewLowerRange
                            >;

                            constexpr static const veho::common_frame::id_diff_type difference
                                    = ReferenceUpperRange::min - lower_range_candidate::max;

                            using next_iteration_step = make_next_iteration_step<
                                    boost::mp11::mp_if_c<
                                            (difference > 0 && difference < LastDifference),
                                            lower_range_candidate,
                                            LowerRange
                                    >
                            >;

                        public:
                            using type = typename next_iteration_step::type;
                        };

                        template <typename ReferenceUpperRange,
                                typename RangeList,
                                typename LowerRange,
                                veho::common_frame::id_diff_type LastDifference
                        >
                        struct find_least_diff_range_pair_inner<
                                ReferenceUpperRange, RangeList, LowerRange, LastDifference, 0
                        >{
                            using type = range_pair<LowerRange, ReferenceUpperRange>;
                        };

                        template <
                                typename RangeList,
                                typename WorkingRangeList = RangeList,
                                typename LowerRange = veho::frame_matchers::range<0, 0>,
                                typename UpperRange = veho::frame_matchers::range<
                                        std::numeric_limits<veho::common_frame::id_type>::max(),
                                        std::numeric_limits<veho::common_frame::id_type>::max()
                                >,
                                veho::common_frame::id_diff_type LastDifference = UpperRange::min - LowerRange::max,
                                std::size_t WorkingRangeListSize = boost::mp11::mp_size<WorkingRangeList>::value
                        >
                        struct find_least_diff_range_pair_outer {
                        private:
                            using upper_range_candidate = boost::mp11::mp_front<WorkingRangeList>;

                            using current_inner_iteration_step = find_least_diff_range_pair_inner<
                                    upper_range_candidate, RangeList, LowerRange, LastDifference
                            >;

                            using inner_iteration_result = typename current_inner_iteration_step::type;

                            template <typename NewLowerRange, typename NewUpperRange>
                            using make_next_outer_iteration_step = find_least_diff_range_pair_outer<
                                    RangeList,
                                    boost::mp11::mp_pop_front<WorkingRangeList>,
                                    NewLowerRange, NewUpperRange
                            >;

                            using next_outer_iteration_step = boost::mp11::mp_if_c<
                                    (inner_iteration_result::difference < LastDifference),
                                    make_next_outer_iteration_step<
                                            typename inner_iteration_result::lower_range,
                                            typename inner_iteration_result::upper_range
                                    >,
                                    make_next_outer_iteration_step<LowerRange, UpperRange>
                            >;

                        public:
                            using type = typename next_outer_iteration_step::type;
                        };

                        template <
                                typename RangeList,
                                typename WorkingRangeList,
                                typename LowerRange, typename UpperRange,
                                veho::common_frame::id_diff_type LastDifference
                        >
                        struct find_least_diff_range_pair_outer<
                                RangeList,
                                WorkingRangeList,
                                LowerRange, UpperRange,
                                LastDifference, 0
                        > {
                            using type = range_pair<LowerRange, UpperRange>;
                        };

                        template <typename RangeList>
                        struct find_least_diff_range_pair {
                            using type = typename find_least_diff_range_pair_outer<RangeList>::type;
                        };

                        template <
                                typename Controller,
                                typename LowerRange, typename UpperRange,
                                typename LowerRangeCallback, typename UpperRangeCallback
                        >
                        struct lower_and_upper_range_only_callback_unifier {
                            constexpr lower_and_upper_range_only_callback_unifier(LowerRangeCallback&& lower_range_callback,
                                                                                  UpperRangeCallback&& upper_range_callback)
                                    : lower_range_callback(std::forward<LowerRangeCallback>(lower_range_callback)),
                                      upper_range_callback(std::forward<UpperRangeCallback>(upper_range_callback)) {}

                            template <typename Dependencies>
                            inline void operator()(Dependencies&& deps, const veho::frame<Controller>& frame) {
                                const veho::common_frame::id_type frame_id = frame.id;
                                if (frame_id <= LowerRange::max) {
                                    lower_range_callback(std::forward<Dependencies>(deps), frame);
                                } else if (frame_id >= UpperRange::min) {
                                    upper_range_callback(std::forward<Dependencies>(deps), frame);
                                }
                            }

                            LowerRangeCallback lower_range_callback;

                            UpperRangeCallback upper_range_callback;
                        };

                        template <
                                typename Controller, typename FrontTypeMapEntry, typename Callbacks,
                                typename LowerRange, typename UpperRange,
                                typename LowerRangeCallback, typename UpperRangeCallback,
                                typename TypeMapAccumulator, typename CallbackAccumulator,
                                typename FrontTypeMapKey = boost::mp11::mp_first<FrontTypeMapEntry>
                        > struct update_type_map_and_callback_accumulators_after_inserting_super_range_step {
                        private:
                            using front_type_map_value = boost::mp11::mp_second<FrontTypeMapEntry>;

                        public:
                            constexpr explicit update_type_map_and_callback_accumulators_after_inserting_super_range_step(Callbacks&& callbacks, CallbackAccumulator&& accumulator)
                                    : new_callbacks(veho::detail::tuple_push_back(
                                            std::forward<CallbackAccumulator>(accumulator),
                                            veho::detail::get_tuple_elem_by_type<front_type_map_value>(std::forward<Callbacks>(callbacks))
                                    )) {}

                            using updated_type_map_type = boost::mp11::mp_push_back<
                                    TypeMapAccumulator,
                                    FrontTypeMapEntry
                            >;

                            using updated_callbacks_type = boost::mp11::mp_push_back<
                                    CallbackAccumulator,
                                    front_type_map_value
                            >;

                            updated_callbacks_type new_callbacks;
                        };

                        template <
                                typename Controller, typename FrontTypeMapEntry, typename Callbacks,
                                typename LowerRange, typename UpperRange,
                                typename LowerRangeCallback, typename UpperRangeCallback,
                                typename TypeMapAccumulator, typename CallbackAccumulator
                        > struct update_type_map_and_callback_accumulators_after_inserting_super_range_step<
                                Controller, FrontTypeMapEntry, Callbacks,
                                LowerRange, UpperRange,
                                LowerRangeCallback, UpperRangeCallback,
                                TypeMapAccumulator, CallbackAccumulator,
                                LowerRange
                        > {
                        private:
                            using unified_range_callbacks_type = lower_and_upper_range_only_callback_unifier<
                                    Controller,
                                    LowerRange, UpperRange,
                                    LowerRangeCallback, UpperRangeCallback
                            >;

                            constexpr inline unified_range_callbacks_type unify_lower_and_upper_range_callbacks(Callbacks&& callbacks) {
                                return unified_range_callbacks_type{
                                        veho::detail::get_tuple_elem_by_type<LowerRangeCallback>(std::forward<Callbacks>(callbacks)),
                                        veho::detail::get_tuple_elem_by_type<UpperRangeCallback>(std::forward<Callbacks>(callbacks))
                                };
                            }

                            using super_range = veho::frame_matchers::range<LowerRange::min, UpperRange::max>;

                        public:
                            constexpr explicit update_type_map_and_callback_accumulators_after_inserting_super_range_step(Callbacks&& callbacks, CallbackAccumulator&& accumulator)
                                    : new_callbacks(veho::detail::tuple_push_back(
                                            std::forward<CallbackAccumulator>(accumulator),
                                            unify_lower_and_upper_range_callbacks(std::forward<Callbacks>(callbacks))
                                    )) {}

                            using updated_type_map_type = boost::mp11::mp_push_back<
                                    TypeMapAccumulator,
                                    boost::mp11::mp_list<super_range, unified_range_callbacks_type>
                            >;

                            using updated_callbacks_type = boost::mp11::mp_push_back<
                                    CallbackAccumulator,
                                    unified_range_callbacks_type
                            >;

                            updated_callbacks_type new_callbacks;
                        };

                        template <
                                typename Controller, typename FrontTypeMapEntry, typename Callbacks,
                                typename LowerRange, typename UpperRange,
                                typename LowerRangeCallback, typename UpperRangeCallback,
                                typename TypeMapAccumulator, typename CallbackAccumulator
                        > struct update_type_map_and_callback_accumulators_after_inserting_super_range_step<
                                Controller, FrontTypeMapEntry, Callbacks,
                                LowerRange, UpperRange,
                                LowerRangeCallback, UpperRangeCallback,
                                TypeMapAccumulator, CallbackAccumulator,
                                UpperRange
                        > {
                            constexpr explicit update_type_map_and_callback_accumulators_after_inserting_super_range_step(Callbacks&&, CallbackAccumulator&& accumulator)
                                    : new_callbacks(std::forward<CallbackAccumulator>(accumulator)) {}

                            using updated_type_map_type = TypeMapAccumulator;

                            using updated_callbacks_type = CallbackAccumulator;

                            updated_callbacks_type new_callbacks;
                        };

                        template <
                                typename Controller, typename TypeMap, typename Callbacks,
                                typename LowerRange, typename UpperRange,
                                typename LowerRangeCallback, typename UpperRangeCallback,
                                typename TypeMapAccumulator = boost::mp11::mp_list<>, typename CallbackAccumulator = std::tuple<>,
                                std::size_t TypeMapSize = boost::mp11::mp_size<TypeMap>::value
                        > struct update_type_map_and_callbacks_after_inserting_super_range {
                        private:
                            using front_type_map_entry = boost::mp11::mp_front<TypeMap>;

                            using accumulator_update_step = update_type_map_and_callback_accumulators_after_inserting_super_range_step<
                                    Controller, front_type_map_entry, Callbacks,
                                    LowerRange, UpperRange,
                                    LowerRangeCallback, UpperRangeCallback,
                                    TypeMapAccumulator, CallbackAccumulator
                            >;

                            static_assert(!std::is_same<LowerRange, UpperRange>::value, "THIS SHOULD BE IMPOSSIBLE");

                            using next_step = update_type_map_and_callbacks_after_inserting_super_range<
                                    Controller, boost::mp11::mp_pop_front<TypeMap>, Callbacks,
                                    LowerRange, UpperRange,
                                    LowerRangeCallback, UpperRangeCallback,
                                    typename accumulator_update_step::updated_type_map_type,
                                    typename accumulator_update_step::updated_callbacks_type
                            >;

                        public:
                            constexpr explicit update_type_map_and_callbacks_after_inserting_super_range(Callbacks&& callbacks, CallbackAccumulator&& accumulator)
                                    : new_callbacks(next_step(
                                            std::forward<Callbacks>(callbacks),
                                            accumulator_update_step(
                                                    std::forward<Callbacks>(callbacks),
                                                    std::forward<CallbackAccumulator>(accumulator)
                                            ).new_callbacks
                                    ).new_callbacks) {}

                            using updated_type_map_type = typename next_step::updated_type_map_type;

                            using updated_callbacks_type = typename next_step::updated_callbacks_type;

                            updated_callbacks_type new_callbacks;
                        };

                        template <
                                typename Controller, typename TypeMap, typename Callbacks,
                                typename LowerRange, typename UpperRange,
                                typename LowerRangeCallback, typename UpperRangeCallback,
                                typename TypeMapAccumulator, typename CallbackAccumulator
                        > struct update_type_map_and_callbacks_after_inserting_super_range<
                                Controller, TypeMap, Callbacks,
                                LowerRange, UpperRange,
                                LowerRangeCallback, UpperRangeCallback,
                                TypeMapAccumulator, CallbackAccumulator,
                                0
                        > {
                            constexpr explicit update_type_map_and_callbacks_after_inserting_super_range(Callbacks&&, CallbackAccumulator&& accumulator)
                                    : new_callbacks(std::forward<CallbackAccumulator>(accumulator)) {}

                            using updated_type_map_type = TypeMapAccumulator;

                            using updated_callbacks_type = CallbackAccumulator;

                            updated_callbacks_type new_callbacks;
                        };

                        template <typename Controller, typename TypeMap, typename Callbacks>
                        struct condense_closest_range_filters_to_super_range_pass_impl {
                        private:
                            using frame_matchers = boost::mp11::mp_map_keys<TypeMap>;

                            using range_frame_matchers = boost::mp11::mp_copy_if<frame_matchers, veho::detail::is_range>;

                            static_assert(
                                    boost::mp11::mp_size<range_frame_matchers>::value >= 2,
                                    "There must be at least two range frame matchers to run optimization pass `condense_closest_range_filters_to_super_range_pass`"
                            );

                            using least_diff_range_pair = typename find_least_diff_range_pair<range_frame_matchers>::type;

                            using lower_range = typename least_diff_range_pair::lower_range;

                            using upper_range = typename least_diff_range_pair::upper_range;

                            template <typename Matcher>
                            using callback_for_matcher = boost::mp11::mp_second<boost::mp11::mp_map_find<TypeMap, Matcher>>;

                            using type_map_and_callbacks_updater = update_type_map_and_callbacks_after_inserting_super_range<
                                    Controller, TypeMap, Callbacks,
                                    lower_range, upper_range,
                                    callback_for_matcher<lower_range>, callback_for_matcher<upper_range>
                            >;

                        public:
                            constexpr explicit condense_closest_range_filters_to_super_range_pass_impl(Callbacks&& callbacks)
                                    : new_callbacks(type_map_and_callbacks_updater(std::forward<Callbacks>(callbacks), std::make_tuple()).new_callbacks) {}

                            using updated_type_map_type = typename type_map_and_callbacks_updater::updated_type_map_type;

                            using updated_callbacks_type = typename type_map_and_callbacks_updater::updated_callbacks_type;

                            constexpr static const veho::common_frame::id_diff_type num_frames_wasted = least_diff_range_pair::difference - 1;

                            updated_callbacks_type new_callbacks;
                        };
                    }

                    template <typename TypeMap>
                    struct can_run_condense_closest_range_filters_to_super_range_pass {
                        using matcher_list = boost::mp11::mp_map_keys<TypeMap>;

                        using range_matchers = boost::mp11::mp_copy_if<matcher_list, veho::detail::is_range>;

                        constexpr static const bool value = boost::mp11::mp_size<range_matchers>::value >= 2;
                    };

                    template <typename Controller, typename TypeMap, typename Callbacks,
                            typename = typename std::enable_if<can_run_condense_closest_range_filters_to_super_range_pass<TypeMap>::value>::type>
                    class condense_closest_range_filters_to_super_range_pass {
                    private:
                        using impl = detail::condense_closest_range_filters_to_super_range_pass_impl<Controller, TypeMap, Callbacks>;

                    public:
                        constexpr explicit condense_closest_range_filters_to_super_range_pass(Callbacks&& callbacks)
                                : new_callbacks(impl(std::forward<Callbacks>(callbacks)).new_callbacks) {}

                        using pass_category = veho::facet::receiver::optimization::reduction_pass_tag;

                        using updated_type_map_type = typename impl::updated_type_map_type;

                        using updated_callbacks_type = typename impl::updated_callbacks_type;

                        constexpr static const veho::common_frame::id_diff_type num_frames_wasted = impl::num_frames_wasted;

                        updated_callbacks_type new_callbacks;
                    };
                }
            }
        }
    }
}
#endif //VEHO_CONDENSE_CLOSEST_RANGE_FILTERS_TO_SUPER_RANGE_PASS_HPP
