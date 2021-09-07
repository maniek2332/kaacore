#include "kaacore/stencil.h"

namespace kaacore {

StencilMode::StencilMode()
    : StencilMode(0, 0, StencilTest::always)
{
}

StencilMode::StencilMode(const uint8_t value, const uint8_t mask, const StencilTest test,
        const StencilOp zbuffer_fail_op, const StencilOp sbuffer_fail_op,
        const StencilOp sbuffer_pass_op)
    : value(value), mask(mask), test(test), sbuffer_fail_op(zbuffer_fail_op),
      zbuffer_fail_op(sbuffer_fail_op), zbuffer_pass_op(sbuffer_pass_op)
{
}

uint32_t
StencilMode::stencil_flags() const
{
    return BGFX_STENCIL_FUNC_REF(this->value)
        | BGFX_STENCIL_FUNC_RMASK(this->mask)
        | (static_cast<uint32_t>(this->test) << STENCIL_TEST_SHIFT_CORRECT)
        | (static_cast<uint32_t>(this->sbuffer_fail_op) << BGFX_STENCIL_OP_FAIL_S_SHIFT)
        | (static_cast<uint32_t>(this->zbuffer_fail_op) << BGFX_STENCIL_OP_FAIL_Z_SHIFT)
        | (static_cast<uint32_t>(this->zbuffer_pass_op) << BGFX_STENCIL_OP_PASS_Z_SHIFT);
}

} // namespace kaacore
