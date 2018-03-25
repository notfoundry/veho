/*
* Created by Mark Johnson on 3/25/2018.
*/

#ifndef VEHO_CALLBACK_TRAITS_HPP
#define VEHO_CALLBACK_TRAITS_HPP

namespace veho {
    namespace facet {
        namespace receiver {
            namespace optimization {
                namespace normalization {
                    template <typename Callback>
                    struct normalized_callback_traits {
                        constexpr static const bool is_multi_dispatch = false;
                    };
                }
            }
        }
    }
}

#endif //VEHO_CALLBACK_TRAITS_HPP
