/*
* Created by Mark Johnson on 3/3/2018.
*/

#ifndef VEHO_SAM3X_HPP
#define VEHO_SAM3X_HPP

#include <cstddef>
#include <utility>

#include <boost/mp11/list.hpp>

#include <veho/frame.hpp>
#include <veho/bus_template_builder.hpp>
#include <veho/bus_constructor.hpp>

#include <veho/controller/controller_traits.hpp>
#include <veho/controller/capabilities.hpp>

namespace sam3x {
    struct rev_8e {
        std::size_t callback_count;
        std::size_t transmitter_count;
    };
}

namespace veho {
    namespace controller {
        template <>
        struct controller_traits<sam3x::rev_8e> {
            constexpr const static std::size_t num_mailboxes = 8;

            using capabilities = boost::mp11::mp_list<
                    veho::controller::transmit_capability,
                    veho::controller::receive_capability
            >;
        };
    }

    template <>
    struct frame_extensions<sam3x::rev_8e> {
        using timestamp_type = uint16_t;
        using family_id_type = uint32_t;

        timestamp_type timestamp;
        family_id_type family_id;
    };

    template <typename Config>
    struct builder_extensions<Config, sam3x::rev_8e> {
        using updated_config_for_timestamping_type = Config;

        constexpr inline bus_template_builder<updated_config_for_timestamping_type> record_timestamps() {
            return bus_template_builder<updated_config_for_timestamping_type>(std::move(
                    const_cast<bus_template_builder<updated_config_for_timestamping_type>*>(
                            static_cast<const bus_template_builder<updated_config_for_timestamping_type>*>(
                                    this))->config
            ));
        }
    };

    template <typename Config>
    struct bus_constructor<Config, sam3x::rev_8e> {
        using bus_type = sam3x::rev_8e;

        template <typename... Args>
        static bus_type construct(Config&& config, Args&&... args) {
            std::size_t callback_count = 0, transmitter_count = 0;

            if (config.template has_capability<controller::receive_capability>()) {
                auto receiver_data = config.template capability_data_for<controller::receive_capability>();
                auto callbacks = receiver_data.callbacks;
                callback_count = boost::mp11::mp_size<decltype(callbacks)>::value;
            }

            if (config.template has_capability<controller::transmit_capability>()) {
                auto transmitter_data = config.template capability_data_for<controller::transmit_capability>();
                transmitter_count = transmitter_data.num_transmitters;
            }

            return bus_type{callback_count, transmitter_count};
        }
    };
}

#endif //VEHO_SAM3X_HPP
