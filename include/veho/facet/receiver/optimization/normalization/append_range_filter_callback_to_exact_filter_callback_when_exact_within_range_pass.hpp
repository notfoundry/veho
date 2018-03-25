/*
* Created by Mark Johnson on 1/7/2018.
*/

#ifndef VEHO_APPEND_RANGE_FILTER_CALLBACK_TO_EXACT_FILTER_CALLBACK_WHEN_EXACT_WITHIN_RANGE_PASS_HPP
#define VEHO_APPEND_RANGE_FILTER_CALLBACK_TO_EXACT_FILTER_CALLBACK_WHEN_EXACT_WITHIN_RANGE_PASS_HPP

#include <utility>

#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/map.hpp>
#include <boost/mp11/integral.hpp>

#include <veho/facet/receiver/optimization/pass_traits.hpp>

#include <veho/detail/tuple_utils.hpp>
#include <veho/detail/matcher_utils.hpp>

namespace veho {
    namespace facet {
        namespace receiver {
            namespace optimization {
                namespace normalization {
                    template <typename Controller, typename TypeMap, typename Callbacks>
                    class append_range_filter_callback_to_exact_filter_callback_when_exact_within_range_pass {
                    private:
                        template <typename Range, typename Exact>
                        using exact_within_range = boost::mp11::mp_bool<
                                Exact::id >= Range::min && Exact::id <= Range::max
                        >;

                        using type_list = boost::mp11::mp_map_keys<TypeMap>;

                        using partitioned = boost::mp11::mp_partition<type_list, veho::detail::is_exact>;


                    public:
                        constexpr explicit append_range_filter_callback_to_exact_filter_callback_when_exact_within_range_pass(
                                Callbacks&& callbacks)
                                : new_callbacks(std::forward<Callbacks>(callbacks)) {}

                        using pass_category = veho::facet::receiver::optimization::normalization_pass_tag;

                        using updated_type_map_type = TypeMap;

                        using updated_callbacks_type = Callbacks;

                        updated_callbacks_type new_callbacks;
                    };
                }
            }
        }
    }
}
#endif //VEHO_APPEND_RANGE_FILTER_CALLBACK_TO_EXACT_FILTER_CALLBACK_WHEN_EXACT_WITHIN_RANGE_PASS_HPP
