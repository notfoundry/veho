/*
* Created by Mark Johnson on 3/31/2018.
*/

#ifndef VEHO_RECEIVER_FACET_CONSTRUCTION_REQUIREMENT_HPP
#define VEHO_RECEIVER_FACET_CONSTRUCTION_REQUIREMENT_HPP

#include <utility>
#include <type_traits>

#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/function.hpp>

#include <veho/controller/capabilities.hpp>

#include <veho/config/config_traits.hpp>

#include <veho/detail/instantiation_utils.hpp>

#include <veho/bus_construction_requirement.hpp>

namespace veho {
    namespace facet {
        namespace receiver {
            namespace detail {
                template <typename DependencySet, typename ArgsList>
                struct ensure_dependency_set_in_args {
                    template <typename Dependency>
                    using is_dependency_present = boost::mp11::mp_contains<ArgsList, Dependency>;

                    static_assert(boost::mp11::mp_all_of<DependencySet, is_dependency_present>::value,
                                  "Callback dependency not satisfied during bus construction");
                };
            }
        }
    }

    template <typename Config, typename ArgsList>
    struct bus_construction_requirement<Config, ArgsList, veho::controller::receive_capability> {
        using dependency_set = typename veho::config::config_traits<Config>
                ::template capability_data_type<veho::controller::receive_capability>::dependency_set_type;

        template <typename T>
        using to_cv_unqualified_type = typename std::remove_const<
                typename std::remove_reference<T>::type
        >::type;

        using unqualified_args_list = boost::mp11::mp_transform<to_cv_unqualified_type, ArgsList>;

        using dependency_check = facet::receiver::detail::ensure_dependency_set_in_args<dependency_set, unqualified_args_list>;
    };
}

#endif //VEHO_RECEIVER_FACET_CONSTRUCTION_REQUIREMENT_HPP
