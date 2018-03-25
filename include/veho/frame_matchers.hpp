/*
* Created by Mark Johnson on 3/3/2018.
*/

#ifndef VEHO_FRAME_MATCHERS_HPP
#define VEHO_FRAME_MATCHERS_HPP

#include "frame.hpp"

namespace veho {
    namespace frame_matchers {

        template <common_frame::id_type ID>
        struct exact {
            constexpr const static common_frame::id_type id = ID;
        };

        template <common_frame::id_type ID, common_frame::id_type Mask>
        struct masked {
            constexpr const static common_frame::id_type id = ID;

            constexpr const static common_frame::id_type mask = Mask;
        };

        template <common_frame::id_type Min, common_frame::id_type Max>
        struct range {
            constexpr const static common_frame::id_type min = Min;

            constexpr const static common_frame::id_type max = Max;
        };
    }
}

#endif //VEHO_FRAME_MATCHERS_HPP
