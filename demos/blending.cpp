#include <cstdlib>

#include <glm/glm.hpp>

#include "kaacore/engine.h"
#include "kaacore/shapes.h"
#include "kaacore/blending.h"
#include "kaacore/scenes.h"

namespace kaacore { namespace demos {

struct BlendingDemoScene : Scene {
    Shape default_shape;
    glm::dvec4 color_1;
    glm::dvec4 color_2;

    BlendingDemoScene()
        : default_shape(Shape::Box({70., 25.})), color_1({0., 1., 1., 0.7}), color_2({1., 1., 0., 0.7})
    {
        this->camera().position({0., 0.});

        this->create_pair(
            {-350, -350},
            BlendingMode(),
            BlendingMode()
        );
        this->create_pair(
            {-250, -350},
            BlendingMode(),
            BlendingMode(BlendingFactor::one, BlendingFactor::one, BlendingEquation::max)
        );
        this->create_pair(
            {-250, -250},
            BlendingMode(),
            BlendingMode(BlendingFactor::one, BlendingFactor::one, BlendingFactor::src_alpha, BlendingFactor::inverted_src_alpha, BlendingEquation::max, BlendingEquation::add)
        );
        this->create_pair(
            {-150, -350},
            BlendingMode(),
            BlendingMode(BlendingFactor::one, BlendingFactor::one, BlendingEquation::min)
        );
        this->create_pair(
            {-150, -250},
            BlendingMode(),
            BlendingMode(BlendingFactor::one, BlendingFactor::one, BlendingFactor::src_alpha, BlendingFactor::inverted_src_alpha, BlendingEquation::min, BlendingEquation::add)
        );
        this->create_pair(
            {-50, -350},
            BlendingMode(),
            BlendingMode(BlendingFactor::one, BlendingFactor::zero, BlendingEquation::add)
        );
    }

    void create_pair(const glm::dvec2 slot_position, const BlendingMode blending_1, const BlendingMode blending_2)
    {
        auto parent_node = make_node();
        parent_node->position(slot_position);

        auto node_1 = make_node();
        node_1->position({-10., -5.});
        node_1->shape(this->default_shape);
        node_1->color(this->color_1);
        node_1->z_index(1);
        node_1->blending_mode(blending_1);

        auto node_2 = make_node();
        node_2->position({10., 5.});
        node_2->shape(this->default_shape);
        node_2->color(this->color_2);
        node_2->z_index(2);
        node_2->blending_mode(blending_2);

        parent_node->add_child(node_1);
        parent_node->add_child(node_2);
        this->root_node.add_child(parent_node);
    }

    void update(const kaacore::Duration dt) override
    {
        for (auto const& event : this->get_events()) {
            auto keyboard_key = event.keyboard_key();
            if (keyboard_key and keyboard_key->key() == kaacore::Keycode::q) {
                kaacore::get_engine()->quit();
                break;
            }
        }
    }
};

}} // namespace kaacore::demos


extern "C" int
main(int argc, char* argv[])
{
    kaacore::Engine eng({800, 800});
    kaacore::demos::BlendingDemoScene scene;
    eng.run(&scene);

    return 0;
}
