/*
* Created by Mark Johnson on 3/3/2018.
*/

#ifndef VEHO_CONTROLLER_TRAITS_HPP
#define VEHO_CONTROLLER_TRAITS_HPP

#include <cstddef>

namespace veho {
    namespace controller {
        namespace detail {
            template <typename Controller>
            struct controller_traits_impl {
                constexpr const static std::size_t num_mailboxes = Controller::num_mailboxes;

                using capabilities = typename Controller::capabilities;
            };
        }

        template <typename Controller>
        struct controller_traits : detail::controller_traits_impl<Controller> {};
    }
}

#endif //VEHO_CONTROLLER_TRAITS_HPP
