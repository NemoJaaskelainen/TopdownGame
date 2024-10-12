/***********************************************************************************************************************
 * @file
 * @brief The program's main file.
 * @todo The placeable target-objects are just static objects without special functionality.
 **********************************************************************************************************************/

#if 1

    #include "main.hpp"
    #include "input.hpp"
    #include "time.hpp"
    #include "playerCharacter.hpp"

    #include <algorithm>
    #include <array>
    #include <cmath>
    #include <cstdlib>
    #include <filesystem>
    #include <format>
    #include <fstream>
    #include <iostream>
    #include <numbers>
    #include <string>
    #include <tuple>
    #include <unordered_map>

    #include <SDL.h>
    #include <SDL_image.h>

    using namespace n_tdg::n_input;
    using namespace n_tdg::n_time;

    using namespace std;

#endif




namespace n_tdg
{

namespace
{

SDL_Window   *g_window   {};    //!< The game window.
SDL_Renderer *g_renderer {};    //!< The game window's renderer.
float         g_windowW  {800}; //!< The game window's width in pixels.
float         g_windowH  {600}; //!< The game window's height in pixels.

unordered_map<string, SDL_Texture *> g_textures {}; //!< The game's textures.

float g_tileW {32.f}; //!< The width of a tile in pixels. Must be > 0. @sa fg_setTileSizeAndCenter
float g_tileH {32.f}; //!< The height of a tile in pixels. Must be > 0. @sa fg_setTileSizeAndCenter

float g_viewportX {0.f};   //!< The viewport's X-position as a world-space coordinate.
float g_viewportY {0.f};   //!< The viewport's Y-position as a world-space coordinate.
float g_viewportW {800.f}; //!< The viewport's width in pixels, without scaling. @sa fg_setViewportSizeAndCenter
float g_viewportH {600.f}; //!< The viewport's height in pixels, without scaling. @sa fg_setViewportSizeAndCenter

//! A placement mode, for @c g_currentPlacementMode.
enum class e_placementMode {ev_walls, ev_targets};
//! The current placement mode.
e_placementMode g_currentPlacementMode {e_placementMode::ev_walls};

/***********************************************************************************************************************
 * @brief Loads a texture from the given path.
 * @param p_path The path to the texture.
 * @return A pointer to the loaded texture. A @c nullptr on failure.
 **********************************************************************************************************************/
SDL_Texture *fg_loadTexture(const string &p_path)
{
    SDL_Texture *l_texture        {};
    SDL_Surface *l_textureSurface {IMG_Load(p_path.c_str())};

    if (!l_textureSurface)
        return nullptr;

    l_texture = SDL_CreateTextureFromSurface(g_renderer, l_textureSurface);
    SDL_FreeSurface(l_textureSurface);

    if (!l_texture)
        return nullptr;

    return l_texture;
}

/***********************************************************************************************************************
 * @brief Loads the program's resources, such as textures.
 * @return True on success.
 **********************************************************************************************************************/
bool fg_loadResources()
{
    constexpr string_view l_textureDir {"textures"};

    for (const auto &l_entry : filesystem::recursive_directory_iterator {l_textureDir})
    {
        if (l_entry.is_regular_file() && l_entry.path().extension().string() == ".png")
        {
            string l_textureName {l_entry.path().relative_path().string().erase(0u, l_textureDir.size() + 1u)};

            replace(l_textureName.begin(), l_textureName.end(), '\\', '/');

            g_textures[l_textureName] = fg_loadTexture(l_entry.path().string());

            if (!g_textures[l_textureName])
            {
                cout << "Failed to load the texture \""
                     << l_textureName
                     << "\". SDL Error: "
                     << SDL_GetError()
                     << '\n';

                g_textures.erase(l_textureName);

                return false;
            }

            cout << "Loaded the texture \"" << l_textureName << "\".\n";
        }
    }

    return true;
}

/***********************************************************************************************************************
 * @brief Generates the world.
 **********************************************************************************************************************/
void fg_generateWorld()
{
    for (size_t l_y {}; l_y != g_worldH; ++l_y)
        for (size_t l_x {}; l_x != g_worldW; ++l_x)
        {
            if (l_y % 1u == 0u)
                g_staticObjs[l_x][l_y] = 1u; else
                g_staticObjs[l_x][l_y] = 0u;
        }
}

/***********************************************************************************************************************
 * @brief Initializes the program.
 * @return True on success.
 **********************************************************************************************************************/
bool fg_init()
{
    // SDL.

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        cout << "Could not initialize SDL! SDL error: " << SDL_GetError();
        return false;
    }

    // SDL_image.

    if (!IMG_Init(IMG_INIT_PNG))
    {
        cout << "Could not initialize SDL_image! SDL error: " << SDL_GetError();
        return false;
    }

    // The window.

    g_window = SDL_CreateWindow
    (
        "TopdownGame",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        g_windowW,
        g_windowH,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );

    if (!g_window)
    {
        cout << "Could not create the main SDL window! SDL error: " << SDL_GetError();
        return false;
    }

    // The renderer.

    g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);

    if (!g_renderer)
    {
        cout << "Could not create the main SDL window's renderer! SDL error: " << SDL_GetError();
        return false;
    }

    // Resources.

    if (!fg_loadResources())
    {
        cout << "Failed to load a resource!";
        return false;
    }

    // World generation.

    fg_generateWorld();

    return true;
}

/***********************************************************************************************************************
 * @brief Prepares the program for termination.
 **********************************************************************************************************************/
void fg_prepareForTermination()
{
    for (const auto &[l_key, l_texture] : g_textures)
        SDL_DestroyTexture(l_texture);

    SDL_DestroyRenderer(g_renderer);
    SDL_DestroyWindow(g_window);
    IMG_Quit();
    SDL_Quit();
}

/***********************************************************************************************************************
 * @brief Gets the viewport's scale multiplier.
 **********************************************************************************************************************/
float fg_getViewportScaleMult()
{
    float l_scaleMult {min(static_cast<float>(g_windowW) / g_viewportW, static_cast<float>(g_windowH) / g_viewportH)};
    return min(1.f, l_scaleMult);
}

/***********************************************************************************************************************
 * @return The amounts of viewport X-padding and Y-padding.
 **********************************************************************************************************************/
pair<float, float> fg_getViewportPaddings()
{
    float l_viewportW {round(g_viewportW * fg_getViewportScaleMult())};
    float l_viewportH {round(g_viewportH * fg_getViewportScaleMult())};
    float l_padX      {max(0.f, ceil((g_windowW - l_viewportW) * .5f))};
    float l_padY      {max(0.f, ceil((g_windowH - l_viewportH) * .5f))};

    return {l_padX, l_padY};
}

/***********************************************************************************************************************
 * @brief Gets a world-space position from a window-space position.
 * @param p_x, p_y The window-space position.
 * @return The world-space position.
 **********************************************************************************************************************/
pair<float, float> fg_getWorldSpacePos(float p_x, float p_y)
{
    auto [l_padX, l_padY] {fg_getViewportPaddings()};
    float l_posX {(p_x - l_padX) / fg_getViewportScaleMult() / g_tileW + g_viewportX};
    float l_posY {(p_y - l_padY) / fg_getViewportScaleMult() / g_tileH + g_viewportY};

    return {l_posX, l_posY};
}

/***********************************************************************************************************************
 * @brief Centers the viewport to the given world-space position.
 * @param p_x, p_y The world-space position.
 **********************************************************************************************************************/
void fg_centerViewport(float p_x, float p_y)
{
    g_viewportX = p_x - g_viewportW * .5f / g_tileW;
    g_viewportY = p_y - g_viewportH * .5f / g_tileH;
}

/***********************************************************************************************************************
 * @brief Sets the viewport's size, and keeps the viewport's center at the same world-space position.
 * @param p_w, p_h The new width and height of the viewport.
 **********************************************************************************************************************/
void fg_setViewportSizeAndCenter(float p_w, float p_h)
{
    if (g_viewportW == p_w && g_viewportH == p_h)
        return;
    
    auto [l_padX, l_padY]       {fg_getViewportPaddings()};
    float l_a                   {g_viewportW * fg_getViewportScaleMult() * .5f + l_padX};
    float l_b                   {g_viewportH * fg_getViewportScaleMult() * .5f + l_padY};
    auto [l_centerX, l_centerY] {fg_getWorldSpacePos(l_a, l_b)};

    g_viewportW = p_w;
    g_viewportH = p_h;

    fg_centerViewport(l_centerX, l_centerY);
}

/***********************************************************************************************************************
 * @brief Sets the size of a tile, and keeps the viewport's center at the same world-space position. This effectively
 * functions as a zooming function.
 * @param p_w, p_h The new width and height of a tile in pixels. If this is < 1, set to 1.
 **********************************************************************************************************************/
void fg_setTileSizeAndCenter(float p_w, float p_h)
{
    if (g_tileW == p_w && g_tileH == p_h)
        return;
    
    p_w = max(1.f, p_w);
    p_h = max(1.f, p_h);
    
    auto [l_padX, l_padY]       {fg_getViewportPaddings()};
    float l_a                   {g_viewportW * fg_getViewportScaleMult() * .5f + l_padX};
    float l_b                   {g_viewportH * fg_getViewportScaleMult() * .5f + l_padY};
    auto [l_centerX, l_centerY] {fg_getWorldSpacePos(l_a, l_b)};

    g_tileW = p_w;
    g_tileH = p_h;

    fg_centerViewport(l_centerX, l_centerY);
}

/***********************************************************************************************************************
 * @brief Draws the tiles of the given position. Used in @c fg_drawWorld.
 * @param p_x, p_y The position of the drawable tiles, in tile units.
 * @param p_viewportX, p_viewportY The position of the viewport, in pixels.
 * @param p_tileW, p_tileH The width and height of the drawable tiles, in pixels.
 * @param p_padX, p_padY The X-padding and Y-padding of the viewport, in pixels.
 * @todo The player characters are drawn here. This should probably be done differently.
 **********************************************************************************************************************/
void fg_drawTile
(int p_x, int p_y, float p_viewportX, float p_viewportY, float p_tileW, float p_tileH, float p_padX, float p_padY)
{
    auto fl_drawCenteredTexture = [&](const string &l_textureName, int p_textureW, int p_textureH)
    {
        auto &l_texture {g_textures[l_textureName]};
        float l_textureWRatio {static_cast<float>(p_textureW) / 32.f};
        float l_textureHRatio {static_cast<float>(p_textureH) / 32.f};

        float l_posOffsetMultX {.5f - l_textureWRatio * .5f};
        float l_posOffsetMultY {.5f - l_textureHRatio * .5f};

        SDL_Rect l_renderRect
        {
            static_cast<int>(static_cast<float>(p_x) * p_tileW + p_padX - p_viewportX + p_tileW * l_posOffsetMultX),
            static_cast<int>(static_cast<float>(p_y) * p_tileH + p_padY - p_viewportY + p_tileH * l_posOffsetMultY),
            static_cast<int>(ceil(p_tileW * l_textureWRatio)),
            static_cast<int>(ceil(p_tileH * l_textureHRatio))
        };

        SDL_RenderCopy(g_renderer, l_texture, nullptr, &l_renderRect);
    };

    for (const auto &l_i : g_playerCharacters)
    {
        if (l_i.f_getPos().first == p_x && l_i.f_getPos().second == p_y)
            fl_drawCenteredTexture("tex_playerCharacter_32x32.png", 32, 32);
    }

    switch (g_staticObjs[p_x][p_y])
    {
        case 1u: fl_drawCenteredTexture("tex_wall_32x32.png", 32, 32); break;
        case 2u: fl_drawCenteredTexture("tex_target_16x16.png", 16, 16); break;
    }
}

/***********************************************************************************************************************
 * @brief Draws the game world.
 * @todo The viewport padding's transparency is not final, but for testing and demonstration purposes.
 **********************************************************************************************************************/
void fg_drawWorld()
{
    // Synchronizes the window-size variables.
    {
        int l_w, l_h;
        SDL_GetWindowSize(g_window, &l_w, &l_h);
        g_windowW = static_cast<float>(l_w);
        g_windowH = static_cast<float>(l_h);
    }

    // Scales the variables required for drawing to the viewport.

    auto [l_padX, l_padY] {fg_getViewportPaddings()};
    float l_viewportW     {round(g_viewportW * fg_getViewportScaleMult())};
    float l_viewportH     {round(g_viewportH * fg_getViewportScaleMult())};
    float l_viewportX     {g_viewportX * g_tileW * fg_getViewportScaleMult()};
    float l_viewportY     {g_viewportY * g_tileH * fg_getViewportScaleMult()};
    float l_tileW         {g_tileW * fg_getViewportScaleMult()};
    float l_tileH         {g_tileH * fg_getViewportScaleMult()};

    // Calculates the visible tile ranges.

    float l_a {l_tileW == 0.f ? 0.f : l_viewportX / l_tileW};
    float l_b {l_tileH == 0.f ? 0.f : l_viewportY / l_tileH};

    int l_fromX {static_cast<int>(l_a)};
    int l_toX   {static_cast<int>(ceil(l_a + (l_tileW <= 0.f ? 0.f : l_viewportW / l_tileW)))};
    int l_fromY {static_cast<int>(l_b)};
    int l_toY   {static_cast<int>(ceil(l_b + (l_tileH <= 0.f ? 0.f : l_viewportH / l_tileH)))};

    // Draws the world.

    for (int l_y {l_fromY}; l_y != l_toY; ++l_y)
        for (int l_x {l_fromX}; l_x != l_toX; ++l_x)
            if (l_x >= 0 && l_y >= 0 && l_x < g_worldW && l_y < g_worldH)
                fg_drawTile(l_x, l_y, l_viewportX, l_viewportY, l_tileW, l_tileH, l_padX, l_padY);

    // Draws the viewport padding.

    int l_windowWInt {static_cast<int>(g_windowW)};
    int l_windowHInt {static_cast<int>(g_windowH)};
    int l_padXInt {static_cast<int>(l_padX)};
    int l_padYInt {static_cast<int>(l_padY)};
    SDL_Rect l_padTop    {0,                        0,                        l_windowWInt, l_padYInt};
    SDL_Rect l_padBottom {0,                        l_windowHInt - l_padYInt, l_windowWInt, l_padYInt};
    SDL_Rect l_padLeft   {0,                        l_padYInt,                l_padXInt,    l_windowHInt - l_padYInt * 2};
    SDL_Rect l_padRight  {l_windowWInt - l_padXInt, l_padYInt,                l_padXInt,    l_windowHInt - l_padYInt * 2};

    SDL_SetRenderDrawBlendMode(g_renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(g_renderer, 0u, 0u, 0u, 127);
    SDL_RenderFillRect(g_renderer, &l_padTop);
    SDL_RenderFillRect(g_renderer, &l_padBottom);
    SDL_RenderFillRect(g_renderer, &l_padLeft);
    SDL_RenderFillRect(g_renderer, &l_padRight);
}

}

array<array<unsigned char, g_worldH>, g_worldW> g_staticObjs {};

vector<c_playerCharacter> g_playerCharacters {};

}




/***********************************************************************************************************************
 * @brief The main function.
 * @param p_argCount The number of the program arguments.
 * @param p_args The program arguments.
 **********************************************************************************************************************/
int main([[maybe_unused]] int p_argCount, [[maybe_unused]] char *p_args[])
{
    using namespace n_tdg;

    if (!fg_init())
    {
        fg_prepareForTermination();
        return EXIT_FAILURE;
    }

    fg_registerKeybind(ev_moveUp,    SDLK_w);
    fg_registerKeybind(ev_moveDown,  SDLK_s);
    fg_registerKeybind(ev_moveLeft,  SDLK_a);
    fg_registerKeybind(ev_moveRight, SDLK_d);
    fg_registerKeybind(ev_moveFaster, SDLK_LSHIFT);
    fg_registerKeybind(ev_setPfGoal, SDLK_SPACE);

    fg_registerKeybindAxis(ev_moveUpDown,    ev_moveUp,   ev_moveDown);
    fg_registerKeybindAxis(ev_moveLeftRight, ev_moveLeft, ev_moveRight);

    fg_registerKeybind(ev_placeWalls, SDLK_1);
    fg_registerKeybind(ev_placeTargets, SDLK_2);

    g_staticObjs[1u][1u] = 0u;
    g_playerCharacters.push_back({1, 1});
    size_t l_playerGoalX {1u};
    size_t l_playerGoalY {1u};

    while (fg_handleInputEvents())
    {
        fg_handleTimingOfMainTick();

        SDL_SetRenderDrawColor(g_renderer, 0u, 63u, 0u, 255u);
        SDL_RenderClear(g_renderer);

        fg_drawWorld();

        // Viewport movement.
        {
            float l_speed {(fg_isKeybindDown(ev_moveFaster) ? 4.f : 1.f) * fg_getDTime()};
            g_viewportX += l_speed * fg_getKeybindAxisVal<float>(ev_moveLeftRight);
            g_viewportY += l_speed * fg_getKeybindAxisVal<float>(ev_moveUpDown);
        }

        // Zooming.
        fg_setTileSizeAndCenter(g_tileW + fg_getScrollAmountY() * 2.f, g_tileH + fg_getScrollAmountY() * 2.f);

        // Object placement.
        {
            if (fg_wasKeybindPressed(ev_placeWalls))
                g_currentPlacementMode = e_placementMode::ev_walls;
            else if (fg_wasKeybindPressed(ev_placeTargets))
                g_currentPlacementMode = e_placementMode::ev_targets;

            auto [l_pointerPosX, l_pointerPosY] {fg_getWorldSpacePos(fg_getPointerX(), fg_getPointerY())};

            if (fg_isPosInWorldBounds(l_pointerPosX, l_pointerPosY))
            {
                size_t l_tileX {static_cast<size_t>(l_pointerPosX)};
                size_t l_tileY {static_cast<size_t>(l_pointerPosY)};

                if (fg_isPointerPrimaryDown())
                {
                    switch (g_currentPlacementMode)
                    {
                        case e_placementMode::ev_walls:
                            g_staticObjs[l_tileX][l_tileY] = 1u;
                            break;

                        case e_placementMode::ev_targets:
                            g_staticObjs[l_tileX][l_tileY] = 2u;
                    }
                }
                else if (fg_isPointerSecondaryDown())
                {
                    g_staticObjs[l_tileX][l_tileY] = 0u;
                }
            }
        }

        // The player character's movement.
        {
            if (fg_isKeybindDown(ev_setPfGoal))
            {
                auto [l_pointerPosX, l_pointerPosY] {fg_getWorldSpacePos(fg_getPointerX(), fg_getPointerY())};

                if (fg_isPosInWorldBounds(l_pointerPosX, l_pointerPosY))
                {
                    l_playerGoalX = static_cast<size_t>(l_pointerPosX);
                    l_playerGoalY = static_cast<size_t>(l_pointerPosY);
                }
            }

            g_playerCharacters.front().f_pfMoveTowardsGoal(l_playerGoalX, l_playerGoalY);
        }

        SDL_RenderPresent(g_renderer);
    }

    fg_prepareForTermination();
    return EXIT_SUCCESS;
}