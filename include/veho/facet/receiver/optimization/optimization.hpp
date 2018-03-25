/*
* Created by Mark Johnson on 1/4/2018.
*/

#ifndef VEHO_TRANSFORMATION_HPP
#define VEHO_TRANSFORMATION_HPP

#include <cstddef>
#include <utility>
#include <type_traits>

#include <boost/mp11/list.hpp>

#include "expansion/expansion.hpp"
#include "normalization/normalization.hpp"
#include "reduction/reduction.hpp"

namespace veho {
    namespace facet {
        namespace receiver {
            namespace optimization {
                namespace detail {
                    template <typename TypeMap, typename Callbacks>
                    struct dummy_optimizer {
                        constexpr explicit dummy_optimizer(Callbacks&& callbacks)
                                : new_callbacks(std::forward<Callbacks>(callbacks)) {}

                        using new_type_map_type = TypeMap;

                        using new_callbacks_type = Callbacks;

                        new_callbacks_type new_callbacks;
                    };

                    template <typename Controller, typename TypeMap, typename Callbacks, std::size_t UsableMailboxCount, typename Enable = void>
                    struct determine_optimization_process;

                    template <typename Controller, typename TypeMap, typename Callbacks, std::size_t UsableMailboxCount>
                    struct determine_optimization_process<Controller, TypeMap, Callbacks, UsableMailboxCount, typename std::enable_if<
                            (boost::mp11::mp_size<Callbacks>::value > UsableMailboxCount)
                    >::type> {
                        using optimizer = reduction::callback_reducer<
                                Controller,
                                TypeMap,
                                Callbacks,
                                UsableMailboxCount
                        >;
                    };

                    template <typename Controller, typename TypeMap, typename Callbacks, std::size_t UsableMailboxCount>
                    struct determine_optimization_process<Controller, TypeMap, Callbacks, UsableMailboxCount, typename std::enable_if<
                            (boost::mp11::mp_size<Callbacks>::value < UsableMailboxCount)
                    >::type> {
                        using optimizer = expansion::callback_expander<
                                Controller,
                                TypeMap,
                                Callbacks,
                                UsableMailboxCount
                        >;
                    };

                    template <typename Controller, typename TypeMap, typename Callbacks, std::size_t UsableMailboxCount>
                    struct determine_optimization_process<Controller, TypeMap, Callbacks, UsableMailboxCount, typename std::enable_if<
                            (boost::mp11::mp_size<Callbacks>::value == UsableMailboxCount)
                    >::type> {
                        using optimizer = dummy_optimizer<TypeMap, Callbacks>;
                    };
                }

                template <typename Controller, typename TypeMap, typename Callbacks, std::size_t UsableMailboxCount>
                struct callback_optimizer {
                    using normalizer = normalization::callback_normalizer<Controller, TypeMap, Callbacks>;

                    using normalized_type_map = typename normalizer::new_type_map_type;

                    using normalized_callbacks_type = typename normalizer::new_callbacks_type;

                    using optimization_process = typename detail::determine_optimization_process<
                            Controller, normalized_type_map, normalized_callbacks_type, UsableMailboxCount
                    >::optimizer;

                    constexpr explicit callback_optimizer(Callbacks&& callbacks)
                            : new_callbacks(optimization_process(normalizer(std::forward<Callbacks>(callbacks)).new_callbacks).new_callbacks) {}

                    using new_type_map_type = typename optimization_process::new_type_map_type;

                    using new_callbacks_type = typename optimization_process::new_callbacks_type;

                    new_callbacks_type new_callbacks;

//                    constexpr explicit callback_optimizer(Callbacks&& callbacks)
//                            : new_callbacks(normalizer(std::forward<Callbacks>(callbacks)).new_callbacks) {}
//
//                    using new_type_map_type = normalized_type_map;
//
//                    using new_callbacks_type = normalized_callbacks_type;
//
//                    new_callbacks_type new_callbacks;
                };
            }
        }
    }
}

#endif //VEHO_TRANSFORMATION_HPP
