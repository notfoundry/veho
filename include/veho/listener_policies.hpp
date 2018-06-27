/*
* Created by Mark Johnson on 5/7/2018.
*/

#ifndef VEHO_LISTENER_FACETS_HPP
#define VEHO_LISTENER_FACETS_HPP

#include <cstddef>

namespace veho {
    namespace listener_policies {
        namespace retention {
            template <std::size_t FramesToRetain>
            struct most_recent {};

            template <std::size_t FramesToRetain>
            struct discard_after {};
        }

        namespace launch {
            struct enqueue {};

            struct realtime {};
        }

        template <typename RetentionPolicy>
        struct retention_policy {};

        template <typename ExecutionPolicy>
        struct execution_policy {};
    }
}

#endif //VEHO_LISTENER_FACETS_HPP
