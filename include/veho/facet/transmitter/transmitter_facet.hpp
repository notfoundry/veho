/*
* Created by Mark Johnson on 3/14/2018.
*/

#ifndef VEHO_TRANSMITTER_FACET_HPP
#define VEHO_TRANSMITTER_FACET_HPP

#include <utility>
#include <type_traits>

#include <veho/bus_builder_fwd.hpp>

#include <veho/config/config_traits.hpp>

#include <veho/detail/instantiation_utils.hpp>

#include <veho/controller/controller_traits.hpp>
#include <veho/controller/capabilities.hpp>

#include "transmitter_facet_config_postprocessor.hpp"
#include "transmitter_facet_capability_data.hpp"

namespace veho {
    namespace facet {
        namespace transmitter {
            namespace detail {
                template <typename Config, std::size_t NumTransmitters, typename Enable = void>
                struct update_config_after_specifying_transmitter_count {
                    constexpr explicit update_config_after_specifying_transmitter_count(Config&& config)
                            : updated_config(config.template add_capability_data<veho::controller::transmit_capability>(
                            transmitter_capability_data<NumTransmitters>())) {}

                    using updated_config_type = decltype(std::declval<Config>().template add_capability_data<veho::controller::transmit_capability>(
                            std::declval<transmitter_capability_data<NumTransmitters>>()
                    ));

                    updated_config_type updated_config;
                };

                template <typename Config, std::size_t NumTransmitters>
                struct update_config_after_specifying_transmitter_count<Config, NumTransmitters, typename std::enable_if<
                        (NumTransmitters >
                         veho::controller::controller_traits<typename veho::config::config_traits<Config>::controller_type>::num_mailboxes)
                >::type> {
                    static_assert(veho::detail::false_if_instantiated<Config>::value,
                                  "More transmitters have been requested than there are available mailboxes");
                };

                template <typename Config, bool HasTransmitterCountBeenSpecified = veho::config::config_traits<Config>::template has_capability<controller::transmit_capability>::value>
                struct ensure_transmitter_count_not_previously_specified {
                };

                template <typename Config>
                struct ensure_transmitter_count_not_previously_specified<Config, true> {
                    static_assert(veho::detail::false_if_instantiated<Config>::value,
                                  "Transmitter count has already been specified");
                };

            }
        }

        template <typename Config>
        struct builder_facet<Config, controller::transmit_capability> {
        private:
            template <std::size_t NumTransmitters>
            using config_updater = transmitter::detail::update_config_after_specifying_transmitter_count<Config, NumTransmitters>;

        public:
            template <std::size_t NumTransmitters>
            constexpr veho::bus_builder<typename config_updater<NumTransmitters>::updated_config_type>
            with_transmitters() {
                return (transmitter::detail::ensure_transmitter_count_not_previously_specified<Config>(),
                        bus_builder<typename config_updater<NumTransmitters>::updated_config_type>(
                                config_updater<NumTransmitters>(
                                        std::move(
                                                const_cast<bus_builder<Config>*>(static_cast<const bus_builder<Config>*>(this))->config)
                                ).updated_config));
            }
        };
    }
}

#endif //VEHO_TRANSMITTER_FACET_HPP
