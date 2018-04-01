/*
* Created by Mark on 3/19/2018.
*/

#ifndef VEHO_POST_PROCESSING_HPP
#define VEHO_POST_PROCESSING_HPP

#include <type_traits>

#include <boost/mp11/list.hpp>
#include <boost/mp11/utility.hpp>
#include <boost/mp11/algorithm.hpp>

#include <veho/controller/controller_traits.hpp>
#include <veho/controller/capabilities.hpp>

#include <veho/facet/receiver/receiver_facet_config_postprocessor.hpp>
#include <veho/facet/transmitter/transmitter_facet_config_postprocessor.hpp>

#include "config_postprocessor_fwd.hpp"
#include "config_traits.hpp"

namespace veho {
    namespace config {
        namespace detail {
            template <typename Config, typename UnconfiguredProcessorList, std::size_t ProcessorListSize = boost::mp11::mp_size<UnconfiguredProcessorList>::value>
            struct config_postprocessing_impl {
                using current_unconfigured_processor = boost::mp11::mp_front<UnconfiguredProcessorList>;

                using current_processor = boost::mp11::mp_invoke<current_unconfigured_processor, Config>;

                using next_postprocessing_stage_config = typename current_processor::new_config_type;

                static_assert(std::is_same<
                        typename veho::config::config_traits<Config>::used_capabilities_set_type,
                        typename veho::config::config_traits<next_postprocessing_stage_config>::used_capabilities_set_type
                >::value, "Capabilities in config were changed during postprocessing");

                using next_postprocessing_stage = config_postprocessing_impl<
                        next_postprocessing_stage_config,
                        boost::mp11::mp_pop_front<UnconfiguredProcessorList>
                >;

                constexpr explicit config_postprocessing_impl(Config&& config)
                        : new_config(
                        next_postprocessing_stage(
                                current_processor(std::forward<Config>(config)).new_config
                        ).new_config
                ) {}

                using new_config_type = typename next_postprocessing_stage::new_config_type;

                new_config_type new_config;
            };

            template <typename Config, typename UnconfiguredProcessorList>
            struct config_postprocessing_impl<Config, UnconfiguredProcessorList, 0> {
                constexpr explicit config_postprocessing_impl(Config&& config) : new_config(std::forward<Config>(config)) {}

                using new_config_type = Config;

                new_config_type new_config;
            };
        }

        template <typename Config>
        struct postprocess_config {
            using used_capabilities = typename veho::config::config_traits<Config>::used_capabilities_set_type;

            template <typename Capability>
            using unconfigured_postprocessor_from_capability = boost::mp11::mp_bind_back<
                    veho::config::config_postprocessor,
                    Capability
            >;

            using valid_unconfigured_postprocessors = boost::mp11::mp_transform<
                    unconfigured_postprocessor_from_capability,
                    used_capabilities
            >;

            using postprocessor_impl = detail::config_postprocessing_impl<Config, valid_unconfigured_postprocessors>;

            constexpr explicit postprocess_config(Config&& config)
                    : new_config(postprocessor_impl(std::forward<Config>(config)).new_config) {}

            using new_config_type = typename postprocessor_impl::new_config_type;

            new_config_type new_config;
        };
    }
}

#endif //VEHO_POST_PROCESSING_HPP
