/*
* Created by Mark Johnson on 3/3/2018.
*/

#ifndef VEHO_EXPANSION_HPP
#define VEHO_EXPANSION_HPP

#include <utility>

namespace veho {
    namespace facet {
        namespace receiver {
            namespace optimization {
                namespace expansion {
                    template <typename Controller, typename TypeMap, typename Callbacks, std::size_t UsableMailboxCount>
                    struct callback_expander {

                        constexpr explicit callback_expander(Callbacks&& callbacks)
                                : new_callbacks(std::forward<Callbacks>(callbacks)) {}

                        using new_type_map_type = TypeMap;

                        using new_callbacks_type = Callbacks;

                        new_callbacks_type new_callbacks;
                    };
                }
            }
        }
    }
}

#endif //VEHO_EXPANSION_HPP
