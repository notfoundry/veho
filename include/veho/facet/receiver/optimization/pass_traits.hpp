/*
* Created by Mark Johnson on 1/6/2018.
*/

#ifndef VEHO_OPTIMIZATION_PASS_TRAITS_HPP
#define VEHO_OPTIMIZATION_PASS_TRAITS_HPP

#include <cstddef>

#include <veho/frame.hpp>

namespace veho {
    namespace facet {
        namespace receiver {
            namespace optimization {
                struct preprocessing_pass_tag {};
                struct normalization_pass_tag {};
                struct reduction_pass_tag {};
                struct expansion_pass_tag {};

                namespace detail {
                    template <typename Pass>
                    struct abstract_pass_traits {
                        typedef typename Pass::updated_type_map_type updated_type_map_type;

                        typedef typename Pass::updated_callbacks_type updated_callbacks_type;
                    };

                    template <typename Pass, typename Tag = typename Pass::pass_category>
                    struct pass_traits_impl;

                    template <typename Pass>
                    struct pass_traits_impl<Pass, normalization_pass_tag> : abstract_pass_traits<Pass> {
                    };

                    template <typename Pass>
                    struct pass_traits_impl<Pass, reduction_pass_tag> : abstract_pass_traits<Pass> {
                        constexpr static const veho::common_frame::id_diff_type num_frames_wasted = Pass::num_frames_wasted;
                    };
                }

                template <typename Pass>
                struct pass_traits : detail::pass_traits_impl<Pass> {
                };
            }
        }
    }
}
#endif //VEHO_OPTIMIZATION_PASS_TRAITS_HPP
