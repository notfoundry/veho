/*
* Created by Mark Johnson on 3/24/2018.
*/

#ifndef VEHO_TRANSMITTER_FACET_CAPABILITY_DATA_HPP
#define VEHO_TRANSMITTER_FACET_CAPABILITY_DATA_HPP

#include <cstddef>

namespace veho {
    namespace facet {
        namespace transmitter {
            template <std::size_t NumTransmitters>
            struct transmitter_capability_data {
                static_assert(NumTransmitters > 0,
                              "At least one transmitter must be added to specify transmitter count");

                constexpr static const std::size_t num_transmitters = NumTransmitters;
            };
        }
    }
}

#endif //VEHO_TRANSMITTER_FACET_CAPABILITY_DATA_HPP
