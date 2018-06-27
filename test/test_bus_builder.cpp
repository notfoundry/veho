/*
* Created by Mark Johnson on 1/20/2018.
*/

#include <cstdio>

#include <veho/bus_template_builder.hpp>
#include <veho/frame_matchers.hpp>
#include <veho/listener_policies.hpp>

#include "sam3x/rev_8e.hpp"

struct custom_thing_1 {};

struct custom_thing_2 {};

struct custom_thing_3 {};

int main() {
    namespace fm = veho::frame_matchers;
    namespace lp = veho::listener_policies;

    constexpr auto bus_template = veho::make_bus_template<sam3x::rev_8e>()
            .with_transmitters<7>()
            .record_timestamps()
            .on(fm::exact<20>(),
                [](const custom_thing_1& t1, const veho::frame<sam3x::rev_8e>& f) {
                    std::printf("asdas");
                },
                lp::retention_policy<lp::retention::most_recent<5>>(),
                lp::execution_policy<lp::launch::enqueue>()
            )
            .on(fm::exact<24>(),
                [](const custom_thing_2& t2, const veho::frame<sam3x::rev_8e>& f) {
                    std::abort();
                },
                lp::execution_policy<lp::launch::realtime>()
            )
            .on(fm::range<0, 4>(),
                [](const custom_thing_1& t1, const custom_thing_3& t3, const veho::frame<sam3x::rev_8e>& f) {
                    std::exit(0);
                },
                lp::retention_policy<lp::retention::discard_after<10>>(),
                lp::execution_policy<lp::launch::enqueue>()
            )
            .on(fm::range<6, 10>(),
                [](const custom_thing_3& t3, const veho::frame<sam3x::rev_8e>& f) {
                    std::puts("foo");
                }
            )
            .build();

//    static_assert(bus_template_traits<decltype(bus_template)>::needs_state_advancing, "");

    auto bus = bus_template
            .with(custom_thing_1(), custom_thing_2(), custom_thing_3())
            .instantiate();

    std::printf("%u", bus.transmitter_count);
}
