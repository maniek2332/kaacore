#pragma once

#include <cstdint>

#include <bgfx/defines.h>

#include "kaacore/utils.h"

// XXX value of BGFX_STENCIL_TEST_SHIFT is incorrect
#define STENCIL_TEST_SHIFT_CORRECT 12

namespace kaacore {

enum class StencilTest : uint8_t {
    less = BGFX_STENCIL_TEST_LESS >> STENCIL_TEST_SHIFT_CORRECT,
    less_equal = BGFX_STENCIL_TEST_LEQUAL >> STENCIL_TEST_SHIFT_CORRECT,
    equal = BGFX_STENCIL_TEST_EQUAL >> STENCIL_TEST_SHIFT_CORRECT,
    greater_equal = BGFX_STENCIL_TEST_GEQUAL >> STENCIL_TEST_SHIFT_CORRECT,
    greater = BGFX_STENCIL_TEST_GREATER >> STENCIL_TEST_SHIFT_CORRECT,
    not_equal = BGFX_STENCIL_TEST_NOTEQUAL >> STENCIL_TEST_SHIFT_CORRECT,
    never = BGFX_STENCIL_TEST_NEVER >> STENCIL_TEST_SHIFT_CORRECT,
    always = BGFX_STENCIL_TEST_ALWAYS >> STENCIL_TEST_SHIFT_CORRECT,
};

enum class StencilOp : uint8_t {
    zero = BGFX_STENCIL_OP_FAIL_S_ZERO >> BGFX_STENCIL_OP_FAIL_S_SHIFT,
    keep = BGFX_STENCIL_OP_FAIL_S_KEEP >> BGFX_STENCIL_OP_FAIL_S_SHIFT,
    replace = BGFX_STENCIL_OP_FAIL_S_REPLACE >> BGFX_STENCIL_OP_FAIL_S_SHIFT,
    increase_wrap = BGFX_STENCIL_OP_FAIL_S_INCR >> BGFX_STENCIL_OP_FAIL_S_SHIFT,
    increase_clamp = BGFX_STENCIL_OP_FAIL_S_INCRSAT >> BGFX_STENCIL_OP_FAIL_S_SHIFT,
    decrease_wrap = BGFX_STENCIL_OP_FAIL_S_DECR >> BGFX_STENCIL_OP_FAIL_S_SHIFT,
    decrease_clamp = BGFX_STENCIL_OP_FAIL_S_DECRSAT >> BGFX_STENCIL_OP_FAIL_S_SHIFT,
    invert = BGFX_STENCIL_OP_FAIL_S_INVERT >> BGFX_STENCIL_OP_FAIL_S_SHIFT,
};


struct StencilMode {
    StencilMode();
    StencilMode(const uint8_t value, const uint8_t mask, const StencilTest test,
        const StencilOp zbuffer_fail_op = StencilOp::keep,
        const StencilOp sbuffer_fail_op = StencilOp::keep,
        const StencilOp sbuffer_pass_op = StencilOp::keep);
    uint32_t stencil_flags() const;
    bool operator==(const StencilMode& other) const;

    uint8_t value;
    uint8_t mask;
    StencilTest test;
    StencilOp sbuffer_fail_op;
    StencilOp zbuffer_fail_op;
    StencilOp zbuffer_pass_op;
};

} // namespace kaacore

namespace std {
using kaacore::hash_combined;

template<>
struct hash<kaacore::StencilMode> {
    size_t operator()(const kaacore::StencilMode& stencil_mode) const
    {
        return hash_combined(
            stencil_mode.value,
            stencil_mode.mask,
            static_cast<uint8_t>(stencil_mode.sbuffer_fail_op),
            static_cast<uint8_t>(stencil_mode.zbuffer_fail_op),
            static_cast<uint8_t>(stencil_mode.zbuffer_pass_op)
        );
    }
};
}
