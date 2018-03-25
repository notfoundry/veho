/*
* Created by Mark Johnson on 3/3/2018.
*/

#ifndef VEHO_FRAME_HPP
#define VEHO_FRAME_HPP

#include <cinttypes>

namespace veho {
    struct common_frame {
        using payload_type = uint8_t[8];
        using id_type = uint32_t;
        using data_length_type = uint8_t;

        using id_diff_type = int64_t;

        payload_type data;
        id_type id;
        data_length_type data_length;
        bool is_remote_transmission_request;
        bool is_extended;
    };

    template <typename Controller>
    struct frame_extensions {};

    template <typename Controller>
    struct frame : common_frame, frame_extensions<Controller> {};
}

#endif //VEHO_FRAME_HPP
