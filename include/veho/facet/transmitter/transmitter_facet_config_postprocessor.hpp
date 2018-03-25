/*
* Created by Mark Johnson on 3/24/2018.
*/

#ifndef VEHO_TRANSMITTER_FACET_CONFIG_POSTPROCESSOR_HPP
#define VEHO_TRANSMITTER_FACET_CONFIG_POSTPROCESSOR_HPP

#include <utility>

#include <veho/controller/capabilities.hpp>

#include <veho/config/config_traits.hpp>
#include <veho/config/config_postprocessor_fwd.hpp>

namespace veho {
    namespace facet {
        namespace transmitter {
            template <typename Config>
            struct transmitter_facet_validation_config_processor {
                constexpr explicit transmitter_facet_validation_config_processor(Config&& config)
                        : new_config(std::forward<Config>(config)) {}

                using new_config_type = Config;

                new_config_type new_config;
            };
        }
    }

    namespace config {
        template <typename Config>
        struct config_postprocessor<Config, veho::controller::transmit_capability>
                : veho::facet::transmitter::transmitter_facet_validation_config_processor<Config> {
            constexpr explicit config_postprocessor(Config&& config)
                    : veho::facet::transmitter::transmitter_facet_validation_config_processor<Config>(std::forward<Config>(config)) {}
        };
    }
}

#endif //VEHO_TRANSMITTER_FACET_CONFIG_POSTPROCESSOR_HPP
