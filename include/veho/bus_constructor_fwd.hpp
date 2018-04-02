/*
* Created by Mark Johnson on 4/2/2018.
*/

#ifndef VEHO_BUS_CONSTRUCTOR_FWD_HPP
#define VEHO_BUS_CONSTRUCTOR_FWD_HPP

#include <veho/config/config_traits.hpp>

namespace veho {
    template <typename CompiletimeConfig, typename RuntimeConfig, typename Capability>
    struct bus_construction_requirement;

    template <
            typename CompiletimeConfig, typename RuntimeConfig,
            typename Controller = typename veho::config::config_traits<CompiletimeConfig>::controller_type
    >
    struct bus_constructor;
}

#endif //VEHO_BUS_CONSTRUCTOR_FWD_HPP
