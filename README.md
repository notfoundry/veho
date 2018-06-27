<h1 align="center">Veho</h1>

<div align="center">
  <strong>A C++11 embedded CAN development platform for the 21st Century </strong>
</div>
<div align="center">
  Hand-written performance, compiler-enforced safety, fluent modern syntax
</div>
<br />

<div align="center">
  
  [Check out the wiki for design notes!](https://github.com/foundry27/veho/wiki/Design-Details)
</div>

## Table of Contents
- [What is it?](#what-is-it?)
- [Example](#example)

## What is it?
Veho is a library for transforming specifications for Controller Area Network (CAN) nodes into implementations of those specifications that are optimized at compile-time to account for the capabilities and limitations of the target CAN platform. What this means is that you as a developer can focus on specifying _what_ you want your CAN node to do, and worry less about _how_ those goals are accomplished.

## Example
```C++

int main() {
  namespace fm = veho::frame_matchers;
  namespace lp = veho::listener_policies;

  constexpr auto bus_template = veho::make_bus_template<sam3x::rev_8e>()
          .with_transmitters<2>()
          .record_timestamps()
          .on(fm::exact<engine_status_id>(),
              [](const fancy_code& fc, const veho::frame<sam3x::rev_8e>& f){ 
                  fc.update_engine(f);
              },
              lp::execution_policy<lp::launch::realtime>()
          )
          .on(fm::range<min_flow_id, max_flow_id>(),
              [](const even_fancier& ef, const veho::frame<sam3x::rev_8e>& f){
                  ef.process_flow(f);
              },
              lp::retention_policy<lp::retention::most_recent<6>>,
              lp::execution_policy<lp::launch::enqueue>()
          )
          .on(fm::mask<base_id, mask_for_data_spread>(),
              [](const fancy_code& fc, const even_fancier& ef, const veho::frame<sam3x::rev_8e>& f){
                  fc.handle_spread(ef.preprocess_spread(f));
              },
              lp::retention_policy<lp::retention::discard_after<10>>(),
              lp::execution_policy<lp::launch::enqueue>()
          )
          .build();
          
  auto bus = bus_template
      .with(get_fancy(), get_fancier())
      .instantiate(get_default_pin_number());
}
```
