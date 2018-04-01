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

#include "sam3x.hpp"
#include "fancy_code.hpp"

int main() {
  namespace fm = veho::frame_matchers;

  constexpr auto bus_template = veho::make_bus_template<sam3x::rev_8e>()
          .with_transmitters<2>()
          .record_timestamps()
          .on(fm::exact<engine_status_id>(),
                  [](const fancy_code& fc, const veho::frame<sam3x::rev_8e>& f){ 
              fc.update_engine(f);
          })
          .on(fm::range<min_flow_id, max_flow_id>(),
                  [](const even_fancier& ef, const veho::frame<sam3x::rev_8e>& f){
              ef.process_flow(f);
          })
          .on(fm::mask<base_id, mask_for_data_spread>(),
                  [](const fancy_code& fc, const even_fancier& ef, const veho::frame<sam3x::rev_8e>& f){
              fc.handle_spread(ef.preprocess_spread(f));
          })
          .build();
          
  auto bus = bus_template.instantiate(get_fanciness(), get_even_fancier_fanciness());
  
  bus.set_timing(sam3x::timing_builder()
      .clock_freq_hz(48000000)
      .baudrate(500000)
      .tx_rx_delay_ns(200)
      .sample_point(0.8f));
      
  bus.activate(get_pin_number());
  
  auto your_timer = cool_timer_event_maker(8_seconds, [&]{
    bus.transmit(your_fancy_code.generate_keep_alive());
  });
}
```
