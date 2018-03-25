/*
* Created by Mark Johnson on 3/3/2018.
*/

#ifndef VEHO_REDUCTION_HPP
#define VEHO_REDUCTION_HPP

#include <boost/mp11/list.hpp>
#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/utility.hpp>
#include <boost/mp11/integral.hpp>

#include <veho/facet/receiver/optimization/pass_traits.hpp>

#include "condense_closest_exact_filters_to_range_pass.hpp"
#include "condense_closest_range_filters_to_super_range_pass.hpp"
#include "extend_range_filter_to_encapsulate_closest_exact_filter_pass.hpp"
#include "irreducible_mask_filter_handling_pass.hpp"

namespace veho {
    namespace facet {
        namespace receiver {
            namespace optimization {
                namespace reduction {
                    namespace detail {
                        template <typename Controller, typename TypeMap, typename Callbacks, typename QuotedPassList>
                        struct apply_best_reduction_pass {
                            template <typename QuotedPass>
                            using instantiate_quoted_pass = boost::mp11::mp_invoke<QuotedPass, Controller, TypeMap, Callbacks>;

                            template <typename QuotedPass>
                            using is_legal_to_instantiate_pass = boost::mp11::mp_valid<
                                    instantiate_quoted_pass,
                                    QuotedPass
                            >;

                            using all_viable_passes = boost::mp11::mp_copy_if<QuotedPassList, is_legal_to_instantiate_pass>;

                            using preemptively_ran_pass_list = boost::mp11::mp_transform<instantiate_quoted_pass, all_viable_passes>;

                            template <typename FirstPass, typename SecondPass>
                            using reduction_pass_waste_less = boost::mp11::mp_bool<
                                    (pass_traits<FirstPass>::num_frames_wasted < pass_traits<SecondPass>::num_frames_wasted)
                            >;

                            using best_reduction = boost::mp11::mp_min_element<preemptively_ran_pass_list, reduction_pass_waste_less>;

                            constexpr explicit apply_best_reduction_pass(Callbacks&& callbacks)
                                    : new_callbacks(best_reduction(std::forward<Callbacks>(callbacks)).new_callbacks) {}

                            using new_type_map_type = typename best_reduction::updated_type_map_type;

                            using new_callbacks_type = typename best_reduction::updated_callbacks_type;

                            new_callbacks_type new_callbacks;
                        };

                        template <
                                typename Controller, typename TypeMap, typename Callbacks, typename QuotedPassList,
                                std::size_t UsableMailboxCount, std::size_t NumCallbacks = boost::mp11::mp_size<Callbacks>::value
                        >
                        class callback_reducer_impl {
                        private:
                            static_assert(NumCallbacks > UsableMailboxCount,
                                          "Number of callbacks is already less than number of usable mailboxes");

                            using applied_best_pass = apply_best_reduction_pass<
                                    Controller, TypeMap, Callbacks, QuotedPassList
                            >;

                            static_assert(!std::is_same<TypeMap, typename applied_best_pass::new_type_map_type>::value, "TYPE MAPS ARE SAME");
                            static_assert(!std::is_same<Callbacks, typename applied_best_pass::new_callbacks_type>::value, "CALLBACKS ARE SAME");

                            using next_stage = callback_reducer_impl<
                                    Controller,
                                    typename applied_best_pass::new_type_map_type,
                                    typename applied_best_pass::new_callbacks_type,
                                    QuotedPassList,
                                    UsableMailboxCount
                            >;

                        public:
                            constexpr explicit callback_reducer_impl(Callbacks&& callbacks)
                                    : new_callbacks(next_stage(
                                            applied_best_pass(
                                                    std::forward<Callbacks>(callbacks)
                                            ).new_callbacks
                                    ).new_callbacks) {}

                            using new_type_map_type = typename next_stage::new_type_map_type;

                            using new_callbacks_type = typename next_stage::new_callbacks_type;

                            new_callbacks_type new_callbacks;

                        };

                        template <typename Controller, typename TypeMap, typename Callbacks, typename PassList, std::size_t UsableMailboxCount>
                        class callback_reducer_impl<Controller, TypeMap, Callbacks, PassList, UsableMailboxCount, UsableMailboxCount> {
                        public:
                            constexpr explicit callback_reducer_impl(Callbacks&& callbacks) : new_callbacks(
                                    std::forward<Callbacks>(callbacks)) {}

                            using new_type_map_type = TypeMap;

                            using new_callbacks_type = Callbacks;

                            new_callbacks_type new_callbacks;

                        };
                    }

                    template <typename Controller, typename TypeMap, typename Callbacks, std::size_t UsableMailboxCount>
                    class callback_reducer {
                    private:
                        using reduction_pass_list = boost::mp11::mp_list<
                                boost::mp11::mp_quote<condense_closest_exact_filters_to_range_pass>,
                                boost::mp11::mp_quote<condense_closest_range_filters_to_super_range_pass>,
                                boost::mp11::mp_quote<extend_range_filter_to_encapsulate_closest_exact_filter_pass>,
                                boost::mp11::mp_quote<irreducible_mask_filter_handling_pass>
                        >;

                        using impl = detail::callback_reducer_impl<Controller, TypeMap, Callbacks, reduction_pass_list, UsableMailboxCount>;

                    public:
                        constexpr explicit callback_reducer(Callbacks&& callbacks)
                                : new_callbacks(impl(std::forward<Callbacks>(callbacks)).new_callbacks) {}

                        using new_type_map_type = typename impl::new_type_map_type;

                        using new_callbacks_type = typename impl::new_callbacks_type;

                        new_callbacks_type new_callbacks;
                    };
                }
            }
        }
    }
}

#endif //VEHO_REDUCTION_HPP
