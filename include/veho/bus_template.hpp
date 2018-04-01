/*
* Created by Mark Johnson on 3/31/2018.
*/

#ifndef VEHO_BUS_TEMPLATE_HPP
#define VEHO_BUS_TEMPLATE_HPP

#include <utility>

#include <boost/mp11/list.hpp>
#include <boost/mp11/algorithm.hpp>

#include <veho/config/config_traits.hpp>

#include <veho/facet/receiver/receiver_facet_construction_requirement.hpp>

#include "bus_constructor.hpp"
#include "bus_construction_requirement.hpp"

namespace veho {
    namespace detail {
        template <
                typename Config, typename ArgsList, typename RequirementList,
                std::size_t RequirementsListSize = boost::mp11::mp_size<RequirementList>::value
        >
        struct enforce_construction_requirements_impl {
        private:
            using current_requirement = boost::mp11::mp_front<RequirementList>;

            using next_enforcement_step = enforce_construction_requirements_impl<
                    Config, ArgsList, boost::mp11::mp_pop_front<RequirementList>
            >;

        public:
            constexpr enforce_construction_requirements_impl()
                    : value((current_requirement(), next_enforcement_step().value)) {}

            bool value;
        };

        template <typename Config, typename ArgsList, typename RequirementList>
        struct enforce_construction_requirements_impl<Config, ArgsList, RequirementList, 0> {
            constexpr enforce_construction_requirements_impl() : value(true) {}

            bool value;
        };

        template <typename Config, typename... Args>
        struct enforce_construction_requirements {
            using args_list = boost::mp11::mp_list<Args...>;

            using capabilities = typename veho::config::config_traits<Config>::used_capabilities_set_type;

            template <typename Capability>
            using find_construction_requirement = veho::bus_construction_requirement<Config, args_list, Capability>;

            using requirements_list = boost::mp11::mp_transform<find_construction_requirement, capabilities>;

            constexpr enforce_construction_requirements()
                    : value(enforce_construction_requirements_impl<Config, args_list, requirements_list>().value) {}

            bool value;
        };
    }

    template <typename Config>
    struct bus_template {
        constexpr explicit bus_template(Config&& config) : config(std::forward<Config>(config)) {}

        template <typename... Args>
        inline typename bus_constructor<Config>::bus_type
        instantiate(Args&&... args) const {
            detail::enforce_construction_requirements<Config, Args...>();

            return bus_constructor<Config>::construct(std::move(static_cast<Config>(config)), std::forward<Args>(args)...);
        }

        Config config;
    };
}

#endif //VEHO_BUS_TEMPLATE_HPP
