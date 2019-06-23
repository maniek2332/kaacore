#include <iostream>
#include <cstdlib>
#include <vector>

#include <glm/glm.hpp>

#include "kaacore/engine.h"
#include "kaacore/scenes.h"
#include "kaacore/nodes.h"
#include "kaacore/geometry.h"
#include "kaacore/log.h"

using namespace kaacore;


struct PolygonTesterDemoScene : Scene {
    std::vector<glm::dvec2> points;
    Node* shape_repr;
    Engine* engine;

    PolygonTesterDemoScene()
    {
        this->engine = get_engine();
        this->camera.size = this->engine->window->size();
        this->camera.refresh();

        this->shape_repr = new Node();
        this->shape_repr->set_position({0, 0});
        this->shape_repr->set_shape(Shape::Box({3, 3}));
        this->root_node.add_child(this->shape_repr);
    }

    void add_point(const glm::dvec2& p)
    {
        if (not this->points.empty() and this->points.back() == p) {
            return;
        }
        Node* point_node = new Node();
        point_node->set_position(p);
        point_node->set_shape(Shape::Circle(1.));
        this->root_node.add_child(point_node);

        if (this->points.size()) {
            this->add_segment(p, this->points.back());
        }
        this->points.push_back(p);
    }

    void add_segment(const glm::dvec2& a, const glm::dvec2& b)
    {
        Node* segment_node = new Node();
        segment_node->set_position(a);
        segment_node->set_shape(Shape::Segment({0, 0}, b - a));
        this->root_node.add_child(segment_node);
    }

    void finalize_polygon() {
        this->add_segment(this->points.back(), this->points.front());
        auto center = find_points_center(this->points);
        for (auto& pt : this->points) {
            pt -= center;
        }
        auto polygon_type = classify_polygon(this->points);
        log("Polygon type: %d", static_cast<int>(polygon_type));
        if (polygon_type != PolygonType::not_convex) {
            this->shape_repr->set_shape(Shape::Polygon(this->points));
        } else {
            log<LogLevel::error>("Polygon not convex!");
        }
        this->points.clear();
    }

    void update(uint32_t dt) override
    {
        for (auto const& event : this->get_events()) {
            if (event.is_pressing(Keycode::q) or event.is_quit()) {
                get_engine()->quit();
                break;
            } else if (event.is_pressing(Mousecode::left)) {
                auto pos = event.get_mouse_position();
                log("Adding point: (%f, %f)", pos.x, pos.y);
                auto window_size = this->engine->window->size();
                this->add_point(pos);
            } else if (event.is_pressing(Keycode::f)) {
                log("Finalizing polygon");
                this->finalize_polygon();
            }
        }
    }
};


extern "C" int main(int argc, char *argv[])
{
    Engine eng;
    eng.window->show();
    PolygonTesterDemoScene scene;
    eng.run(&scene);

    return 0;
}
