/*
* Created by Mark Johnson on 1/7/2018.
*/

#ifndef VEHO_APPEND_EXACT_FILTER_CALLBACK_TO_RANGE_FILTER_CALLBACK_WHEN_EXACT_WITHIN_RANGE_PASS_HPP
#define VEHO_APPEND_EXACT_FILTER_CALLBACK_TO_RANGE_FILTER_CALLBACK_WHEN_EXACT_WITHIN_RANGE_PASS_HPP

#include <utility>

#include <boost/mp11.hpp>

#include <veho/detail/matcher_utils.hpp>

#include <veho/facet/receiver/optimization/pass_traits.hpp>

#include "normalized_callback_traits.hpp"

namespace veho {
    namespace facet {
        namespace receiver {
            namespace optimization {
                namespace normalization {
                    namespace detail {
                        template <
                                typename Controller,
                                typename Range, typename Exact,
                                typename RangeCallback, typename ExactCallback
                        >
                        struct exact_filter_within_range_filter_callback_unifier {
                            constexpr exact_filter_within_range_filter_callback_unifier(RangeCallback&& range_callback, ExactCallback&& exact_callback)
                                    : range_callback(std::forward<RangeCallback>(range_callback)),
                                      exact_callback(std::forward<ExactCallback>(exact_callback)) {}

                            void operator()(const veho::frame<Controller>& frame) {
                                range_callback(frame);
                                if (frame.id == Exact::id) {
                                    exact_callback(frame);
                                }
                            }

                            RangeCallback range_callback;

                            ExactCallback exact_callback;
                        };

                        template <typename Controller, typename TypeMap, typename Callbacks>
                        struct append_exact_filter_callback_to_range_filter_callback_when_exact_within_range_pass_impl {
                        private:
                            using matcher_list = boost::mp11::mp_map_keys<TypeMap>;

                            using exact_matchers = boost::mp11::mp_copy_if<matcher_list, veho::detail::is_exact>;
                        public:
                            constexpr explicit append_exact_filter_callback_to_range_filter_callback_when_exact_within_range_pass_impl(
                                    Callbacks&& callbacks)
                            : new_callbacks(std::forward<Callbacks>(callbacks)) {}

                            using updated_type_map_type = TypeMap;

                            using updated_callbacks_type =Callbacks;

                            updated_callbacks_type new_callbacks;
                        };
                    }

                    template <typename Controller,
                            typename Range, typename Exact,
                            typename RangeCallback, typename ExactCallback
                    >
                    struct normalized_callback_traits<detail::exact_filter_within_range_filter_callback_unifier<
                            Controller, Range, Exact, RangeCallback, ExactCallback
                    >> {
                        constexpr static const bool is_multi_dispatch = true;

                        using original_callback_type = RangeCallback;

                        static inline original_callback_type get_original_callback(
                                detail::exact_filter_within_range_filter_callback_unifier<
                                        Controller, Range, Exact, RangeCallback, ExactCallback
                                >& callback) {
                            return callback.range_callback;
                        }
                    };

                    template <typename Controller, typename TypeMap, typename Callbacks>
                    class append_exact_filter_callback_to_range_filter_callback_when_exact_within_range_pass {
                    private:
                        using impl = detail::append_exact_filter_callback_to_range_filter_callback_when_exact_within_range_pass_impl<
                                Controller, TypeMap, Callbacks
                        >;

                    public:
                        constexpr explicit append_exact_filter_callback_to_range_filter_callback_when_exact_within_range_pass(
                                Callbacks&& callbacks)
                                : new_callbacks(impl(std::forward<Callbacks>(callbacks)).new_callbacks) {}

                        using pass_category = veho::facet::receiver::optimization::normalization_pass_tag;

                        using updated_type_map_type = typename impl::updated_type_map_type;

                        using updated_callbacks_type = typename impl::updated_callbacks_type;

                        updated_callbacks_type new_callbacks;
                    };
                }
            }
        }
    }
}

#endif //VEHO_APPEND_EXACT_FILTER_CALLBACK_TO_RANGE_FILTER_CALLBACK_WHEN_EXACT_WITHIN_RANGE_PASS_HPP
