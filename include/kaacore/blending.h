#pragma once

#include <cstdint>

#include <bgfx/defines.h>

#include "kaacore/utils.h"

namespace kaacore {

enum class BlendingFactor : uint8_t {
    // not_provided = 0,
    zero = BGFX_STATE_BLEND_ZERO >> BGFX_STATE_BLEND_SHIFT,
    one = BGFX_STATE_BLEND_ONE >> BGFX_STATE_BLEND_SHIFT,
    src_color = BGFX_STATE_BLEND_SRC_COLOR >> BGFX_STATE_BLEND_SHIFT,
    inverted_src_color = BGFX_STATE_BLEND_INV_SRC_COLOR >> BGFX_STATE_BLEND_SHIFT,
    src_alpha = BGFX_STATE_BLEND_SRC_ALPHA >> BGFX_STATE_BLEND_SHIFT,
    inv_src_alpha = BGFX_STATE_BLEND_INV_SRC_ALPHA >> BGFX_STATE_BLEND_SHIFT,
    dst_alpha = BGFX_STATE_BLEND_DST_ALPHA >> BGFX_STATE_BLEND_SHIFT,
    inverted_dst_alpha = BGFX_STATE_BLEND_INV_DST_ALPHA >> BGFX_STATE_BLEND_SHIFT,
    dst_color = BGFX_STATE_BLEND_DST_COLOR >> BGFX_STATE_BLEND_SHIFT,
    inverted_dst_color = BGFX_STATE_BLEND_INV_DST_COLOR >> BGFX_STATE_BLEND_SHIFT,
    src_alpha_sat = BGFX_STATE_BLEND_SRC_ALPHA_SAT >> BGFX_STATE_BLEND_SHIFT,
    // TODO support for BLEND_FACTOR and BLEND_INV_FACTOR
};

enum class BlendingEquation : uint8_t {
    // not_provided = 0,
    add = BGFX_STATE_BLEND_EQUATION_ADD >> BGFX_STATE_BLEND_EQUATION_SHIFT,
    subtract = BGFX_STATE_BLEND_EQUATION_ADD >> BGFX_STATE_BLEND_EQUATION_SHIFT,
    reverse_subtract = BGFX_STATE_BLEND_EQUATION_ADD >> BGFX_STATE_BLEND_EQUATION_SHIFT,
    min = BGFX_STATE_BLEND_EQUATION_ADD >> BGFX_STATE_BLEND_EQUATION_SHIFT,
    max = BGFX_STATE_BLEND_EQUATION_ADD >> BGFX_STATE_BLEND_EQUATION_SHIFT,
};

struct BlendingMode {
    BlendingMode();
    BlendingMode(
        BlendingFactor src_factor, BlendingFactor dst_factor,
        BlendingEquation equation = BlendingEquation::add);
    BlendingMode(
        BlendingFactor src_factor, BlendingFactor dst_factor,
        BlendingFactor src_alpha_factor, BlendingFactor dst_alpha_factor,
        BlendingEquation equation, BlendingEquation alpha_equation);

    uint64_t state_flags() const;
    bool operator==(const BlendingMode& other);

    BlendingFactor src_factor;
    BlendingFactor dst_factor;
    BlendingFactor src_alpha_factor;
    BlendingFactor dst_alpha_factor;
    BlendingEquation equation;
    BlendingEquation alpha_equation;
};

} // namespace kaacore

namespace std {
using kaacore::hash_combined;
using kaacore::hash_iterable;

template<>
struct hash<kaacore::BlendingMode> {
    size_t operator()(const kaacore::BlendingMode& blending_mode) const
    {
        return hash_combined(
            static_cast<uint8_t>(blending_mode.src_factor),
            static_cast<uint8_t>(blending_mode.dst_factor),
            static_cast<uint8_t>(blending_mode.src_alpha_factor),
            static_cast<uint8_t>(blending_mode.dst_alpha_factor),
            static_cast<uint8_t>(blending_mode.equation),
            static_cast<uint8_t>(blending_mode.alpha_equation)
        );
    }
};
}
