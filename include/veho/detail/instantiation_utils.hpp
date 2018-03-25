/*
* Created by Mark Johnson on 3/18/2018.
*/

#ifndef VEHO_INSTANTIATION_UTILS_HPP
#define VEHO_INSTANTIATION_UTILS_HPP

#include <type_traits>

namespace veho {
    namespace detail {
        template <typename>
        struct false_if_instantiated : std::false_type {};
    }
}

#endif //VEHO_INSTANTIATION_UTILS_HPP
