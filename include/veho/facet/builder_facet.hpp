/*
* Created by Mark Johnson on 3/24/2018.
*/

#ifndef VEHO_FACET_DEF_FWD_HPP
#define VEHO_FACET_DEF_FWD_HPP

#include <veho/detail/instantiation_utils.hpp>

namespace veho {
    namespace facet {
        template <typename Config, typename Capability>
        struct builder_facet {
            static_assert(veho::detail::false_if_instantiated<Config>::value, "Builder facet has not been defined for controller capability");
        };
    }
}

#endif //VEHO_FACET_DEF_FWD_HPP
