cmake_minimum_required(VERSION 3.5)
project(veho VERSION 0.1.3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)

option(VEHO_ENABLE_STRONG_INVARIANT_CHECKS, "whether or not to enable additional internal assertions to enforce invariants" OFF)
if (${VEHO_ENABLE_STRONG_INVARIANT_CHECKS})
    set(_veho_enable_strong_invariant_checks 1)
else()
    set(_veho_enable_strong_invariant_checks 0)
endif()

add_library(veho INTERFACE)

set(veho_core_headers
        include/veho/bus_constructor.hpp
        include/veho/bus_constructor_fwd.hpp
        include/veho/bus_template.hpp
        include/veho/bus_template_fwd.hpp
        include/veho/bus_template_builder.hpp
        include/veho/bus_template_builder_fwd.hpp
        include/veho/frame.hpp
        include/veho/frame_matchers.hpp
        include/veho/listener_policies.hpp
 )

set(veho_detail_headers
        include/veho/detail/instantiation_utils.hpp
        include/veho/detail/matcher_utils.hpp
        include/veho/detail/tuple_utils.hpp
)

set(veho_controller_headers
        include/veho/controller/capabilities.hpp
        include/veho/controller/controller_traits.hpp
)

set(veho_receiver_facet_headers
        include/veho/facet/receiver/receiver_facet.hpp
        include/veho/facet/receiver/optimization/optimization.hpp
        include/veho/facet/receiver/optimization/preprocessing/unify_identically_filtered_callbacks_pass.hpp
        include/veho/facet/receiver/optimization/reduction/condense_closest_exact_filters_to_range_pass.hpp
        include/veho/facet/receiver/optimization/pass_traits.hpp
        include/veho/facet/receiver/optimization/normalization/append_range_filter_callback_to_exact_filter_callback_when_exact_within_range_pass.hpp
        include/veho/facet/receiver/optimization/normalization/append_exact_filter_callback_to_range_filter_callback_when_exact_within_range_pass.hpp
        include/veho/facet/receiver/optimization/reduction/condense_closest_range_filters_to_super_range_pass.hpp
        include/veho/facet/receiver/optimization/reduction/extend_range_filter_to_encapsulate_closest_exact_filter_pass.hpp
        include/veho/facet/receiver/optimization/reduction/reduction.hpp
        include/veho/facet/receiver/optimization/normalization/normalization.hpp
        include/veho/facet/receiver/optimization/expansion/expansion.hpp
        include/veho/facet/receiver/receiver_facet_config_postprocessor.hpp
        include/veho/facet/receiver/receiver_facet_compiletime_capability_data.hpp
        include/veho/facet/receiver/receiver_facet_runtime_capability_data.hpp
        include/veho/facet/receiver/optimization/normalization/append_overlapping_range_filter_segment_callbacks_pass.hpp
        include/veho/facet/receiver/optimization/normalization/normalized_callback_traits.hpp
        include/veho/facet/receiver/detail/function_traits.hpp
        include/veho/facet/receiver/optimization/preprocessing/preprocessing.hpp
        include/veho/facet/receiver/receiver_facet_construction_requirement.hpp
)

set(veho_transmitter_facet_headers
        include/veho/facet/transmitter/transmitter_facet.hpp
        include/veho/facet/transmitter/transmitter_facet_config_postprocessor.hpp
        include/veho/facet/transmitter/transmitter_facet_capability_data.hpp
)

set(veho_facet_headers
        ${veho_receiver_facet_headers}
        ${veho_transmitter_facet_headers}
        include/veho/facet/builder_facet.hpp
        include/veho/facet/template_facet.hpp
        include/veho/facet/facet_traits.hpp
)

set(veho_config_headers
        include/veho/config/builder_config.hpp
        include/veho/config/config_traits.hpp
        include/veho/config/detail/replace_data_for_capability_in_config.hpp
        include/veho/config/detail/add_data_for_capability_in_config.hpp
        include/veho/config/config_postprocessing.hpp
        include/veho/config/config_postprocessor_fwd.hpp
)

set(header_files
        ${veho_core_headers}
        ${veho_config_headers}
        ${veho_controller_headers}
        ${veho_detail_headers}
        ${veho_facet_headers}
)

target_sources(veho INTERFACE $<BUILD_INTERFACE:${header_files}>)
target_include_directories(veho INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
add_subdirectory(third_party EXCLUDE_FROM_ALL)

target_compile_features(veho INTERFACE cxx_auto_type)

target_compile_definitions(veho INTERFACE
        VEHO_ENABLE_STRONG_INVARIANT_CHECKS=${_veho_enable_strong_invariant_checks})

option(VEHO_BUILD_TESTS "build tests" OFF)
if (${VEHO_BUILD_TESTS})
    add_subdirectory(test)
endif()