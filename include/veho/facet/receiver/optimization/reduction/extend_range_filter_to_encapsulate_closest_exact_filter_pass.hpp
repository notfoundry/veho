/*
* Created by Mark Johnson on 1/20/2018.
*/

#ifndef VEHO_EXTEND_RANGE_FILTER_TO_ENCAPSULATE_CLOSEST_EXACT_FILTER_PASS_HPP
#define VEHO_EXTEND_RANGE_FILTER_TO_ENCAPSULATE_CLOSEST_EXACT_FILTER_PASS_HPP

#include <utility>
#include <limits>
#include <type_traits>

#include <boost/mp11/map.hpp>
#include <boost/mp11/list.hpp>
#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/integral.hpp>

#include <veho/facet/receiver/optimization/pass_traits.hpp>

namespace veho {
    namespace facet {
        namespace receiver {
            namespace optimization {
                namespace reduction {
                    namespace detail {
                        template <typename Controller, typename TypeMap, typename Callbacks>
                        struct extend_range_filter_to_encapsulate_closest_exact_filter_pass_impl {
                            constexpr explicit extend_range_filter_to_encapsulate_closest_exact_filter_pass_impl(Callbacks&& callbacks)
                                    : new_callbacks(std::forward<Callbacks>(callbacks)) {}

                            using updated_type_map_type = TypeMap;

                            using updated_callbacks_type = Callbacks;

                            constexpr static const veho::common_frame::id_diff_type num_frames_wasted
                                    = std::numeric_limits<veho::common_frame::id_diff_type>::max();

                            updated_callbacks_type new_callbacks;
                        };
                    }

                    template <typename TypeMap>
                    struct can_run_extend_range_filter_to_encapsulate_closest_exact_filter_pass {
                        using matcher_list = boost::mp11::mp_map_keys<TypeMap>;

                        template <typename Sequence, std::size_t MinSize>
                        using is_size_of_at_least = boost::mp11::mp_bool<boost::mp11::mp_size<Sequence>::value >= MinSize>;

                        using partitioned_list = boost::mp11::mp_partition<matcher_list, veho::detail::is_exact>;

                        constexpr static const bool does_at_least_one_exact_exist
                                = is_size_of_at_least<boost::mp11::mp_first<partitioned_list>, 1>::value;

                        constexpr static const bool does_at_least_one_range_exist
                                = is_size_of_at_least<boost::mp11::mp_second<partitioned_list>, 1>::value;

                        constexpr static const bool value =
                                does_at_least_one_exact_exist && does_at_least_one_range_exist;
                    };

                    template <typename Controller, typename TypeMap, typename Callbacks,
                            typename = typename std::enable_if<can_run_extend_range_filter_to_encapsulate_closest_exact_filter_pass<TypeMap>::value>::type>
                    class extend_range_filter_to_encapsulate_closest_exact_filter_pass {
                    private:
                        using impl = detail::extend_range_filter_to_encapsulate_closest_exact_filter_pass_impl<
                                Controller, TypeMap, Callbacks
                        >;
                    public:
                        constexpr explicit extend_range_filter_to_encapsulate_closest_exact_filter_pass(
                                Callbacks&& callbacks)
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

#endif //VEHO_EXTEND_RANGE_FILTER_TO_ENCAPSULATE_CLOSEST_EXACT_FILTER_PASS_HPP
