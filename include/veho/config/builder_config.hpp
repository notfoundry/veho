/*
* Created by Mark Johnson on 3/14/2018.
*/

#ifndef VEHO_BUILDER_CONFIG_HPP
#define VEHO_BUILDER_CONFIG_HPP

#include <utility>
#include <type_traits>

#include <boost/mp11/map.hpp>
#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/list.hpp>

#include <veho/detail/tuple_utils.hpp>
#include <veho/detail/instantiation_utils.hpp>

#include "detail/replace_data_for_capability_in_config.hpp"
#include "detail/add_data_for_capability_in_config.hpp"

namespace veho {
    namespace config {
        namespace detail {
            template <typename CapabilityMap, typename Capability, bool IsCapabilityAlreadyPresent = boost::mp11::mp_map_contains<CapabilityMap, Capability>::value>
            struct ensure_capability_not_already_present_in_map {};

            template <typename CapabilityMap, typename Capability>
            struct ensure_capability_not_already_present_in_map<CapabilityMap, Capability, true> {
                static_assert(veho::detail::false_if_instantiated<Capability>::value, "Capability to be added is already present in configuration");
            };
        }

        template <typename Controller, typename CapabilityMap = boost::mp11::mp_list<>, typename CapabilityDataSet = std::tuple<>>
        class builder_config {
        public:
            constexpr explicit builder_config(CapabilityDataSet&& capability_data) : capability_data_set(std::forward<CapabilityDataSet>(capability_data)) {}

            using controller_type = Controller;

            using capability_map_type = CapabilityMap;

            using capability_data_set_type = CapabilityDataSet;

            static_assert(std::is_same<
                    boost::mp11::mp_rename<capability_data_set_type, boost::mp11::mp_list>,
                            boost::mp11::mp_rename<boost::mp11::mp_transform<boost::mp11::mp_second, capability_map_type>, boost::mp11::mp_list>
                          >::value,
                          "Capability mapping values and capability data set do not hold the same types");

            template <typename Capability>
            constexpr inline bool has_capability() {
                return boost::mp11::mp_map_contains<capability_map_type, Capability>::value;
            }

            template <typename Capability>
            constexpr inline boost::mp11::mp_second<boost::mp11::mp_map_find<CapabilityMap, Capability>>
            capability_data_for() const {
                using facet_data_type = boost::mp11::mp_second<boost::mp11::mp_map_find<CapabilityMap, Capability>>;
                return veho::detail::get_tuple_elem_by_type<facet_data_type>(capability_data_set);
            };

            template <typename Capability, typename NewCapabilityData>
            constexpr inline builder_config<
                    Controller,
                    typename detail::replace_data_for_capability<capability_map_type, CapabilityDataSet, Capability, NewCapabilityData>::new_capability_map,
                    typename detail::replace_data_for_capability<capability_map_type, CapabilityDataSet, Capability, NewCapabilityData>::new_capability_data_set_type
            > replace_capability_data(NewCapabilityData&& new_capability_data) {
                return builder_config<
                        Controller,
                        typename detail::replace_data_for_capability<capability_map_type, CapabilityDataSet, Capability, NewCapabilityData>::new_capability_map,
                        typename detail::replace_data_for_capability<capability_map_type, CapabilityDataSet, Capability, NewCapabilityData>::new_capability_data_set_type
                >(detail::replace_data_for_capability<capability_map_type, CapabilityDataSet, Capability, NewCapabilityData>(
                        std::move(static_cast<CapabilityDataSet>(capability_data_set)), std::forward<NewCapabilityData>(new_capability_data)).new_capability_data_set);
            };

            template <typename Capability, typename CapabilityData>
            constexpr inline builder_config<
                    Controller,
                    typename detail::add_data_for_capability<capability_map_type, CapabilityDataSet, Capability, CapabilityData>::new_capability_map,
                    typename detail::add_data_for_capability<capability_map_type, CapabilityDataSet, Capability, CapabilityData>::new_capability_data_set_type
            > add_capability_data(CapabilityData&& capability_data) {
                return (detail::ensure_capability_not_already_present_in_map<capability_map_type, Capability>(),
                        builder_config<
                                Controller,
                                typename detail::add_data_for_capability<capability_map_type, CapabilityDataSet, Capability, CapabilityData>::new_capability_map,
                                typename detail::add_data_for_capability<capability_map_type, CapabilityDataSet, Capability, CapabilityData>::new_capability_data_set_type
                        >(detail::add_data_for_capability<capability_map_type, CapabilityDataSet, Capability, CapabilityData>(
                                std::move(static_cast<CapabilityDataSet>(capability_data_set)), std::forward<CapabilityData>(capability_data)).new_capability_data_set));
            };

            constexpr inline CapabilityDataSet get_all_capability_data() const {
                return capability_data_set;
            }

            CapabilityDataSet capability_data_set;
        };

        template <typename Controller>
        constexpr inline builder_config<Controller> make_config() {
            return builder_config<Controller>(std::make_tuple());
        }
    }
}

#endif //VEHO_BUILDER_CONFIG_HPP
