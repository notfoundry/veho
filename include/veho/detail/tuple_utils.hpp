/*
* Created by Mark Johnson on 1/4/2018.
*/

#ifndef VEHO_TUPLE_UTILS_HPP
#define VEHO_TUPLE_UTILS_HPP

#include <tuple>

#include "instantiation_utils.hpp"

namespace veho {
    namespace detail {

        template <typename Tuple, typename T>
        constexpr inline decltype(std::tuple_cat(std::declval<Tuple>(), std::make_tuple(std::declval<T>())))
        tuple_push_back(Tuple&& tuple, T&& t) {
            return std::tuple_cat(std::forward<Tuple>(tuple), std::make_tuple(std::forward<T>(t)));
        };

        namespace detail {
            struct select_next_action {};
            template <std::size_t Index, class Search, class First, class... Types>
            struct get_type_index {
                static_assert(sizeof...(Types) > 0, "Type is not present in tuple");
                typedef typename get_type_index<Index + 1, Search, Types...>::type type;
                static constexpr const std::size_t index = Index;
            };

            template <std::size_t Index, class Search, class... Types>
            struct get_type_index<Index, Search, Search, Types...> {
                typedef get_type_index type;
                static constexpr const std::size_t index = Index;
            };
        }

        template <class T, class... Types>
        constexpr inline T get_tuple_elem_by_type(const std::tuple<Types...>& tuple) {
            return std::get<detail::get_type_index<0, T, Types...>::type::index>(tuple);
        }
    }
}
#endif //VEHO_TUPLE_UTILS_HPP
