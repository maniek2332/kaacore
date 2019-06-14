#pragma once

#include <vector>
#include <memory>

#include <bgfx/bgfx.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "kaacore/files.h"
#include "kaacore/texture_loader.h"
#include "kaacore/log.h"


namespace kaacore {

typedef uint16_t VertexIndex;

struct StandardVertexData {
    glm::fvec3 xyz;
    glm::fvec2 uv;
    glm::fvec2 mn;
    glm::fvec4 rgba;


    StandardVertexData(float x = 0., float y = 0., float z = 0.,
                       float u = 0., float v = 0.,
                       float m = 0., float n = 0.,
                       float r = 1., float g = 1., float b = 1., float a = 1.)
    : xyz(x, y, z), uv(u, v), mn(m, n), rgba(r, g, b, a) {};

    static inline StandardVertexData XY_UV(float x, float y, float u, float v)
    {
        return StandardVertexData(x, y, 0., u, v);
    }

    static inline StandardVertexData XY_UV_MN(float x, float y, float u, float v, float m, float n)
    {
        return StandardVertexData(x, y, 0., u, v, m, n);
    }

    static void align_position_center(std::vector<StandardVertexData>& vertices);
};


struct Renderer {
    uint32_t clear_flags = BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH;
    uint32_t reset_flags = BGFX_RESET_VSYNC | BGFX_RESET_MSAA_X2;

    bgfx::VertexDecl vertex_decl;

    std::unique_ptr<Image> default_image;

    bgfx::UniformHandle texture_uniform;
    bgfx::ProgramHandle default_program;
    // TODO replace with default_image
    bgfx::TextureHandle default_texture;

    glm::fmat4 projection_matrix;
    glm::uvec2 view_size;
    glm::uvec2 border_size;

    Renderer(const glm::uvec2& window_size);
    ~Renderer();

    void begin_frame();
    void end_frame();

    void reset();

    void render_vertices(const std::vector<StandardVertexData>& vertices,
                         const std::vector<VertexIndex>& indices,
                         const bgfx::TextureHandle texture) const;
};

} // namespace kaacore
