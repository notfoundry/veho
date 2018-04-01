/*
* Created by Mark Johnson on 1/4/2018.
*/

#ifndef VEHO_TRANSFORMATION_HPP
#define VEHO_TRANSFORMATION_HPP

#include <cstddef>
#include <utility>
#include <type_traits>

#include <boost/mp11/list.hpp>

#include "preprocessing/preprocessing.hpp"
#include "normalization/normalization.hpp"
#include "expansion/expansion.hpp"
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
                private:
                    using preprocessor = preprocessing::callback_preprocessor<Controller, TypeMap, Callbacks>;

                    using preprocessed_type_map = typename preprocessor::new_type_map_type;

                    using preprocessed_callbacks_type = typename preprocessor::new_callbacks_type;


                    using normalizer = normalization::callback_normalizer<Controller, preprocessed_type_map, preprocessed_callbacks_type>;

                    using normalized_type_map = typename normalizer::new_type_map_type;

                    using normalized_callbacks_type = typename normalizer::new_callbacks_type;


                    using optimizer = typename detail::determine_optimization_process<
                            Controller, normalized_type_map, normalized_callbacks_type, UsableMailboxCount
                    >::optimizer;

                public:
                    constexpr explicit callback_optimizer(Callbacks&& callbacks)
                            : new_callbacks(optimizer(normalizer(std::forward<Callbacks>(callbacks)).new_callbacks).new_callbacks) {}

                    using new_type_map_type = typename optimizer::new_type_map_type;

                    using new_callbacks_type = typename optimizer::new_callbacks_type;

                    new_callbacks_type new_callbacks;
                };
            }
        }
    }
}

#endif //VEHO_TRANSFORMATION_HPP
