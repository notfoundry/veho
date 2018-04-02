/*
* Created by Mark Johnson on 3/14/2018.
*/

#ifndef VEHO_BUS_CONSTRUCTOR_HPP
#define VEHO_BUS_CONSTRUCTOR_HPP

#include <type_traits>

#include "detail/instantiation_utils.hpp"

#include "controller/capabilities.hpp"

#include "config/config_traits.hpp"

#include "bus_template_fwd.hpp"

namespace veho {
    template <typename CompiletimeConfig, typename RuntimeConfig, typename Capability>
    struct bus_construction_requirement {};

    template <typename CompiletimeConfig, typename RuntimeConfig, typename Controller>
    struct bus_constructor {
        static_assert(veho::detail::false_if_instantiated<CompiletimeConfig>::value, "Bus constructor has not been defined for controller type");

        using bus_type = void;

        template <typename... Args>
        static bus_type construct(CompiletimeConfig&&, RuntimeConfig&&, Args&&...) {}
    };
}

#endif //VEHO_BUS_CONSTRUCTOR_HPP
