/*
* Created by Mark Johnson on 3/31/2018.
*/

#ifndef VEHO_RECEIVER_FACET_CONSTRUCTION_REQUIREMENT_HPP
#define VEHO_RECEIVER_FACET_CONSTRUCTION_REQUIREMENT_HPP

#include <utility>
#include <type_traits>

#include <boost/mp11/list.hpp>
#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/function.hpp>

#include <veho/controller/capabilities.hpp>

#include <veho/config/config_traits.hpp>

#include <veho/detail/instantiation_utils.hpp>

#include <veho/bus_constructor_fwd.hpp>

namespace veho {
    template <typename CompiletimeConfig, typename RuntimeConfig>
    struct bus_construction_requirement<CompiletimeConfig, RuntimeConfig, veho::controller::receive_capability> {
        using required_dependency_set = typename veho::config::config_traits<CompiletimeConfig>
                ::template capability_data_type<veho::controller::receive_capability>::dependency_set_type;

        using supplied_dependency_set = typename veho::config::config_traits<RuntimeConfig>
                ::template capability_data_type<veho::controller::receive_capability>::dependency_set_type;

        template <typename T>
        using to_cv_unqualified_type = typename std::remove_const<
                typename std::remove_reference<T>::type
        >::type;

        using unqualified_supplied_dependency_set = boost::mp11::mp_transform<to_cv_unqualified_type, supplied_dependency_set>;

        template <typename Dependency>
        using is_dependency_present = boost::mp11::mp_contains<unqualified_supplied_dependency_set, Dependency>;

        static_assert(boost::mp11::mp_all_of<required_dependency_set, is_dependency_present>::value,
                      "Not all callback dependencies satisfied during bus construction");
    };
}

#endif //VEHO_RECEIVER_FACET_CONSTRUCTION_REQUIREMENT_HPP
