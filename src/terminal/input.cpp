#include <cppurses/terminal/input.hpp>

#include <cstddef>
#include <memory>
#include <utility>

#include <ncurses.h>

#include <cppurses/system/detail/find_widget_at.hpp>
#include <cppurses/system/event.hpp>
#include <cppurses/system/events/key.hpp>
#include <cppurses/system/events/mouse.hpp>
#include <cppurses/system/events/resize_event.hpp>
#include <cppurses/system/focus.hpp>
#include <cppurses/system/shortcuts.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/area.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/widget.hpp>

namespace {
using namespace cppurses;

/// Check if mouse_event is a button_mask type of event.
template <typename Mask_t>
auto is(Mask_t button_mask, const ::MEVENT& mouse_event) -> bool
{
    return static_cast<bool>(mouse_event.bstate & button_mask);
}

/// Extract the Event type and Mouse::Button from a given MEVENT object.
auto extract_info(const ::MEVENT& mouse_event)
    -> std::pair<Event::Type, Mouse::Button>
{
    auto type_button = std::make_pair(Event::None, Mouse::Button::None);
    auto& type       = type_button.first;
    auto& button     = type_button.second;
    // Button 1 / Left Button
    if (is(BUTTON1_PRESSED, mouse_event)) {
        type   = Event::MouseButtonPress;
        button = Mouse::Button::Left;
    }
    else if (is(BUTTON1_RELEASED, mouse_event)) {
        type   = Event::MouseButtonRelease;
        button = Mouse::Button::Left;
    }
    // Button 2 / Middle Button
    else if (is(BUTTON2_PRESSED, mouse_event)) {
        type   = Event::MouseButtonPress;
        button = Mouse::Button::Middle;
    }
    else if (is(BUTTON2_RELEASED, mouse_event)) {
        type   = Event::MouseButtonRelease;
        button = Mouse::Button::Middle;
    }
    // Button 3 / Right Button
    else if (is(BUTTON3_PRESSED, mouse_event)) {
        type   = Event::MouseButtonPress;
        button = Mouse::Button::Right;
    }
    else if (is(BUTTON3_RELEASED, mouse_event)) {
        type   = Event::MouseButtonRelease;
        button = Mouse::Button::Right;
    }
    // Button 4 / Scroll Up
    else if (is(BUTTON4_PRESSED, mouse_event)) {
        type   = Event::MouseButtonPress;
        button = Mouse::Button::ScrollUp;
    }
    else if (is(BUTTON4_RELEASED, mouse_event)) {
        type   = Event::MouseButtonRelease;
        button = Mouse::Button::ScrollUp;
    }
    // Button 5 / Scroll Down
#if defined(BUTTON5_PRESSED) && defined(BUTTON5_RELEASED)
    else if (is(BUTTON5_PRESSED, mouse_event)) {
        type   = Event::MouseButtonPress;
        button = Mouse::Button::ScrollDown;
    }
    else if (is(BUTTON5_RELEASED, mouse_event)) {
        type   = Event::MouseButtonRelease;
        button = Mouse::Button::ScrollDown;
    }
#endif
    return type_button;
}

auto make_mouse_event() -> std::unique_ptr<Event>
{
    auto mouse_event = ::MEVENT{};
    if (::getmouse(&mouse_event) != OK)
        return nullptr;
    Widget* receiver = detail::find_widget_at(mouse_event.x, mouse_event.y);
    if (receiver == nullptr)
        return nullptr;

    // Coordinates
    const auto global = Point{static_cast<std::size_t>(mouse_event.x),
                              static_cast<std::size_t>(mouse_event.y)};
    const auto local =
        Point{global.x - receiver->inner_x(), global.y - receiver->inner_y()};

    // Create Event
    const auto type_button = extract_info(mouse_event);
    const auto type        = type_button.first;
    const auto button      = type_button.second;
    if (type == Event::MouseButtonPress) {
        return std::make_unique<Mouse::Press>(
            *receiver, Mouse::State{button, global, local, mouse_event.id});
    }
    if (type == Event::MouseButtonRelease) {
        return std::make_unique<Mouse::Release>(
            *receiver, Mouse::State{button, global, local, mouse_event.id});
    }
    return nullptr;
}

auto make_resize_event() -> std::unique_ptr<Event>
{
    Widget* const receiver = System::head();
    if (receiver != nullptr) {
        const auto width  = System::terminal.width();
        const auto height = System::terminal.height();
        return std::make_unique<Resize_event>(*receiver, Area{width, height});
    }
    return nullptr;
}

auto make_keyboard_event(int input) -> std::unique_ptr<Event>
{
    const auto code = static_cast<Key::Code>(input);
    Widget* const receiver =
        Shortcuts::send_key(code) ? nullptr : Focus::focus_widget();
    return receiver == nullptr ? nullptr
                               : std::make_unique<Key::Press>(*receiver, code);
}
}  // namespace

namespace cppurses {
namespace input {

auto get() -> std::unique_ptr<Event>
{
    const auto input = ::getch();
    switch (input) {
        case ERR: return nullptr;  // Timeout and no event.
        case KEY_MOUSE: return make_mouse_event();
        case KEY_RESIZE: return make_resize_event();
        default: return make_keyboard_event(input);  // Key_event
    }
}

}  // namespace input
}  // namespace cppurses
