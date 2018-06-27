/*
* Created by Mark Johnson on 3/14/2018.
*/

#ifndef VEHO_BUS_CONSTRUCTOR_HPP
#define VEHO_BUS_CONSTRUCTOR_HPP

#include <type_traits>

#include "detail/instantiation_utils.hpp"

#include "controller/capabilities.hpp"

#include "config/config_traits.hpp"

#include "bus_template_fwd.hpp"

namespace veho {
    template <typename CompiletimeConfig, typename RuntimeConfig, typename Capability>
    struct bus_construction_requirement {};


    /**
     * @brief Constructs an instance of a CAN bus corresponding with the given controller type
     *
     * The \c bus_constructor class takes the specification for a CAN bus as detailed in the provided CompiletimeConfig,
     * along with additional runtime information in the provided RuntimeConfig, and produces a CAN bus type which
     * implements the provided bus spec.
     *
     * A new CAN controller being supported should partially specialize this class on the type representation of the
     * controller, define a \c bus_type typedef for the produced bus type, and define a static function \c construct
     * as in this example:
     *
     * @code{.cpp}
     *  template <typename CompiletimeConfig, typename RuntimeConfig>
     *  struct bus_constructor <CompiletimeConfig, RuntimeConfig, my_controller_type> {
     *      using bus_type = bus_for_my_controller;
     *
     *      template <typename... Args>
     *      static bus_type construct(CompiletimeConfig&& ct_cfg, RuntimeConfig&& rt_cfg, Args&&... args) {
     *          //initialization logic here
     *      }
     *  };
     * @endcode
     *
     * @tparam CompiletimeConfig The builder config used to store compiletime information
     * @tparam RuntimeConfig The builder config used to store runtime information
     * @tparam Controller The CAN controller type being specialized for by the struct, which must
     * match the controller type of the compiletime and runtime configs
     */
    template <typename CompiletimeConfig, typename RuntimeConfig, typename Controller>
    struct bus_constructor {
        static_assert(veho::detail::false_if_instantiated<CompiletimeConfig>::value, "Bus constructor has not been defined for controller type");

        /**
         * The bus type to construct for the given controller type
         */
        using bus_type = void;

        /**
         * @brief Construct an instance of the bus for the given controller type
         * @tparam Args The types of any additional runtime arguments necessary to construct the bus
         * @param ct_cfg The compiletime config instance used to construct the bus
         * @param rt_cfg The runtime config instance used to construct the bus
         * @param args  Any additional runtime arguments necessary to construct the bus
         * @return A fully-constructed CAN bus instance
         */
        template <typename... Args>
        static bus_type construct(CompiletimeConfig&& ct_cfg, RuntimeConfig&& rt_cfg, Args&&... args) {}
    };
}

#endif //VEHO_BUS_CONSTRUCTOR_HPP
