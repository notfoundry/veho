/*
* Created by Mark Johnson on 1/20/2018.
*/

#ifndef VEHO_MATCHER_UTILS_HPP
#define VEHO_MATCHER_UTILS_HPP

#include <boost/mp11/integral.hpp>

#include <veho/frame.hpp>
#include <veho/frame_matchers.hpp>

namespace veho {
    namespace detail {
        template <typename T>
        struct is_range : boost::mp11::mp_false {
        };

        template <common_frame::id_type Min, common_frame::id_type Max>
        struct is_range<veho::frame_matchers::range<Min, Max>> : boost::mp11::mp_true {
        };

        template <typename T>
        struct is_mask : boost::mp11::mp_false {
        };

        template <common_frame::id_type ID, common_frame::id_type Mask>
        struct is_mask<veho::frame_matchers::masked<ID, Mask>> : boost::mp11::mp_true {
        };

        template <typename T>
        struct is_exact : boost::mp11::mp_false {
        };

        template <common_frame::id_type ID>
        struct is_exact<veho::frame_matchers::exact<ID>> : boost::mp11::mp_true {
        };
    }
}

#endif //VEHO_MATCHER_UTILS_HPP
