#ifndef VEHO_BUS_TEMPLATE_BUILDER_HPP
#define VEHO_BUS_TEMPLATE_BUILDER_HPP

#include <tuple>
#include <type_traits>

#include "config/builder_config.hpp"
#include "config/config_traits.hpp"
#include "config/config_postprocessing.hpp"

#include "controller/controller_traits.hpp"

#include "facet/builder_facet.hpp"
#include "facet/receiver/receiver_facet.hpp"
#include "facet/transmitter/transmitter_facet.hpp"

#include "bus_template_builder_fwd.hpp"
#include "bus_template.hpp"

#include "controller/capabilities.hpp"

namespace veho {
    template <
            typename Config,
            typename Controller = typename veho::config::config_traits<Config>::controller_type
    > class builder_extensions {};

    namespace detail {
        template <typename Config, typename CapabilitiesList = typename controller::controller_traits<
                typename config::config_traits<Config>::controller_type
        >::capabilities>
        struct builder_facet_enabler;

        template <typename Config, template <class...> class List, typename... Capabilities>
        struct builder_facet_enabler<Config, List<Capabilities...>> : facet::builder_facet<Config, Capabilities>... {
        };

        template <typename Config>
        struct bus_template_builder_impl : builder_facet_enabler<Config> {
        };
    }

    template <typename Config>
    class bus_template_builder : public detail::bus_template_builder_impl<Config>, public builder_extensions<Config> {
    private:
        using config_postprocessor = config::postprocess_config<Config>;

        using post_processed_config = typename config_postprocessor::new_config_type;

        using controller = typename config::config_traits<Config>::controller_type;

    public:
        constexpr explicit bus_template_builder(Config&& config) : config(std::forward<Config>(config)) {}

        constexpr inline bus_template<post_processed_config>
        build() const {
            return bus_template<post_processed_config>(
                    config_postprocessor(std::move(static_cast<Config>(config))).new_config,
                    config::builder_config<controller>(std::make_tuple())
            );
        }

    private:
        template <typename, typename>
        friend struct veho::facet::builder_facet;

        template <typename, typename>
        friend struct veho::builder_extensions;

        Config config;
    };

    template <typename Controller>
    constexpr inline bus_template_builder<config::builder_config<Controller>>
    make_bus_template() noexcept {
        return bus_template_builder<config::builder_config<Controller>>(config::builder_config<Controller>(std::make_tuple()));
    }
}

#endif //VEHO_BUS_TEMPLATE_BUILDER_HPP