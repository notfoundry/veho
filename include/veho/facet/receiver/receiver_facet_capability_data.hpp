/*
* Created by Mark Johnson on 3/24/2018.
*/

#ifndef VEHO_RECEIVER_FACET_CAPABILITY_DATA_HPP
#define VEHO_RECEIVER_FACET_CAPABILITY_DATA_HPP

#include <utility>

#include <boost/mp11/list.hpp>
#include <boost/mp11/map.hpp>

#include <veho/detail/tuple_utils.hpp>

namespace veho {
    namespace facet {
        namespace receiver {
            namespace detail {
                template <std::size_t Index, typename Callback>
                struct indexed_callback {
                    constexpr explicit indexed_callback(Callback&& callback) : callback(
                            std::forward<Callback>(callback)) {}

                    constexpr static const std::size_t index = Index;

                    using callback_type = Callback;

                    template <typename... Ts>
                    constexpr void operator()(Ts&& ... ts) {
                        callback(std::forward<Ts>(ts)...);
                    }

                    Callback callback;
                };
            }

            template <typename TypeMap = boost::mp11::mp_list<>, typename Callbacks = std::tuple<>>
            struct receiver_capability_data {
            private:
                template <typename Matcher>
                using compute_map_entry_if_absent = boost::mp11::mp_eval_if_c<
                        !boost::mp11::mp_map_contains<TypeMap, Matcher>::value,
                        boost::mp11::mp_list<Matcher>,
                        boost::mp11::mp_map_find, TypeMap, Matcher
                >;

                template <typename Matcher, typename Callback>
                using appended_or_inserted_into_map_type = boost::mp11::mp_map_replace<
                        TypeMap,
                        boost::mp11::mp_push_back<
                                compute_map_entry_if_absent<Matcher>,
                                Callback
                        >
                >;

                template <typename Matcher, typename Callback>
                using pushed_back_type = receiver_capability_data<
                        appended_or_inserted_into_map_type<Matcher, Callback>,
                        boost::mp11::mp_push_back<Callbacks, Callback>
                >;

                template <typename Callback>
                using make_indexed_callback = detail::indexed_callback<boost::mp11::mp_size<Callbacks>::value, Callback>;

            public:
                constexpr explicit receiver_capability_data(Callbacks&& callbacks) : callbacks(
                        std::forward<Callbacks>(callbacks)) {}



                template <typename Matcher, typename Callback>
                constexpr inline pushed_back_type<Matcher, make_indexed_callback<Callback>>
                push_back(Matcher&&, Callback&& callback) {
                    return pushed_back_type<Matcher, make_indexed_callback<Callback>>
                            (veho::detail::tuple_push_back(std::move(static_cast<Callbacks>(callbacks)),
                                                           make_indexed_callback<Callback>(std::forward<Callback>(callback))));
                }

                constexpr static const std::size_t num_callbacks = boost::mp11::mp_size<Callbacks>::value;

                using callbacks_type = Callbacks;

                using matcher_to_callback_map_type = TypeMap;

                callbacks_type callbacks;
            };

            constexpr inline receiver_capability_data<> make_receiver_capability_data() {
                return receiver_capability_data<>(std::make_tuple());
            }
        }
    }
}
#endif //VEHO_RECEIVER_FACET_CAPABILITY_DATA_HPP
