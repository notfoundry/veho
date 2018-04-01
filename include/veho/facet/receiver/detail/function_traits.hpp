/*
* Created by Mark Johnson on 3/31/2018.
*/

#ifndef VEHO_FUNCTION_TRAITS_HPP
#define VEHO_FUNCTION_TRAITS_HPP

#include <boost/mp11/list.hpp>

namespace veho {
    namespace facet {
        namespace receiver {
            namespace detail {
                template <typename F>
                struct function_traits;

                template <typename R, typename... Args>
                struct function_traits<R(*)(Args...)> : function_traits<R(Args...)> {};

                template <typename R, typename... Args>
                struct function_traits<R(Args...)> {
                    using return_type = R;

                    using parameter_type_list = boost::mp11::mp_list<Args...>;

                    constexpr static const std::size_t arity = sizeof...(Args);
                };

                template <typename C, typename R, typename... Args>
                struct function_traits<R(C::*)(Args...)> : function_traits<R(C&,Args...)> {};

                template <typename C, typename R, typename... Args>
                struct function_traits<R(C::*)(Args...) const> : function_traits<R(C&,Args...)> {};

                template <typename C, typename R>
                struct function_traits<R(C::*)> : function_traits<R(C&)> {};

                template <typename F>
                struct function_traits {
                private:
                    using underlying_function_traits = function_traits<decltype(&F::operator())>;

                public:
                    using return_type = typename underlying_function_traits::return_type;

                    constexpr static const std::size_t arity = underlying_function_traits::arity - 1;

                    using parameter_type_list = boost::mp11::mp_pop_front<typename underlying_function_traits::parameter_type_list>;
                };

                template <typename F>
                struct function_traits<F&> : function_traits<F> {};

                template <typename F>
                struct function_traits<F&&> : function_traits<F> {};
            }
        }
    }
}

#endif //VEHO_FUNCTION_TRAITS_HPP
