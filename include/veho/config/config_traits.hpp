/*
* Created by Mark Johnson on 3/3/2018.
*/

#ifndef VEHO_CONFIG_TRAITS_HPP
#define VEHO_CONFIG_TRAITS_HPP

#include <utility>

#include <boost/mp11/map.hpp>
#include <boost/mp11/list.hpp>

#include <veho/detail/tuple_utils.hpp>

#include "detail/add_data_for_capability_in_config.hpp"
#include "detail/replace_data_for_capability_in_config.hpp"

namespace veho {
    namespace config {
        namespace detail {
            template <typename Controller, typename CapabilityMap, typename CapabilityDataSet>
            struct config_traits_impl {
                using controller_type = Controller;

                using capability_map_type = CapabilityMap;

                using capability_data_set_type = CapabilityDataSet;

                using used_capabilities_set_type = boost::mp11::mp_map_keys<capability_map_type>;

                template <typename Capability>
                using has_capability = boost::mp11::mp_map_contains<capability_map_type, Capability>;

                template <typename Capability>
                using capability_data_type = boost::mp11::mp_second<boost::mp11::mp_map_find<capability_map_type, Capability>>;
            };
        }

        template <typename Config>
        struct config_traits : detail::config_traits_impl<
                typename Config::controller_type,
                typename Config::capability_map_type,
                typename Config::capability_data_set_type> {
        };
    }
}

#endif //VEHO_CONFIG_TRAITS_HPP
