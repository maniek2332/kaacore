#include <SDL.h>

#include "kaacore/log.h"

#include "kaacore/input.h"


namespace kaacore {

Event::Event() : sdl_event()
{}

Event::Event(SDL_Event sdl_event, EngineEventCode engine_code)
    : sdl_event(sdl_event), engine_code(engine_code)
{}

bool Event::is_quit() const
{
    return this->sdl_event.type == SDL_QUIT;
}

bool Event::is_keyboard_event() const
{
    return (this->sdl_event.type == SDL_KEYUP or
            this->sdl_event.type == SDL_KEYDOWN);
}

bool Event::is_mouse_event() const
{
    return (this->sdl_event.type == SDL_MOUSEBUTTONUP or
            this->sdl_event.type == SDL_MOUSEBUTTONDOWN);
}

bool Event::is_pressing(Keycode kc) const
{
    return (this->sdl_event.type == SDL_KEYDOWN and
            this->sdl_event.key.keysym.sym == static_cast<SDL_Keycode>(kc));
}

bool Event::is_pressing(Mousecode mc) const
{
    return (this->sdl_event.type == SDL_MOUSEBUTTONDOWN or
            this->sdl_event.button.button == static_cast<uint8_t>(mc));
}

bool Event::is_releasing(Keycode kc) const
{
    return (this->sdl_event.type == SDL_KEYUP and
            this->sdl_event.key.keysym.sym == static_cast<SDL_Keycode>(kc));
}

bool Event::is_releasing(Mousecode mc) const
{
    return (this->sdl_event.type == SDL_MOUSEBUTTONUP or
            this->sdl_event.button.button == static_cast<uint8_t>(mc));
}

glm::dvec2 Event::get_mouse_position() const
{
    return glm::dvec2(this->sdl_event.button.x, this->sdl_event.button.y);
}

InputManager::InputManager()
{
    this->engine_event_type = SDL_RegisterEvents(1);
}

void InputManager::push_event(SDL_Event sdl_event)
{
    if (sdl_event.type == this->engine_event_type) {
        this->events_queue.emplace_back(sdl_event,
                                        EngineEventCode(sdl_event.user.code));
    } else {
        this->events_queue.emplace_back(sdl_event);
    }
}

void InputManager::generate_engine_event(const EngineEventCode engine_code)
{
    SDL_Event sdl_event;
    SDL_memset(&sdl_event, 0, sizeof(sdl_event));
    sdl_event.type = this->engine_event_type;
    sdl_event.user.code = int32_t(engine_code);
    if(!SDL_PushEvent(&sdl_event)) {
        log<LogLevel::error>("Failed to queue event: %s", SDL_GetError());
    }
}

void InputManager::clear_events()
{
    this->events_queue.clear();
}

bool InputManager::is_pressed(Keycode kc) const
{
    auto scancode = SDL_GetScancodeFromKey(static_cast<SDL_Keycode>(kc));
    return SDL_GetKeyboardState(NULL)[scancode] == 1;
}

bool InputManager::is_pressed(Mousecode mc) const
{
    return (SDL_GetMouseState(NULL, NULL) &
            SDL_BUTTON(static_cast<uint8_t>(mc)));
}

bool InputManager::is_released(Keycode kc) const
{
    auto scancode = SDL_GetScancodeFromKey(static_cast<SDL_Keycode>(kc));
    return SDL_GetKeyboardState(NULL)[scancode] == 0;
}

bool InputManager::is_released(Mousecode mc) const
{
    return not (SDL_GetMouseState(NULL, NULL) &
                SDL_BUTTON(static_cast<uint8_t>(mc)));
}

glm::dvec2 InputManager::get_mouse_position() const
{
    int pos_x, pos_y;
    SDL_GetMouseState(&pos_x, &pos_y);
    return glm::dvec2(pos_x, pos_y);
}

} // namespace kaacore
