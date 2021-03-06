#include <chrono>
#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "kaacore/engine.h"
#include "kaacore/input.h"
#include "kaacore/log.h"
#include "kaacore/nodes.h"
#include "kaacore/scenes.h"
#include "kaacore/timers.h"

using namespace std::chrono_literals;

struct DemoScene : kaacore::Scene {
    kaacore::NodePtr node;
    kaacore::Duration duration;
    kaacore::Timer timer;

    DemoScene()
    {
        auto node = kaacore::make_node();
        node->position({0, 0});
        node->color({1., 0., 0., 1});
        node->shape(kaacore::Shape::Box({100., 100.}));
        this->node = this->root_node.add_child(node);
        this->duration = 1.s;

        this->timer = kaacore::Timer([this](kaacore::TimerContext context) {
            KAACORE_APP_LOG_INFO("Timer callback called.");
            this->node->visible(not this->node->visible());
            return context.interval;
        });
        this->timer.start(this->duration, this);
    }

    void update(const kaacore::Duration dt) override
    {
        for (auto const& event : this->get_events()) {
            if (auto keyboard_key = event.keyboard_key()) {
                if (keyboard_key->is_key_down()) {
                    if (keyboard_key->key() == kaacore::Keycode::q) {
                        kaacore::get_engine()->quit();
                    } else if (keyboard_key->key() == kaacore::Keycode::s) {
                        if (this->timer.is_running()) {
                            this->timer.stop();
                        } else {
                            this->timer.start(this->duration, this);
                        }
                    }
                }
            }
        }
    }
};

extern "C" int
main(int argc, char* argv[])
{
    kaacore::Engine eng({800, 600});
    DemoScene scene;
    scene.camera().position({0., 0.});
    eng.run(&scene);

    return 0;
}
