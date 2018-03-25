/*
* Created by Mark Johnson on 3/25/2018.
*/

#ifndef VEHO_IRREDUCIBLE_MASK_FILTER_HANDLING_PASS_HPP
#define VEHO_IRREDUCIBLE_MASK_FILTER_HANDLING_PASS_HPP

#include <utility>
#include <limits>

#include <boost/mp11/map.hpp>
#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/list.hpp>

#include <veho/facet/receiver/optimization/pass_traits.hpp>

#include <veho/detail/matcher_utils.hpp>
#include <veho/detail/instantiation_utils.hpp>

namespace veho {
    namespace facet {
        namespace receiver {
            namespace optimization {
                namespace reduction {
                    template <typename TypeMap>
                    struct can_run_irreducible_mask_filter_handling_pass {
                        using matcher_list = boost::mp11::mp_map_keys<TypeMap>;

                        using non_mask_matchers = boost::mp11::mp_remove_if<matcher_list, veho::detail::is_mask>;

                        constexpr static const bool value = boost::mp11::mp_size<non_mask_matchers>::value == 0;
                    };

                    template <typename Controller, typename TypeMap, typename Callbacks,
                            typename = typename std::enable_if<can_run_irreducible_mask_filter_handling_pass<TypeMap>::value>::type>
                    class irreducible_mask_filter_handling_pass {
                    public:
                        static_assert(veho::detail::false_if_instantiated<Controller>::value,
                                      "Reduction failed: mask filter reduction is not yet implemented");

                        constexpr explicit irreducible_mask_filter_handling_pass(
                                Callbacks&& callbacks)
                                : new_callbacks(std::forward<Callbacks>(callbacks)) {}

                        using pass_category = veho::facet::receiver::optimization::reduction_pass_tag;

                        using updated_type_map_type = TypeMap;

                        using updated_callbacks_type = Callbacks;

                        constexpr static const veho::common_frame::id_diff_type num_frames_wasted
                                = std::numeric_limits<veho::common_frame::id_diff_type>::max();

                        updated_callbacks_type new_callbacks;
                    };
                }
            }
        }
    }
}

#endif //VEHO_IRREDUCIBLE_MASK_FILTER_HANDLING_PASS_HPP
