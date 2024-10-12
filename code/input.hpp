/***********************************************************************************************************************
 * @file
 * @brief For handling input events.
 **********************************************************************************************************************/

#pragma once

#include <concepts>

#include <SDL.h>




namespace n_tdg::n_input
{

//! IDs for keybinds.
enum e_keybindIds
{
    ev_moveUp,
    ev_moveDown,
    ev_moveLeft,
    ev_moveRight,
    ev_moveFaster,
    ev_placeWalls,
    ev_placeTargets,
    ev_setPfGoal
};

//! IDs for keybind axes.
enum e_keybindAxisIds
{
    ev_moveUpDown,
    ev_moveLeftRight
};




// Class declarations.
namespace np_input
{
    /*******************************************************************************************************************
     * @brief A keybind that binds an ID to an SDL keycode. Only for internal use in input.hpp and input.cpp.
     ******************************************************************************************************************/
    class c_keybind
    {
        public:

        int         v_id      {};             //!< The ID of the keybind.
        SDL_Keycode v_keycode {SDLK_UNKNOWN}; //!< The SDL keycode of the keybind.

        bool v_wasPressed {}; //!< Whether the key was pressed on the last input-event handling.
        bool v_isDown     {}; //!< Whether the key was held down on the last input-event handling.

        /***************************************************************************************************************
         * @brief Creates a keybind with the given ID and SDL keycode.
         * @param p_id The ID of the keybind.
         * @param p_keycode The SDL keycode that activates the keybind.
         **************************************************************************************************************/
        c_keybind(int p_id, SDL_Keycode p_keycode);

        /***************************************************************************************************************
         * @return True if the key was pressed on the last input-event handling.
         **************************************************************************************************************/
        bool f_wasPressed() const;

        /***************************************************************************************************************
         * @return True if the key was pressed or held down on the last input-event handling.
         **************************************************************************************************************/
        bool f_isDown() const;
    };

    /*******************************************************************************************************************
     * @brief A keybind axis for tracking the last pressed keybind from two given keybinds. Only for internal use in
     * input.hpp and input.cpp.
     ******************************************************************************************************************/
    class c_keybindAxis
    {
        public:

        int v_id                   {}; //!< The ID of the keybind axis.
        int v_negativeKeybindId    {}; //!< The ID of the keybind which produces a negative keybind axis value.
        int v_positiveKeybindId    {}; //!< The ID of the keybind which produces a positive keybind axis value.
        int v_lastPressedKeybindId {}; //!< The ID of the last-pressed keybind.

        /***************************************************************************************************************
         * @brief Creates a keybind axis with the given ID and keybind IDs.
         * @param p_id The ID of the keybind axis.
         * @param p_negativeKeybindId The ID of the keybind which produces a negative keybind axis value.
         * @param p_positiveKeybindId The ID of the keybind which produces a positive keybind axis value.
         **************************************************************************************************************/
        c_keybindAxis(int p_id, int p_negativeKeybindId, int p_positiveKeybindId);
    };
}




/***********************************************************************************************************************
 * @return True if the pointer's primary button was pressed on the last input-event handling.
 **********************************************************************************************************************/
bool fg_wasPointerPrimaryPressed();

/***********************************************************************************************************************
 * @return True if the pointer's secondary button was pressed on the last input-event handling.
 **********************************************************************************************************************/
bool fg_wasPointerSecondaryPressed();

/***********************************************************************************************************************
 * @return True if the pointer's middle button was pressed on the last input-event handling.
 **********************************************************************************************************************/
bool fg_wasPointerMiddlePressed();

/***********************************************************************************************************************
 * @return True if the pointer's primary button was pressed or held down on the last input-event handling.
 **********************************************************************************************************************/
bool fg_isPointerPrimaryDown();

/***********************************************************************************************************************
 * @return True if the pointer's secondary button was pressed or held down on the last input-event handling.
 **********************************************************************************************************************/
bool fg_isPointerSecondaryDown();

/***********************************************************************************************************************
 * @return True if the pointer's middle button was pressed or held down on the last input-event handling.
 **********************************************************************************************************************/
bool fg_isPointerMiddleDown();

/***********************************************************************************************************************
 * @brief Gets a keybind with the given ID. Only for internal use in input.hpp and input.cpp.
 * @param p_id The ID of the gettable keybind.
 * @return The keybind with the given ID, or @c nullptr if it doesn't exist.
 **********************************************************************************************************************/
np_input::c_keybind *fg_getKeybind(int p_id);

/***********************************************************************************************************************
 * @brief Checks whether there's a keybind registered with the given ID.
 * @param p_id The ID.
 * @return True if there's a keybind registered with the given ID.
 **********************************************************************************************************************/
bool fg_isKeybindRegistered(int p_id);

/***********************************************************************************************************************
 * @brief Registers a keybind with the given ID.
 * @param p_id The ID.
 * @param p_keycode The SDL keycode for the keybind.
 * @return False if a keybind with the same ID already exists.
 **********************************************************************************************************************/
bool fg_registerKeybind(int p_id, SDL_Keycode p_keycode);

/***********************************************************************************************************************
 * @brief Unregisters the keybind of the given ID.
 * @param p_id The ID.
 * @return False if the keybind of the given ID doesn't exist.
 **********************************************************************************************************************/
bool fg_unregisterKeybind(int p_id);

/***********************************************************************************************************************
 * @brief Changes the keycode of the given ID's keybind.
 * @param p_id The ID.
 * @param p_keycode The new keycode.
 * @return False if the keybind of the given ID doesn't exist.
 **********************************************************************************************************************/
bool fg_changeKeybindKeycode(int p_id, SDL_Keycode p_keycode);

/***********************************************************************************************************************
 * @brief Checks whether the keybind of the given ID was pressed on the last input-event handling.
 * @param p_id The ID.
 * @return True if the keybind of the given ID was pressed on the last input-event handling.
 **********************************************************************************************************************/
bool fg_wasKeybindPressed(int p_id);

/***********************************************************************************************************************
 * @brief Checks whether the keybind of the given ID was pressed or held down on the last input-event handling.
 * @param p_id The ID.
 * @return True if the keybind of the given ID was pressed or held down on the last input-event handling.
 **********************************************************************************************************************/
bool fg_isKeybindDown(int p_id);

/***********************************************************************************************************************
 * @brief Gets a keybind axis with the given ID. Only for internal use in input.hpp and input.cpp.
 * @param p_id The ID of the gettable keybind axis.
 * @return The keybind axis with the given ID, or @c nullptr if it doesn't exist.
 **********************************************************************************************************************/
np_input::c_keybindAxis *fg_getKeybindAxis(int p_id);

/***********************************************************************************************************************
 * @brief Checks whether there's a keybind axis registered with the given ID.
 * @param p_id The ID.
 * @return True if there's a keybind axis registered with the given ID.
 **********************************************************************************************************************/
bool fg_isKeybindAxisRegistered(int p_id);

/***********************************************************************************************************************
 * @brief Registers a keybind axis with the given ID.
 * @param p_id The ID of the keybind axis.
 * @param p_negativeKeybindId The ID of the keybind which produces a negative keybind axis value.
 * @param p_positiveKeybindId The ID of the keybind which produces a positive keybind axis value.
 * @return False if a keybind axis with the same ID already exists.
 **********************************************************************************************************************/
bool fg_registerKeybindAxis(int p_id, int p_negativeKeybindId, int p_positiveKeybindId);

/***********************************************************************************************************************
 * @brief Unregisters the keybind axis of the given ID.
 * @param p_id The ID.
 * @return False if the keybind axis of the given ID doesn't exist.
 **********************************************************************************************************************/
bool fg_unregisterKeybindAxis(int p_id);

/***********************************************************************************************************************
 * @brief Changes the keybinds of the given ID's keybind axis.
 * @param p_id The ID of the keybind axis.
 * @param p_negativeKeybindId The ID of the new keybind which produces a negative keybind axis value.
 * @param p_positiveKeybindId The ID of the new keybind which produces a positive keybind axis value.
 * @return False if the keybind axis of the given ID doesn't exist.
 **********************************************************************************************************************/
bool fg_changeKeybindAxisKeybinds(int p_id, int p_negativeKeybindId, int p_positiveKeybindId);




//! A template macro for @c fg_getKeybindAxisVal. For clarity.
#define tmp_getKeybindAxisVal                                                                                          \
    template                                                                                                           \
    <                                                                                                                  \
        typename tp_t = int,                                                                                           \
        tp_t tp_negativeVal = static_cast<tp_t>(-1),                                                                   \
        tp_t tp_neutralVal = static_cast<tp_t>(0),                                                                     \
        tp_t tp_positiveVal = static_cast<tp_t>(1)                                                                     \
    >                                                                                                                  \
    requires std::is_arithmetic_v<tp_t>

/***********************************************************************************************************************
 * @brief Gets a keybind axis' value.
 * @tparam tp_t The type of the returnable value. @c int by default.
 * @tparam tp_negativeVal The negative axis value. -1 by default.
 * @tparam tp_neutralVal  The neutral axis value. 0 by default.
 * @tparam tp_positiveVal The positive axis value. 1 by default.
 * @param p_id The ID of the keybind axis. Allowed to be invalid.
 * @return @p tp_negativeVal or @p tp_positiveVal depending on the last-pressed keybind if both are down, or
 * depending on the currently pressed keybind if only one is down. @p tp_neutralVal if neither keybind is down, or
 * if the keybind axis or one of its keybinds doesn't exist.
 **********************************************************************************************************************/
tmp_getKeybindAxisVal tp_t fg_getKeybindAxisVal(int p_id)
{
    np_input::c_keybindAxis *l_keybindAxis {fg_getKeybindAxis(p_id)};

    if (!l_keybindAxis)
        return tp_neutralVal;

    np_input::c_keybind *l_keybindNegative {fg_getKeybind(l_keybindAxis->v_negativeKeybindId)};
    np_input::c_keybind *l_keybindPositive {fg_getKeybind(l_keybindAxis->v_positiveKeybindId)};

    if (!l_keybindNegative || !l_keybindPositive)
        return tp_neutralVal;
    
    if (!l_keybindNegative->f_isDown() && !l_keybindPositive->f_isDown())
        return tp_neutralVal;
    
    if (l_keybindNegative->f_isDown() && l_keybindPositive->f_isDown())
        return l_keybindAxis->v_lastPressedKeybindId == l_keybindAxis->v_negativeKeybindId ?
            tp_negativeVal : tp_positiveVal;
    
    return l_keybindNegative->f_isDown() ? tp_negativeVal : tp_positiveVal;
}

#undef tmp_getKeybindAxisVal




/***********************************************************************************************************************
 * @return The pointer's X-position, in pixels.
 * @sa fg_getPointerY
 **********************************************************************************************************************/
float fg_getPointerX();

/***********************************************************************************************************************
 * @return The pointer's Y-position, in pixels.
 * @sa fg_getPointerX
 **********************************************************************************************************************/
float fg_getPointerY();

/***********************************************************************************************************************
 * @return The amount of pointer X-movement on the last input-event handling, in pixels.
 * @sa fg_getPointerMovY
 **********************************************************************************************************************/
float fg_getPointerMovX();

/***********************************************************************************************************************
 * @return The amount of pointer Y-movement on the last input-event handling, in pixels.
 * @sa fg_getPointerMovX
 **********************************************************************************************************************/
float fg_getPointerMovY();

/***********************************************************************************************************************
 * @return The amount of X-scrolling on the last input-event handling.
 * @sa fg_getScrollAmountY
 **********************************************************************************************************************/
float fg_getScrollAmountX();

/***********************************************************************************************************************
 * @return The amount of Y-scrolling on the last input-event handling.
 * @sa fg_getScrollAmountX
 **********************************************************************************************************************/
float fg_getScrollAmountY();

/*******************************************************************************************************************
 * @brief Handles all input events. Should typically be called from the program's main loop.
 * @return False in case of an @c SDL_QUIT event.
 ******************************************************************************************************************/
bool fg_handleInputEvents();

}