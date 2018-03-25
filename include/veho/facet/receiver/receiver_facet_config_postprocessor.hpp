/*
* Created by Mark Johnson on 3/24/2018.
*/

#ifndef VEHO_RECEIVER_FACET_CONFIG_POSTPROCESSOR_HPP
#define VEHO_RECEIVER_FACET_CONFIG_POSTPROCESSOR_HPP

#include <utility>

#include <veho/controller/capabilities.hpp>
#include <veho/controller/controller_traits.hpp>

#include <veho/config/config_traits.hpp>
#include <veho/config/config_postprocessor_fwd.hpp>

#include "receiver_facet_capability_data.hpp"
#include "optimization/optimization.hpp"

namespace veho {
    namespace facet {
        namespace receiver {
            namespace detail {
                template <typename Config, bool AreTransmittersPresent = veho::config::config_traits<Config>::template has_capability<
                        veho::controller::transmit_capability>::value>
                struct determine_available_mailbox_count {
                    constexpr static const std::size_t value = controller::controller_traits<
                            typename config::config_traits<Config>::controller_type
                    >::num_mailboxes;
                };

                template <typename Config>
                struct determine_available_mailbox_count<Config, true> {
                    constexpr static const std::size_t total_mailbox_count = controller::controller_traits<
                            typename config::config_traits<Config>::controller_type
                    >::num_mailboxes;

                    constexpr static const std::size_t transmitter_count = veho::config::config_traits<Config>::template capability_data_type<
                            veho::controller::transmit_capability>::num_transmitters;

                    constexpr static const std::size_t value = total_mailbox_count - transmitter_count;
                };
            }

            template <typename Config>
            struct receiver_facet_callback_optimization_config_processor {
                using old_receiver_capability_data = typename veho::config::config_traits<Config>::template capability_data_type<
                        veho::controller::receive_capability>;

                using optimizer = veho::facet::receiver::optimization::callback_optimizer<
                        typename veho::config::config_traits<Config>::controller_type,
                        typename old_receiver_capability_data::matcher_to_callback_map_type,
                        typename old_receiver_capability_data::callbacks_type,
                        detail::determine_available_mailbox_count<Config>::value
                >;

                using new_type_map_type = typename optimizer::new_type_map_type;

                using new_callbacks_type = typename optimizer::new_callbacks_type;

                constexpr explicit receiver_facet_callback_optimization_config_processor(Config&& config)
                        : new_config(config.template replace_capability_data<veho::controller::receive_capability>(
                                receiver_capability_data<new_type_map_type, new_callbacks_type>(
                                        optimizer(std::move(
                                                config.template capability_data_for<veho::controller::receive_capability>().callbacks
                                        )).new_callbacks
                                )
                        )
                ) {}

                using new_config_type = decltype(
                        std::declval<Config>().template replace_capability_data<veho::controller::receive_capability>(
                                std::declval<receiver_capability_data<new_type_map_type, new_callbacks_type>>()
                        )
                );

                new_config_type new_config;
            };
        }
    }

    namespace config {
        template <typename Config>
        struct config_postprocessor<Config, veho::controller::receive_capability>
                : veho::facet::receiver::receiver_facet_callback_optimization_config_processor<Config> {
            constexpr explicit config_postprocessor(Config&& config)
                    : veho::facet::receiver::receiver_facet_callback_optimization_config_processor<Config>(std::forward<Config>(config)) {}
        };
    }
}

#endif //VEHO_RECEIVER_FACET_CONFIG_POSTPROCESSOR_HPP
