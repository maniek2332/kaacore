#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "kaacore/easings.h"
#include "kaacore/node_ptr.h"
#include "kaacore/nodes.h"
#include "kaacore/transitions.h"

namespace kaacore {

enum struct AttributeTransitionMethod {
    set = 1,
    add = 2,
    multiply = 3,
};

template<typename T>
struct NodeAttributeTransitionState : TransitionStateBase {
    T origin_value;
    T destination_value;

    NodeAttributeTransitionState(
        T origin, T value_advance, AttributeTransitionMethod advance_method)
        : origin_value(origin)
    {
        if (advance_method == AttributeTransitionMethod::set) {
            this->destination_value = value_advance;
        } else if (advance_method == AttributeTransitionMethod::add) {
            this->destination_value = origin + value_advance;
        } else {
            this->destination_value = origin * value_advance;
        }
    }
};

template<
    typename T, typename N, N Node::*N_member, T (N::*F_getter)(),
    void (N::*F_setter)(const T&)>
class SpecializedNodeAttributeTransition : public NodeTransitionCustomizable {
    AttributeTransitionMethod _advance_method;
    T _value_advance;

  public:
    SpecializedNodeAttributeTransition(
        T value_advance, const AttributeTransitionMethod& advance_method,
        const double duration,
        const TransitionWarping& warping = TransitionWarping(),
        const Easing easing = Easing::none)
        : NodeTransitionCustomizable(duration, warping, easing),
          _value_advance(value_advance), _advance_method(advance_method)
    {}

    SpecializedNodeAttributeTransition(
        T value_advance, const double duration,
        const TransitionWarping& warping = TransitionWarping(),
        const Easing easing = Easing::none)
        : SpecializedNodeAttributeTransition(
              value_advance, AttributeTransitionMethod::set, duration, warping,
              easing)
    {}

    std::unique_ptr<TransitionStateBase> prepare_state(NodePtr node) const
    {
        if constexpr (std::is_same_v<N, Node>) {
            static_assert(N_member == nullptr);
            return std::make_unique<NodeAttributeTransitionState<T>>(
                ((node.get())->*F_getter)(), this->_value_advance,
                this->_advance_method);
        } else {
            static_assert(N_member != nullptr);
            N* spec_node = &((node.get())->*N_member);
            return std::make_unique<NodeAttributeTransitionState<T>>(
                (spec_node->*F_getter)(), this->_value_advance,
                this->_advance_method);
        }
    }

    void evaluate(
        TransitionStateBase* state_b, NodePtr node, const double t) const
    {
        if constexpr (std::is_same_v<N, Node>) {
            static_assert(N_member == nullptr);
            auto state = static_cast<NodeAttributeTransitionState<T>*>(state_b);
            T new_value =
                glm::mix(state->origin_value, state->destination_value, t);
            ((node.get())->*F_setter)(new_value);
        } else {
            static_assert(N_member != nullptr);
            auto state = static_cast<NodeAttributeTransitionState<T>*>(state_b);
            N* spec_node = &((node.get())->*N_member);
            T new_value =
                glm::mix(state->origin_value, state->destination_value, t);
            (spec_node->*F_setter)(new_value);
        }
    }
};

typedef SpecializedNodeAttributeTransition<
    glm::dvec2, Node, nullptr, &Node::position, &Node::position>
    NodePositionTransition;

typedef SpecializedNodeAttributeTransition<
    double, Node, nullptr, &Node::rotation, &Node::rotation>
    NodeRotationTransition;

typedef SpecializedNodeAttributeTransition<
    glm::dvec2, Node, nullptr, &Node::scale, &Node::scale>
    NodeScaleTransition;

typedef SpecializedNodeAttributeTransition<
    glm::dvec4, Node, nullptr, &Node::color, &Node::color>
    NodeColorTransition;

typedef SpecializedNodeAttributeTransition<
    glm::dvec2, BodyNode, &Node::body, &BodyNode::velocity, &BodyNode::velocity>
    BodyNodeVelocityTransition;

typedef SpecializedNodeAttributeTransition<
    double, BodyNode, &Node::body, &BodyNode::angular_velocity,
    &BodyNode::angular_velocity>
    BodyNodeAngularVelocityTransition;

class NodeSpriteTransition : public NodeTransitionCustomizable {
    std::vector<Sprite> _frames;
    size_t _frames_count;

  public:
    NodeSpriteTransition(
        const std::vector<Sprite>& frames, const double duration,
        const TransitionWarping& warping = TransitionWarping());

    std::unique_ptr<TransitionStateBase> prepare_state(NodePtr node) const;
    void evaluate(
        TransitionStateBase* state_b, NodePtr node, const double t) const;
};

} // namespace kaacore
