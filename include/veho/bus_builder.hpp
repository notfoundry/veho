#ifndef VEHO_BUS_BUILDER_HPP
#define VEHO_BUS_BUILDER_HPP

#include <tuple>
#include <type_traits>

#include "config/builder_config.hpp"
#include "config/config_traits.hpp"
#include "config/config_postprocessing.hpp"

#include "controller/controller_traits.hpp"

#include "facet/builder_facet.hpp"
#include "facet/receiver/receiver_facet.hpp"
#include "facet/transmitter/transmitter_facet.hpp"

#include "bus_builder_fwd.hpp"
#include "bus_constructor.hpp"

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
        struct bus_builder_impl : builder_facet_enabler<Config> {
        };
    }

    template <typename Config>
    class bus_builder : public detail::bus_builder_impl<Config>, public builder_extensions<Config> {
    public:
        constexpr explicit bus_builder(Config&& config) : config(std::forward<Config>(config)) {}

        inline typename bus_constructor<typename config::postprocess_config<Config>::new_config_type>::bus_type
        build() const {
            return bus_constructor<typename config::postprocess_config<Config>::new_config_type>::construct(
                    config::postprocess_config<Config>(std::move(static_cast<Config>(config))).new_config);
        }

        constexpr inline typename config::postprocess_config<Config>::new_config_type get_processed_config() {
            return config::postprocess_config<Config>(std::move(static_cast<Config>(config))).new_config;
        }

    private:
        template <typename, typename>
        friend struct veho::facet::builder_facet;

        template <typename, typename>
        friend struct veho::builder_extensions;

        Config config;
    };

    template <typename Controller>
    constexpr inline bus_builder<config::builder_config<Controller>>
    bus_template() noexcept {
        return bus_builder<config::builder_config<Controller>>(config::builder_config<Controller>(std::make_tuple()));
    }
}

#endif //VEHO_BUS_BUILDER_HPP