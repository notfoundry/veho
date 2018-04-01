<h1 align="center">Veho</h1>

<div align="center">
  <strong>A C++11 embedded automotive development platform for the 21st Century</strong>
</div>
<div align="center">
  Hand-written performance, compiler-enforced safety, fluent modern syntax
</div>

<br />

## Table of Contents
- [Example](#example)

## Example
```C++
#include <veho/bus_builder.hpp>
#include <veho/frame_matchers.hpp>

#include "fancy_code.hpp"

int main() {
  namespace fm = veho::frame_matchers;
  
  auto your_fancy_code = make_it_happen();
  
  auto bus = veho::bus_template<sam3x::rev_8e>()
              .with_transmitters<2>()
              .record_timestamps()
              .on(fm::exact<engine_status_id>(), [&](const veho::frame<sam3x::rev_8e>& f){ your_fancy_code.update_engine(f); })
              .on(fm::range<min_flow_id, max_flow_id>(), [&](const veho::frame<sam3x::rev_8e>& f){ your_fancy_code.process_flow(f); })
              .on(fm::mask<base_id, mask_for_spread>(), [&](const veho::frame<sam3x::rev_8e>& f){ your_fancy_code.handle_spread(f); })
              .on_remote(fm::exact<remote_request_id>(), [&](const veho::frame<sam3x::rev_8e>& f) { return your_fancy_code.reply(f); })
              .build();
  
  auto your_timer = cool_timer_event_maker(8_seconds, [&]{
    bus.transmit(your_fancy_code.generate_keep_alive());
  });
}
```
