/*
* Created by Mark Johnson on 3/24/2018.
*/

#ifndef VEHO_RECEIVER_FACET_RUNTIME_CAPABILITY_DATA_HPP
#define VEHO_RECEIVER_FACET_RUNTIME_CAPABILITY_DATA_HPP

#include <utility>
#include <tuple>

#include <boost/mp11/list.hpp>
#include <boost/mp11/set.hpp>

#include <veho/detail/tuple_utils.hpp>
#include <veho/detail/instantiation_utils.hpp>

namespace veho {
    namespace facet {
        namespace receiver {
            namespace detail {
                template <typename DependencySet, typename Dependency, bool Exists = boost::mp11::mp_set_contains<DependencySet, Dependency>::value>
                struct ensure_dependency_not_already_satisfied_impl {};

                template <typename DependencySet, typename Dependency>
                struct ensure_dependency_not_already_satisfied_impl<DependencySet, Dependency, true> {
                    static_assert(veho::detail::false_if_instantiated<DependencySet>::value,
                                  "Duplicate dependency exists in dependency set");
                };

                template <typename DependencySet, typename Dependency>
                struct ensure_dependency_not_already_satisfied
                        : ensure_dependency_not_already_satisfied_impl<DependencySet, Dependency> {};
            }

            template <typename DependencySet = std::tuple<>>
            struct receiver_runtime_capability_data {
            private:
                template <typename Dependency>
                using pushed_back_type = receiver_runtime_capability_data<boost::mp11::mp_push_back<DependencySet, Dependency>>;

            public:
                constexpr explicit receiver_runtime_capability_data(DependencySet&& dependency_set)
                        : dependency_set(std::forward<DependencySet>(dependency_set)) {}

                template <typename Dependency>
                constexpr inline pushed_back_type<Dependency>
                push_back(Dependency&& dependency) {
                    return (detail::ensure_dependency_not_already_satisfied<DependencySet, Dependency>(),
                            pushed_back_type<Dependency>(veho::detail::tuple_push_back(
                                    std::move(static_cast<DependencySet>(dependency_set)),
                                    std::forward<Dependency>(dependency)
                            )));
                }

                using dependency_set_type = DependencySet;

                dependency_set_type dependency_set;
            };

            template <typename Controller>
            constexpr inline receiver_runtime_capability_data<> make_receiver_runtime_capability_data() {
                return receiver_runtime_capability_data<>(std::make_tuple());
            }
        }
    }
}
#endif //VEHO_RECEIVER_FACET_RUNTIME_CAPABILITY_DATA_HPP
