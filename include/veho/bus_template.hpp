/*
* Created by Mark Johnson on 3/31/2018.
*/

#ifndef VEHO_BUS_TEMPLATE_HPP
#define VEHO_BUS_TEMPLATE_HPP

#include <utility>

#include <boost/mp11/list.hpp>
#include <boost/mp11/algorithm.hpp>

#include <veho/config/config_traits.hpp>
#include <veho/config/builder_config.hpp>

#include <veho/facet/template_facet.hpp>

#include <veho/facet/receiver/receiver_facet_construction_requirement.hpp>

#include "bus_template_fwd.hpp"
#include "bus_constructor.hpp"

namespace veho {
    namespace detail {
        template <
                typename CompiletimeConfig, typename RuntimeConfig, typename RequirementList,
                std::size_t RequirementsListSize = boost::mp11::mp_size<RequirementList>::value
        >
        struct enforce_construction_requirements_impl {
        private:
            using current_requirement = boost::mp11::mp_front<RequirementList>;

            using next_enforcement_step = enforce_construction_requirements_impl<
                    CompiletimeConfig, RuntimeConfig, boost::mp11::mp_pop_front<RequirementList>
            >;

        public:
            constexpr enforce_construction_requirements_impl()
                    : value((current_requirement(), next_enforcement_step().value)) {}

            bool value;
        };

        template <typename CompiletimeConfig, typename RuntimeConfig, typename RequirementList>
        struct enforce_construction_requirements_impl<CompiletimeConfig, RuntimeConfig, RequirementList, 0> {
            constexpr enforce_construction_requirements_impl() : value(true) {}

            bool value;
        };

        template <typename CompiletimeConfig, typename RuntimeConfig>
        struct enforce_construction_requirements {
            using capabilities = typename veho::config::config_traits<CompiletimeConfig>::used_capabilities_set_type;

            template <typename Capability>
            using find_construction_requirement = veho::bus_construction_requirement<CompiletimeConfig, RuntimeConfig, Capability>;

            using requirements_list = boost::mp11::mp_transform<find_construction_requirement, capabilities>;

            constexpr enforce_construction_requirements()
                    : value(enforce_construction_requirements_impl<CompiletimeConfig, RuntimeConfig, requirements_list>().value) {}

            bool value;
        };

        template <
                typename CompiletimeConfig, typename RuntimeConfig,
                typename CapabilitiesList = typename controller::controller_traits<
                        typename config::config_traits<CompiletimeConfig>::controller_type
                >::capabilities>
        struct template_facet_enabler;

        template <typename CompiletimeConfig, typename RuntimeConfig, template <class...> class List, typename... Capabilities>
        struct template_facet_enabler<CompiletimeConfig, RuntimeConfig, List<Capabilities...>>
                : facet::template_facet<CompiletimeConfig, RuntimeConfig, Capabilities>... {
        };

        template <typename CompiletimeConfig, typename RuntimeConfig>
        struct bus_template_impl : template_facet_enabler<CompiletimeConfig, RuntimeConfig> {
        };
    }

    template <typename CompiletimeConfig, typename RuntimeConfig>
    struct bus_template : detail::bus_template_impl<CompiletimeConfig, RuntimeConfig> {
    public:
        constexpr explicit bus_template(CompiletimeConfig&& compiletime_config, RuntimeConfig&& runtime_config)
                : runtime_config(std::forward<RuntimeConfig>(runtime_config)),
                  compiletime_config(std::forward<CompiletimeConfig>(compiletime_config)) {}

        template <typename... Args>
        inline typename bus_constructor<CompiletimeConfig, RuntimeConfig>::bus_type
        instantiate(Args&&... args) const {
            detail::enforce_construction_requirements<CompiletimeConfig, RuntimeConfig>();

            return bus_constructor<CompiletimeConfig, RuntimeConfig>::construct(
                    std::move(static_cast<CompiletimeConfig>(compiletime_config)),
                    std::move(static_cast<RuntimeConfig>(runtime_config)),
                    std::forward<Args>(args)...
            );
        }

    private:
        template <typename, typename, typename>
        friend struct veho::facet::template_facet;

        CompiletimeConfig compiletime_config;

        RuntimeConfig runtime_config;
    };
}

#endif //VEHO_BUS_TEMPLATE_HPP
