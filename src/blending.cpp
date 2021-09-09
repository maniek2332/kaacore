#include "kaacore/blending.h"

namespace kaacore {

BlendingMode::BlendingMode()
    : BlendingMode(BlendingFactor::src_alpha, BlendingFactor::inverted_src_alpha)
{}

BlendingMode::BlendingMode(
    BlendingFactor src_factor, BlendingFactor dst_factor,
    BlendingEquation equation)
    : BlendingMode(
          src_factor, dst_factor, src_factor, dst_factor, equation, equation)
{}

BlendingMode::BlendingMode(
    BlendingFactor src_factor, BlendingFactor dst_factor,
    BlendingFactor src_alpha_factor, BlendingFactor dst_alpha_factor,
    BlendingEquation equation, BlendingEquation alpha_equation)
    : src_factor(src_factor), dst_factor(dst_factor),
      src_alpha_factor(src_alpha_factor), dst_alpha_factor(dst_alpha_factor),
      equation(equation), alpha_equation(alpha_equation)
{}

uint64_t
BlendingMode::state_flags() const
{
    return BGFX_STATE_BLEND_FUNC_SEPARATE(
               static_cast<uint64_t>(this->src_factor)
                   << BGFX_STATE_BLEND_SHIFT,
               static_cast<uint64_t>(this->dst_factor)
                   << BGFX_STATE_BLEND_SHIFT,
               static_cast<uint64_t>(this->src_alpha_factor)
                   << BGFX_STATE_BLEND_SHIFT,
               static_cast<uint64_t>(this->dst_alpha_factor)
                   << BGFX_STATE_BLEND_SHIFT) |
           BGFX_STATE_BLEND_EQUATION_SEPARATE(
               static_cast<uint64_t>(this->equation)
                   << BGFX_STATE_BLEND_EQUATION_SHIFT,
               static_cast<uint64_t>(this->alpha_equation)
                   << BGFX_STATE_BLEND_EQUATION_SHIFT);
}

bool
BlendingMode::operator==(const BlendingMode& other)
{
    return this->src_factor == other.src_factor and
           this->dst_factor == other.dst_factor and
           this->src_alpha_factor == other.src_alpha_factor and
           this->dst_alpha_factor == other.dst_alpha_factor and
           this->equation == other.equation and
           this->alpha_equation == other.alpha_equation;
}

} // namespace kaacore
