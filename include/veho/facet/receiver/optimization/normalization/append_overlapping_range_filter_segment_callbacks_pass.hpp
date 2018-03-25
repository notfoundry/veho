/*
* Created by Mark Johnson on 3/25/2018.
*/

#ifndef VEHO_APPEND_OVERLAPPING_RANGE_FILTER_SEGMENT_CALLBACKS_PASS_HPP
#define VEHO_APPEND_OVERLAPPING_RANGE_FILTER_SEGMENT_CALLBACKS_PASS_HPP

#include <utility>

#include <veho/facet/receiver/optimization/pass_traits.hpp>

namespace veho {
    namespace facet {
        namespace receiver {
            namespace optimization {
                namespace normalization {
                    template <typename Controller, typename TypeMap, typename Callbacks>
                    class append_overlapping_range_filter_segment_callbacks_pass {
                    public:
                        constexpr explicit append_overlapping_range_filter_segment_callbacks_pass(
                                Callbacks&& callbacks)
                                : new_callbacks(std::forward<Callbacks>(callbacks)) {}

                        using pass_category = veho::facet::receiver::optimization::normalization_pass_tag;

                        using updated_type_map_type = TypeMap;

                        using updated_callbacks_type = Callbacks;

                        updated_callbacks_type new_callbacks;
                    };
                }
            }
        }
    }
}


#endif //VEHO_APPEND_OVERLAPPING_RANGE_FILTER_SEGMENT_CALLBACKS_PASS_HPP
