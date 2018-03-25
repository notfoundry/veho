/*
* Created by Mark Johnson on 3/17/2018.
*/

#ifndef VEHO_REPLACE_DATA_FOR_CAPABILITY_IN_CONFIG_HPP
#define VEHO_REPLACE_DATA_FOR_CAPABILITY_IN_CONFIG_HPP

#include <type_traits>
#include <utility>
#include <tuple>

#include <boost/mp11/list.hpp>
#include <boost/mp11/utility.hpp>

#include <veho/detail/tuple_utils.hpp>

namespace veho {
    namespace config {
        namespace detail {
            namespace mp = boost::mp11;

            template <typename OldCapabilityDataSet, typename OldCapabilityData,
                    typename CapabilityDataSetAccumulator, typename NewCapabilityData,
                    bool ShouldInsertNew = true
            >
            struct update_capability_data_set {
                constexpr update_capability_data_set(const OldCapabilityDataSet&,
                                                     CapabilityDataSetAccumulator&& new_capability_data_set_accumulator,
                                                     NewCapabilityData&& new_capability_data)
                        : updated_tuple(veho::detail::tuple_push_back(
                                std::forward<CapabilityDataSetAccumulator>(new_capability_data_set_accumulator),
                                        std::forward<NewCapabilityData>(new_capability_data))) {}
                        
                using updated_tuple_type = mp::mp_push_back<CapabilityDataSetAccumulator, NewCapabilityData>;

                updated_tuple_type updated_tuple;
            };

            template <typename OldCapabilityDataSet, typename OldCapabilityData,
                    typename CapabilityDataSetAccumulator, typename NewCapabilityData
            >
            struct update_capability_data_set<OldCapabilityDataSet, OldCapabilityData, CapabilityDataSetAccumulator, NewCapabilityData, false> {
                constexpr update_capability_data_set(const OldCapabilityDataSet& old_capability_data_set,
                                                     CapabilityDataSetAccumulator&& new_capability_data_set_accumulator,
                                                     NewCapabilityData&&)
                        : updated_tuple(veho::detail::tuple_push_back(
                        std::forward<CapabilityDataSetAccumulator>(new_capability_data_set_accumulator),
                        veho::detail::get_tuple_elem_by_type<OldCapabilityData>(old_capability_data_set))) {}

                using updated_tuple_type = mp::mp_push_back<CapabilityDataSetAccumulator, OldCapabilityData>;

                updated_tuple_type updated_tuple;
            };
            
            template <typename OldCapabilityMap, typename OldCapabilityDataSet,
                    typename Capability, typename NewCapabilityData,
                    typename CapabilityMapAccumulator = mp::mp_list<>, typename CapabilityDataSetAccumulator = std::tuple<>,
                    std::size_t RemainingElements = mp::mp_size<OldCapabilityMap>::value
            >
            struct replace_data_for_capability_impl {
                using current_map_entry = mp::mp_front<OldCapabilityMap>;

                using current_map_entry_key = mp::mp_first<current_map_entry>;

                using current_map_entry_value = mp::mp_second<current_map_entry>;

                constexpr static const bool should_replace_current_map_entry = std::is_same<current_map_entry_key, Capability>::value;

                using next_map_accumulator_step = mp::mp_push_back<
                        CapabilityMapAccumulator,
                        mp::mp_if_c<
                                should_replace_current_map_entry,
                                std::pair<Capability, NewCapabilityData>,
                                current_map_entry
                        >
                >;

                using next_data_set_accumulator_step = mp::mp_push_back<
                        CapabilityDataSetAccumulator,
                        mp::mp_if_c<
                                should_replace_current_map_entry,
                                NewCapabilityData,
                                current_map_entry_value
                        >
                >;

                using next_replacement_step = replace_data_for_capability_impl<
                        mp::mp_pop_front<OldCapabilityMap>, OldCapabilityDataSet,
                        Capability, NewCapabilityData,
                        next_map_accumulator_step, next_data_set_accumulator_step
                >;

                constexpr replace_data_for_capability_impl(OldCapabilityDataSet&& old_capability_data_set,
                                                                    CapabilityDataSetAccumulator&& new_capability_data_set_accumulator,
                                                                    NewCapabilityData&& new_capability_data)
                : new_capability_data_set_accumulator(next_replacement_step(
                        std::forward<OldCapabilityDataSet>(old_capability_data_set),
                        update_capability_data_set<
                                OldCapabilityDataSet, current_map_entry_value,
                                CapabilityDataSetAccumulator, NewCapabilityData,
                                should_replace_current_map_entry
                        >(old_capability_data_set, std::forward<CapabilityDataSetAccumulator>(new_capability_data_set_accumulator), std::forward<NewCapabilityData>(new_capability_data)).updated_tuple,
                        std::forward<NewCapabilityData>(new_capability_data)
                ).new_capability_data_set_accumulator) {}

                using new_capability_map = typename next_replacement_step::new_capability_map;

                using new_capability_data_set_type = typename next_replacement_step::new_capability_data_set_type;

                new_capability_data_set_type new_capability_data_set_accumulator;
            };

            template <typename OldCapabilityMap, typename OldCapabilityDataSet, typename Capability, typename NewCapabilityData,
                    typename CapabilityMapAccumulator, typename CapabilityDataSetAccumulator>
            struct replace_data_for_capability_impl<OldCapabilityMap, OldCapabilityDataSet, Capability, NewCapabilityData,
                    CapabilityMapAccumulator, CapabilityDataSetAccumulator, 0> {
                constexpr replace_data_for_capability_impl(OldCapabilityDataSet&&,
                                                                    CapabilityDataSetAccumulator&& new_capability_data_set_accumulator,
                                                                    NewCapabilityData&&)
                        : new_capability_data_set_accumulator(std::forward<CapabilityDataSetAccumulator>(new_capability_data_set_accumulator)) {}

                using new_capability_map = CapabilityMapAccumulator;

                using new_capability_data_set_type = CapabilityDataSetAccumulator;

                new_capability_data_set_type new_capability_data_set_accumulator;
            };


            template <typename CapabilityMap, typename CapabilityDataSet, typename Capability, typename NewCapabilityData>
            class replace_data_for_capability {
                using impl = replace_data_for_capability_impl<CapabilityMap, CapabilityDataSet, Capability, NewCapabilityData>;

            public:
                constexpr replace_data_for_capability(CapabilityDataSet&& old_capability_data_set, NewCapabilityData&& new_capability_data)
                : new_capability_data_set(impl(
                        std::forward<CapabilityDataSet>(old_capability_data_set), std::make_tuple(), std::forward<NewCapabilityData>(new_capability_data)
                ).new_capability_data_set_accumulator) {}

                using new_capability_map = typename impl::new_capability_map;

                using new_capability_data_set_type = typename impl::new_capability_data_set_type;

                new_capability_data_set_type new_capability_data_set;
            };
        }
    }
}

#endif //VEHO_REPLACE_DATA_FOR_CAPABILITY_IN_CONFIG_HPP
