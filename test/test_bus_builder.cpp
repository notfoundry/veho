/*
* Created by Mark Johnson on 1/20/2018.
*/

#include <veho/bus_builder.hpp>
#include <veho/frame_matchers.hpp>

#include "sam3x/rev_8e.hpp"

int main() {
    namespace fm = veho::frame_matchers;

    constexpr auto bus_template = veho::bus_template<sam3x::rev_8e>()
            .with_transmitters<7>()
            .record_timestamps()
            .on(fm::exact<20>(), [](const veho::frame<sam3x::rev_8e>& f){ std::printf("%u", f.family_id); })
            .on(fm::exact<24>(), [](const veho::frame<sam3x::rev_8e>& f){ std::abort(); })
            .on(fm::range<0, 4>(), [](const veho::frame<sam3x::rev_8e>& f){ std::exit(0); })
            .on(fm::range<6, 10>(), [](const veho::frame<sam3x::rev_8e>& f){ std::puts("foo"); })
//            .on_remote(fm::exact<7>(), [](const veho::frame<sam3x::rev_8e>& f) { return veho::frame<sam3x::rev_8e>(); })
            ;

    auto bus = bus_template.build();
}
