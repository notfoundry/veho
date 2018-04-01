/*
* Created by Mark Johnson on 3/31/2018.
*/

#ifndef VEHO_PREPROCESSING_HPP
#define VEHO_PREPROCESSING_HPP

#include <boost/mp11/list.hpp>
#include <boost/mp11/utility.hpp>

#include <veho/facet/receiver/optimization/pass_traits.hpp>

#include "unify_identically_filtered_callbacks_pass.hpp"

namespace veho {
    namespace facet {
        namespace receiver {
            namespace optimization {
                namespace preprocessing {
                    namespace detail {
                        template <typename Controller, typename TypeMap, typename Callbacks, typename PassList, std::size_t PassListSize = boost::mp11::mp_size<PassList>::value>
                        struct callback_preprocessor_impl {
                            using current_quoted_pass = boost::mp11::mp_front<PassList>;

                            using current_pass = boost::mp11::mp_invoke<current_quoted_pass, Controller, TypeMap, Callbacks>;

                            using current_pass_type_map_type = typename current_pass::updated_type_map_type;

                            using current_pass_callbacks_type = typename current_pass::updated_callbacks_type;

                            using next_stage = callback_preprocessor_impl<
                                    Controller,
                                    current_pass_type_map_type, current_pass_callbacks_type,
                                    boost::mp11::mp_pop_front<PassList>
                            >;

                            constexpr explicit callback_preprocessor_impl(Callbacks&& callbacks)
                                    : new_callbacks(next_stage(current_pass(std::forward<Callbacks>(callbacks)).new_callbacks).new_callbacks) {}

                            using updated_type_map_type = typename next_stage::updated_type_map_type;

                            using updated_callbacks_type = typename next_stage::updated_callbacks_type;

                            updated_callbacks_type new_callbacks;
                        };

                        template <typename Controller, typename TypeMap, typename Callbacks, typename PassList>
                        struct callback_preprocessor_impl<Controller, TypeMap, Callbacks, PassList, 0> {
                            constexpr explicit callback_preprocessor_impl(Callbacks&& callbacks)
                                    : new_callbacks(std::forward<Callbacks>(callbacks)) {}

                            using updated_type_map_type = TypeMap;

                            using updated_callbacks_type = Callbacks;

                            updated_callbacks_type new_callbacks;
                        };
                    }

                    template <typename Controller, typename TypeMap, typename Callbacks>
                    struct callback_preprocessor {
                    private:
                        using preprocessing_pass_list = boost::mp11::mp_list<
                                boost::mp11::mp_quote<unify_identically_filtered_callbacks_pass>
                        >;

                        using impl = detail::callback_preprocessor_impl<Controller, TypeMap, Callbacks, preprocessing_pass_list>;

                    public:
                        constexpr explicit callback_preprocessor(Callbacks&& callbacks)
                                : new_callbacks(impl(std::forward<Callbacks>(callbacks)).new_callbacks) {}

                        using new_type_map_type = typename impl::updated_type_map_type;

                        using new_callbacks_type = typename impl::updated_callbacks_type;

                        new_callbacks_type new_callbacks;
                    };
                }
            }
        }
    }
}

#endif //VEHO_PREPROCESSING_HPP
