/***********************************************************************************************************************
 * @file
 * @brief The program's main file.
 **********************************************************************************************************************/

#pragma once

#include <array>
#include <vector>




namespace n_tdg
{

constexpr int g_worldW {256}; //!< The world's width in tiles.
constexpr int g_worldH {256}; //!< The world's height in tiles.

extern std::array<std::array<unsigned char, g_worldH>, g_worldW> g_staticObjs; //!< The static objects of the world.

class c_playerCharacter;

extern std::vector<c_playerCharacter> g_playerCharacters;

/***********************************************************************************************************************
 * @brief Checks whether a world-space position is inside the world's boundaries.
 * @param p_x, p_y The world-space position.
 * @return True if the world-space position is inside the world's boundaries.
 **********************************************************************************************************************/
template<typename tp_t> bool fg_isPosInWorldBounds(tp_t p_x, tp_t p_y)
{
    return
        p_x >= static_cast<tp_t>(0)       &&
        p_x < static_cast<tp_t>(g_worldW) &&
        p_y >= static_cast<tp_t>(0)       &&
        p_y < static_cast<tp_t>(g_worldH);
}

}