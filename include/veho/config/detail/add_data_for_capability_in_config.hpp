/*
* Created by Mark Johnson on 3/18/2018.
*/

#ifndef VEHO_ADD_DATA_FOR_CAPABILITY_IN_CONFIG_HPP
#define VEHO_ADD_DATA_FOR_CAPABILITY_IN_CONFIG_HPP

#include <type_traits>
#include <utility>
#include <tuple>

#include <boost/mp11/list.hpp>

#include <veho/detail/tuple_utils.hpp>

namespace veho {
    namespace config {
        namespace detail {
            template <typename CapabilityMap, typename CapabilityDataSet, typename Capability, typename CapabilityData>
            struct add_data_for_capability {
                constexpr add_data_for_capability(CapabilityDataSet&& capability_data_set, CapabilityData&& capability_data)
                        : new_capability_data_set(veho::detail::tuple_push_back(std::forward<CapabilityDataSet>(capability_data_set), std::forward<CapabilityData>(capability_data))) {}

                using new_capability_map = boost::mp11::mp_push_back<CapabilityMap, std::pair<Capability, CapabilityData>>;

                using new_capability_data_set_type = boost::mp11::mp_push_back<CapabilityDataSet, CapabilityData>;

                new_capability_data_set_type new_capability_data_set;
            };
        }
    }
}

#endif //VEHO_ADD_DATA_FOR_CAPABILITY_IN_CONFIG_HPP
