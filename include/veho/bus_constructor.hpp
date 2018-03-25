/*
* Created by Mark Johnson on 3/14/2018.
*/

#ifndef VEHO_BUS_CONSTRUCTOR_HPP
#define VEHO_BUS_CONSTRUCTOR_HPP

#include <type_traits>

#include "detail/instantiation_utils.hpp"
#include "controller/capabilities.hpp"
#include "config/config_traits.hpp"

namespace veho {
    template <typename Config, typename Controller = typename veho::config::config_traits<Config>::controller_type>
    struct bus_constructor {
        static_assert(veho::detail::false_if_instantiated<Config>::value, "Bus constructor has not been defined for controller type");

        using bus_type = void;

        static bus_type construct(Config&&) {}
    };
}

#endif //VEHO_BUS_CONSTRUCTOR_HPP
