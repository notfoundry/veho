/*
* Created by Mark Johnson on 1/20/2018.
*/

#include <veho/bus_template_builder.hpp>
#include <veho/frame_matchers.hpp>

#include "sam3x/rev_8e.hpp"

struct custom_thing_1 {};

struct custom_thing_2 {};

struct custom_thing_3 {};

int main() {
    namespace fm = veho::frame_matchers;

    constexpr auto bus_template = veho::make_bus_template<sam3x::rev_8e>()
            .with_transmitters<7>()
            .record_timestamps()
            .on(fm::exact<20>(), [](const custom_thing_1& t1, const veho::frame<sam3x::rev_8e>& f){
                std::printf("asdas");
            })
            .on(fm::exact<24>(), [](const custom_thing_2& t2, const veho::frame<sam3x::rev_8e>& f){
                std::abort();
            })
            .on(fm::range<0, 4>(), [](const custom_thing_1& t1, const custom_thing_3& t3, const veho::frame<sam3x::rev_8e>& f){
                std::exit(0);
            })
            .on(fm::range<6, 10>(), [](const custom_thing_3& t3, const veho::frame<sam3x::rev_8e>& f){
                std::puts("foo");
            })
            .build();

    auto bus = bus_template.instantiate(custom_thing_1(), custom_thing_2(), custom_thing_3());

    return bus.callback_count;
}
