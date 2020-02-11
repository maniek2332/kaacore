#include <iostream>
#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "kaacore/engine.h"
#include "kaacore/fonts.h"
#include "kaacore/input.h"
#include "kaacore/log.h"
#include "kaacore/node_transitions.h"
#include "kaacore/nodes.h"
#include "kaacore/scenes.h"

using namespace kaacore;

static const std::string txt_lorem_ipsum =
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Suspendisse "
    "ultricies lacus massa. Phasellus tempus convallis ligula, et fermentum "
    "mauris tincidunt a. Donec consequat felis sed massa suscipit "
    "pellentesque. Etiam ullamcorper lacinia arcu ut vehicula. Morbi mattis "
    "lacus velit, nec tincidunt diam vulputate sit amet. Maecenas fermentum "
    "sagittis justo, id lacinia justo auctor ut. Maecenas mollis neque sit "
    "amet tortor porttitor lobortis.";

struct DemoFontsScene : Scene {
    Node* background;
    Node* node_text;

    DemoFontsScene()
    {
        auto font = Font::load("demos/assets/fonts/Roboto/Roboto-Regular.ttf");

        this->background = new Node();
        this->background->shape(Shape::Box({700, 570}));
        this->background->color({0.5, 0.5, 0.5, 1.});
        this->background->z_index(-10);
        this->root_node.add_child(this->background);

        this->node_text = new Node(NodeType::text);
        this->node_text->position({200., 0.});
        this->node_text->text.font(font);
        this->node_text->text.content(txt_lorem_ipsum);
        this->node_text->text.font_size(24.);
        this->node_text->text.line_width(270.);
        this->node_text->text.first_line_indent(15.);
        this->node_text->color({0., 0., 0., 1.});
        this->root_node.add_child(this->node_text);

        this->node_text->transition(make_node_transitions_parallel({
            make_node_transitions_sequence(
                {make_node_transition<NodePositionTransition>(
                     glm::dvec2(200., 200.), 2000.),
                 make_node_transition<NodePositionTransition>(
                     glm::dvec2(0., 300.), 2000.),
                 make_node_transition<NodeScaleTransition>(
                     glm::dvec2(1., 1.), 500.),
                 make_node_transition<NodePositionTransition>(
                     glm::dvec2(-200., -500.), 8000.)}),
            make_node_transition<NodeColorTransition>(
                glm::dvec4(1., 1., 1., 0.5), 10000.),
        }));
    }

    void update(uint32_t dt) override
    {
        for (auto const& event : this->get_events()) {
            auto system = event.system();
            if (system and system->quit()) {
                get_engine()->quit();
                break;
            }

            if (auto keyboard = event.keyboard()) {
                if (keyboard->is_pressing(Keycode::q)) {
                    get_engine()->quit();
                    break;
                } else if (keyboard->is_pressing(Keycode::w)) {
                    this->camera.position += glm::dvec2(0., -2.5);
                    this->camera.refresh();
                } else if (keyboard->is_pressing(Keycode::a)) {
                    this->camera.position += glm::dvec2(-2.5, 0.);
                    this->camera.refresh();
                } else if (keyboard->is_pressing(Keycode::s)) {
                    this->camera.position += glm::dvec2(0., 2.5);
                    this->camera.refresh();
                } else if (keyboard->is_pressing(Keycode::d)) {
                    this->camera.position += glm::dvec2(2.5, 0.);
                    this->camera.refresh();
                } else if (keyboard->is_pressing(Keycode::i)) {
                    this->camera.scale += glm::dvec2(0.1, 0.1);
                    this->camera.refresh();
                } else if (keyboard->is_pressing(Keycode::o)) {
                    this->camera.scale -= glm::dvec2(0.1, 0.1);
                    this->camera.refresh();
                } else if (keyboard->is_pressing(Keycode::l)) {
                    this->node_text->text.content(
                        this->node_text->text.content() + "x");
                } else if (keyboard->is_pressing(Keycode::k)) {
                    this->node_text->text.content(
                        this->node_text->text.content() + " ");
                }
            }
        }
    }
};

extern "C" int
main(int argc, char* argv[])
{
    Engine eng({800, 600});
    DemoFontsScene scene;
    scene.camera.position = {0., 0.};
    eng.window->show();
    eng.run(&scene);

    return 0;
}
