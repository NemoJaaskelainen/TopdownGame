/***********************************************************************************************************************
 * @file
 * @brief For handling input events.
 **********************************************************************************************************************/

#include "input.hpp"

#include <vector>

using namespace std;




namespace n_tdg::n_input
{

namespace
{

//! The pointer's X-position, in pixels.
float g_pointerX {};
//! The pointer's Y-position, in pixels.
float g_pointerY {};
//! The amount of pointer X-movement on the last input-event handling, in pixels.
float g_pointerMovX {};
//! The amount of pointer Y-movement on the last input-event handling, in pixels.
float g_pointerMovY {};
//! The amount of X-scrolling on the last input-event handling.
float g_scrollAmountX {};
//! The amount of Y-scrolling on the last input-event handling.
float g_scrollAmountY {};

//! Whether the pointer's primary button was pressed on the last input-event handling.
bool g_pointerPrimaryWasPressed {};
//! Whether the pointer's primary button was held down on the last input-event handling.
bool g_pointerPrimaryIsDown {};
//! Whether the pointer's secondary button was pressed on the last input-event handling.
bool g_pointerSecondaryWasPressed {};
//! Whether the pointer's secondary button was held down on the last input-event handling.
bool g_pointerSecondaryIsDown {};
//! Whether the pointer's middle button was pressed on the last input-event handling.
bool g_pointerMiddleWasPressed {};
//! Whether the pointer's middle button was held down on the last input-event handling.
bool g_pointerMiddleIsDown {};

vector<np_input::c_keybind>     g_keybinds    {}; //!< The register for keybinds.
vector<np_input::c_keybindAxis> g_keybindAxes {}; //!< The register for keybind axes.

/***********************************************************************************************************************
 * @brief Handles an SDL input event of the type @c SDL_KEYUP or @c SDL_KEYDOWN.
 * @param p_event The SDL input event.
 **********************************************************************************************************************/
void fg_handleInputEventOfKeyType(const SDL_Event &p_event)
{
    if (p_event.key.repeat != 0u)
        return;

    if (p_event.type == SDL_KEYUP)
    {
        for (auto &l_keybind : g_keybinds)
            if (l_keybind.v_keycode == p_event.key.keysym.sym)
                l_keybind.v_isDown = false;
        
        return;
    }

    for (auto &l_keybind : g_keybinds)
    {
        if (l_keybind.v_keycode == p_event.key.keysym.sym)
        {
            l_keybind.v_isDown = true;
            l_keybind.v_wasPressed = true;

            for (auto &l_keybindAxis : g_keybindAxes)
            {
                if
                (
                    l_keybindAxis.v_negativeKeybindId == l_keybind.v_id ||
                    l_keybindAxis.v_positiveKeybindId == l_keybind.v_id
                )
                {
                    l_keybindAxis.v_lastPressedKeybindId = l_keybind.v_id;
                }
            }
        }
    }
}

/***********************************************************************************************************************
 * @brief Handles an SDL input event of the type @c SDL_MOUSEMOTION, @c SDL_MOUSEBUTTONDOWN, or @c SDL_MOUSEBUTTONUP.
 * @param p_event The SDL input event.
 **********************************************************************************************************************/
void fg_handleInputEventOfMouseType(const SDL_Event &p_event)
{
    switch (p_event.type)
    {
        case SDL_MOUSEMOTION:
            g_pointerX = static_cast<float>(p_event.motion.x);
            g_pointerY = static_cast<float>(p_event.motion.y);
            g_pointerMovX += static_cast<float>(p_event.motion.xrel);
            g_pointerMovY += static_cast<float>(p_event.motion.yrel);
            return;
        
        case SDL_MOUSEBUTTONDOWN:
            switch (p_event.button.button)
            {
                case 1u:
                    g_pointerPrimaryIsDown = true;
                    g_pointerPrimaryWasPressed = true;
                    return;
                case 2u:
                    g_pointerMiddleIsDown = true;
                    g_pointerMiddleWasPressed = true;
                    return;
                case 3u:
                    g_pointerSecondaryIsDown = true;
                    g_pointerSecondaryWasPressed = true;
                    return;
                default:
                    return;
            }

        case SDL_MOUSEBUTTONUP:
            switch (p_event.button.button)
            {
                case 1u:
                    g_pointerPrimaryIsDown = false;
                    return;
                case 2u:
                    g_pointerMiddleIsDown = false;
                    return;
                case 3u:
                    g_pointerSecondaryIsDown = false;
                    return;
                default:
                    return;
            }
        
        case SDL_MOUSEWHEEL:
            g_scrollAmountX += p_event.wheel.preciseX;
            g_scrollAmountY += p_event.wheel.preciseY;
    }
}

}




np_input::c_keybind::c_keybind(int p_id, SDL_Keycode p_keycode) : v_id {p_id}, v_keycode {p_keycode}
{

};

bool np_input::c_keybind::f_wasPressed() const
{
    return v_wasPressed;
}

bool np_input::c_keybind::f_isDown() const
{
    return v_isDown || v_wasPressed;
}

np_input::c_keybindAxis::c_keybindAxis(int p_id, int p_negativeKeybindId, int p_positiveKeybindId) :
v_id {p_id},
v_negativeKeybindId {p_negativeKeybindId},
v_positiveKeybindId {p_positiveKeybindId},
v_lastPressedKeybindId {p_positiveKeybindId}
{

};




bool fg_wasPointerPrimaryPressed()
{
    return g_pointerPrimaryWasPressed;
}

bool fg_wasPointerSecondaryPressed()
{
    return g_pointerSecondaryWasPressed;
}

bool fg_wasPointerMiddlePressed()
{
    return g_pointerMiddleWasPressed;
}

bool fg_isPointerPrimaryDown()
{
    return g_pointerPrimaryIsDown || g_pointerPrimaryWasPressed;
}

bool fg_isPointerSecondaryDown()
{
    return g_pointerSecondaryIsDown || g_pointerSecondaryWasPressed;
}

bool fg_isPointerMiddleDown()
{
    return g_pointerMiddleIsDown || g_pointerMiddleWasPressed;
}

np_input::c_keybind *fg_getKeybind(int p_id)
{
    for (auto &l_i : g_keybinds)
        if (l_i.v_id == p_id)
            return &l_i;

    return nullptr;
}

bool fg_isKeybindRegistered(int p_id)
{
    return fg_getKeybind(p_id);
}

bool fg_registerKeybind(int p_id, SDL_Keycode p_keycode)
{
    if (fg_isKeybindRegistered(p_id))
        return false;
    
    g_keybinds.push_back({p_id, p_keycode});
    return true;
}

bool fg_unregisterKeybind(int p_id)
{
    for (auto l_i {g_keybinds.begin()}; l_i != g_keybinds.end(); ++l_i)
        if (l_i->v_id == p_id)
        {
            g_keybinds.erase(l_i);
            return true;
        }
    
    return false;
}

bool fg_changeKeybindKeycode(int p_id, SDL_Keycode p_keycode)
{
    np_input::c_keybind *l_keybind {fg_getKeybind(p_id)};

    if (!l_keybind)
        return false;
    
    l_keybind->v_keycode = p_keycode;
    return true;
}

bool fg_wasKeybindPressed(int p_id)
{
    const np_input::c_keybind *l_keybind {fg_getKeybind(p_id)};

    if (!l_keybind)
        return false;
    
    return l_keybind->f_wasPressed();
}

bool fg_isKeybindDown(int p_id)
{
    const np_input::c_keybind *l_keybind {fg_getKeybind(p_id)};

    if (!l_keybind)
        return false;

    return l_keybind->f_isDown();
}

np_input::c_keybindAxis *fg_getKeybindAxis(int p_id)
{
    for (auto &l_i : g_keybindAxes)
        if (l_i.v_id == p_id)
            return &l_i;

    return nullptr;
}

bool fg_isKeybindAxisRegistered(int p_id)
{
    return fg_getKeybindAxis(p_id);
}

bool fg_registerKeybindAxis(int p_id, int p_negativeKeybindId, int p_positiveKeybindId)
{
    if (fg_isKeybindAxisRegistered(p_id))
        return false;

    g_keybindAxes.push_back({p_id, p_negativeKeybindId, p_positiveKeybindId});
    return true;
}

bool fg_unregisterKeybindAxis(int p_id)
{
    for (auto l_i {g_keybindAxes.begin()}; l_i != g_keybindAxes.end(); ++l_i)
        if (l_i->v_id == p_id)
        {
            g_keybindAxes.erase(l_i);
            return true;
        }
    
    return false;
}

bool fg_changeKeybindAxisKeybinds(int p_id, int p_negativeKeybindId, int p_positiveKeybindId)
{
    np_input::c_keybindAxis *l_keybindAxis {fg_getKeybindAxis(p_id)};

    if (!l_keybindAxis)
        return false;

    l_keybindAxis->v_negativeKeybindId = p_negativeKeybindId;
    l_keybindAxis->v_positiveKeybindId = p_positiveKeybindId;
    l_keybindAxis->v_lastPressedKeybindId = p_positiveKeybindId;
    return true;
}

float fg_getPointerX()
{
    return g_pointerX;
}

float fg_getPointerY()
{
    return g_pointerY;
}

float fg_getPointerMovX()
{
    return g_pointerMovX;
}

float fg_getPointerMovY()
{
    return g_pointerMovY;
}

float fg_getScrollAmountX()
{
    return g_scrollAmountX;
}

float fg_getScrollAmountY()
{
    return g_scrollAmountY;
}

bool fg_handleInputEvents()
{
    g_pointerMovX                = 0.f;
    g_pointerMovY                = 0.f;
    g_scrollAmountX              = 0.f;
    g_scrollAmountY              = 0.f;
    g_pointerPrimaryWasPressed   = false;
    g_pointerSecondaryWasPressed = false;
    g_pointerMiddleWasPressed    = false;

    for (auto &l_i : g_keybinds)
        l_i.v_wasPressed = false;

    for (SDL_Event l_event; SDL_PollEvent(&l_event);)
    {
        switch (l_event.type)
        {
            case SDL_KEYDOWN: [[fallthrough]];
            case SDL_KEYUP:
                fg_handleInputEventOfKeyType(l_event);
                break;

            case SDL_MOUSEMOTION:     [[fallthrough]];
            case SDL_MOUSEBUTTONDOWN: [[fallthrough]];
            case SDL_MOUSEBUTTONUP:   [[fallthrough]];
            case SDL_MOUSEWHEEL:
                fg_handleInputEventOfMouseType(l_event);
                break;

            case SDL_QUIT:
                return false;
        }
    }

    return true;
}

}