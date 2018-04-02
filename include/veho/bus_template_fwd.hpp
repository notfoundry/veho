/*
* Created by Mark Johnson on 4/2/2018.
*/

#ifndef VEHO_BUS_TEMPLATE_FWD_HPP
#define VEHO_BUS_TEMPLATE_FWD_HPP

#include <veho/config/config_traits.hpp>
#include <veho/config/builder_config.hpp>

namespace veho {
    template <
            typename CompiletimeConfig,
            typename RuntimeConfig = config::builder_config<typename config::config_traits<CompiletimeConfig>::controller_type>
    >
    struct bus_template;
}

#endif //VEHO_BUS_TEMPLATE_FWD_HPP
