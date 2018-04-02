/*
* Created by Mark Johnson on 3/24/2018.
*/

#ifndef VEHO_FACET_TRAITS_HPP
#define VEHO_FACET_TRAITS_HPP

#include "builder_facet.hpp"
#include "template_facet.hpp"

namespace veho {
    namespace facet {
        namespace detail {
            template <typename Facet>
            struct facet_traits_impl;

            template <typename Config, typename Capability>
            struct facet_traits_impl<builder_facet<Config, Capability>> {
                using supported_capability_type = Capability;
            };

            template <typename CompiletimeConfig, typename RuntimeConfig, typename Capability>
            struct facet_traits_impl<template_facet<CompiletimeConfig, RuntimeConfig, Capability>> {
                using supported_capability_type = Capability;
            };
        }

        template <typename Facet>
        struct facet_traits : detail::facet_traits_impl<Facet> {};
    }
}

#endif //VEHO_FACET_TRAITS_HPP
