/*
* Created by Mark Johnson on 1/4/2018.
*/

#ifndef VEHO_TUPLE_UTILS_HPP
#define VEHO_TUPLE_UTILS_HPP

#include <tuple>

namespace veho {
    namespace detail {

        template <typename Tuple, typename T>
        constexpr inline decltype(std::tuple_cat(std::declval<Tuple>(), std::make_tuple(std::declval<T>())))
        tuple_push_back(Tuple&& tuple, T&& t) {
            return std::tuple_cat(std::forward<Tuple>(tuple), std::make_tuple(std::forward<T>(t)));
        };

        namespace detail {
            template <int Index, class Search, class First, class... Types>
            struct get_internal {
                typedef typename get_internal<Index + 1, Search, Types...>::type type;
                static constexpr const int index = Index;
            };

            template <int Index, class Search, class... Types>
            struct get_internal<Index, Search, Search, Types...> {
                typedef get_internal type;
                static constexpr const int index = Index;
            };
        }

        template <class T, class... Types>
        constexpr inline T get_tuple_elem_by_type(const std::tuple<Types...>& tuple) {
            return std::get<detail::get_internal<0, T, Types...>::type::index>(tuple);
        }
    }
}
#endif //VEHO_TUPLE_UTILS_HPP
