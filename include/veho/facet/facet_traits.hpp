/*
* Created by Mark Johnson on 3/24/2018.
*/

#ifndef VEHO_FACET_TRAITS_HPP
#define VEHO_FACET_TRAITS_HPP

namespace veho {
    namespace facet {
        namespace detail {
            template <typename Facet>
            struct facet_traits_impl;

            template <template <typename, typename> class BuilderFacetTemplate, typename Config, typename Capability>
            struct facet_traits_impl<BuilderFacetTemplate<Config, Capability>> {
                using supported_capability_type = Capability;
            };
        }

        template <typename Facet>
        struct facet_traits : detail::facet_traits_impl<Facet> {};
    }
}

#endif //VEHO_FACET_TRAITS_HPP
