/***********************************************************************************************************************
 * @file
 * @brief The source file of @c c_playerCharacter.
 **********************************************************************************************************************/

#if 1

    #include "playerCharacter.hpp"

    #include <algorithm>
    #include <array>
    #include <cmath>
    #include <cstdlib>
    #include <filesystem>
    #include <format>
    #include <fstream>
    #include <iostream>
    #include <string>
    #include <tuple>
    #include <unordered_map>
    #include <vector>

    #include <SDL.h>

    using namespace std;
    using namespace n_tdg;

#endif




namespace n_tdg
{

namespace
{

constexpr int g_nodeMaxHealth {15};

}

// Private members.
#if 1

    bool c_playerCharacter::fs_pfIsPosNearWall(int p_x, int p_y)
    {
        for (int l_x {p_x - 1}; l_x != p_x + 2; ++l_x)
            for (int l_y {p_y - 1}; l_y != p_y + 2; ++l_y)
            {
                if (!fg_isPosInWorldBounds(l_x, l_y) || g_staticObjs[l_x][l_y] != 0u)
                    return true;
            }
        
        return false;
    }

    pair<int, int> c_playerCharacter::fs_pfDirToOffset(e_pfNodeDir p_dir)
    {
        switch (p_dir)
        {
            case e_pfNodeDir::ev_right: return { 1,  0};
            case e_pfNodeDir::ev_down:  return { 0,  1};
            case e_pfNodeDir::ev_left:  return {-1,  0};
            case e_pfNodeDir::ev_up:    return { 0, -1};
            default:                    return { 1,  0};
        }
    }

    c_playerCharacter::e_pfNodeDir c_playerCharacter::fs_pfOffsetToDir(int p_offsetX, int p_offsetY)
    {
        if (p_offsetX ==  1 && p_offsetY ==  0) return e_pfNodeDir::ev_right;
        if (p_offsetX ==  0 && p_offsetY ==  1) return e_pfNodeDir::ev_down;
        if (p_offsetX == -1 && p_offsetY ==  0) return e_pfNodeDir::ev_left;
        if (p_offsetX ==  0 && p_offsetY == -1) return e_pfNodeDir::ev_up;
        return e_pfNodeDir::ev_right;
    }

    c_playerCharacter::e_pfNodeDir c_playerCharacter::fs_pfReverseDir(e_pfNodeDir p_dir)
    {
        switch (p_dir)
        {
            case e_pfNodeDir::ev_right: return e_pfNodeDir::ev_left;
            case e_pfNodeDir::ev_down:  return e_pfNodeDir::ev_up;
            case e_pfNodeDir::ev_left:  return e_pfNodeDir::ev_right;
            case e_pfNodeDir::ev_up:    return e_pfNodeDir::ev_down;
            default:                    return e_pfNodeDir::ev_right;
        }
    }

    bool
    c_playerCharacter::fs_pfSpreadNode(int p_fromX, int p_fromY, e_pfNodeDir p_dir, int p_creatorHealth, int p_goalX,
    int p_goalY, t_pfNodesArr &p_pfNodes, t_pfNodePositions &p_newPfNodePositions)
    {
        auto [l_offsetX, l_offsetY] {fs_pfDirToOffset(p_dir)};
        int l_x {p_fromX + l_offsetX};
        int l_y {p_fromY + l_offsetY};

        if (!fg_isPosInWorldBounds(l_x, l_y) || g_staticObjs[l_x][l_y] != 0u)
            return false;

        auto &l_node {p_pfNodes[l_x][l_y]};

        if (l_node && l_node->v_health != 0)
            return false;
        
        l_node = make_unique<c_pfNode>();
        l_node->v_health = fs_pfIsPosNearWall(l_x, l_y) ? g_nodeMaxHealth : p_creatorHealth - 1;
        l_node->v_dir = fs_pfReverseDir(p_dir);

        if (l_node->v_health != 0)
            p_newPfNodePositions.push_back({l_x, l_y});
        
        return l_x == p_goalX && l_y == p_goalY;
    }

    void c_playerCharacter::f_pfProcessNodesIntoPath(int p_goalX, int p_goalY, t_pfNodesArr &p_pfNodes)
    {
        v_pfPath = make_unique<t_pfNodesArr>();
        v_pfGoalX = p_goalX;
        v_pfGoalY = p_goalY;

        int l_x {p_goalX};
        int l_y {p_goalY};

        e_pfNodeDir l_dirToPrev   {};
        bool        l_isFirstNode {true};

        while (true)
        {
            if (l_x == v_posX && l_y == v_posY)
            {
                auto &l_startNode {(*v_pfPath)[l_x][l_y]};
                l_startNode = make_unique<c_pfNode>();
                l_startNode->v_dir = l_dirToPrev;
                break;
            }

            auto &l_pathNode {(*v_pfPath)[l_x][l_y]};
            l_pathNode = move(p_pfNodes[l_x][l_y]);

            auto [l_offsetX, l_offsetY] {fs_pfDirToOffset(l_pathNode->v_dir)};
            l_x += l_offsetX;
            l_y += l_offsetY;

            if (l_isFirstNode)
                l_isFirstNode = false; else
                l_pathNode->v_dir = l_dirToPrev;

            l_dirToPrev = fs_pfOffsetToDir(-l_offsetX, -l_offsetY);
        }
    }

    bool c_playerCharacter::f_pfBuildPathTo(int p_goalX, int p_goalY)
    {
        using enum e_pfNodeDir;

        v_pfPath.reset();

        t_pfNodesArr      l_nodes                {};
        t_pfNodePositions l_processablePositions {};
        t_pfNodePositions l_newPositions         {};

        if
        (
            fs_pfSpreadNode(v_posX, v_posY, ev_right, g_nodeMaxHealth, p_goalX, p_goalY, l_nodes, l_newPositions) ||
            fs_pfSpreadNode(v_posX, v_posY, ev_down,  g_nodeMaxHealth, p_goalX, p_goalY, l_nodes, l_newPositions) ||
            fs_pfSpreadNode(v_posX, v_posY, ev_left,  g_nodeMaxHealth, p_goalX, p_goalY, l_nodes, l_newPositions) ||
            fs_pfSpreadNode(v_posX, v_posY, ev_up,    g_nodeMaxHealth, p_goalX, p_goalY, l_nodes, l_newPositions)
        )
        {
            f_pfProcessNodesIntoPath(p_goalX, p_goalY, l_nodes);
            return true; // Found the goal.
        }

        while (true)
        {
            l_processablePositions = move(l_newPositions);
            l_newPositions.clear();

            for (auto [l_x, l_y] : l_processablePositions)
            {
                const auto &l_node {l_nodes[l_x][l_y]};
                
                if
                (
                    fs_pfSpreadNode(l_x, l_y, ev_right, l_node->v_health, p_goalX, p_goalY, l_nodes, l_newPositions) ||
                    fs_pfSpreadNode(l_x, l_y, ev_down,  l_node->v_health, p_goalX, p_goalY, l_nodes, l_newPositions) ||
                    fs_pfSpreadNode(l_x, l_y, ev_left,  l_node->v_health, p_goalX, p_goalY, l_nodes, l_newPositions) ||
                    fs_pfSpreadNode(l_x, l_y, ev_up,    l_node->v_health, p_goalX, p_goalY, l_nodes, l_newPositions)
                )
                {
                    f_pfProcessNodesIntoPath(p_goalX, p_goalY, l_nodes);
                    return true; // Found the goal.
                }
            }

            if (l_processablePositions.empty())
                return false; // Didn't find the goal.
        }
    }

#endif

// Public members.
#if 1

    c_playerCharacter::c_playerCharacter(int p_posX, int p_posY) : v_posX {p_posX}, v_posY {p_posY}
    {

    }

    std::pair<int, int> c_playerCharacter::f_getPos() const
    {
        return {v_posX, v_posY};
    }

    void c_playerCharacter::f_setPos(int p_posX, int p_posY)
    {
        v_posX = p_posX;
        v_posY = p_posY;
    }

    c_playerCharacter::e_pfMoveResult c_playerCharacter::f_pfMoveTowardsGoal(int p_goalX, int p_goalY)
    {
        if (v_posX == p_goalX && v_posY == p_goalY)
            return e_pfMoveResult::ev_reachedGoal;

        if (!v_pfPath || p_goalX != v_pfGoalX || p_goalY != v_pfGoalY)
            if (!f_pfBuildPathTo(p_goalX, p_goalY))
                return e_pfMoveResult::ev_cannotReachGoal;
        
        if (SDL_GetTicks64() < v_nextMoveTime)
            return e_pfMoveResult::ev_continue;

        auto &l_pathNode {(*v_pfPath)[v_posX][v_posY]};
        auto [l_offsetX, l_offsetY] {fs_pfDirToOffset(l_pathNode->v_dir)};
        int l_nextPosX {v_posX + l_offsetX};
        int l_nextPosY {v_posY + l_offsetY};

        if (g_staticObjs[l_nextPosX][l_nextPosY] != 0u)
        {
            v_pfPath.reset();
            return f_pfMoveTowardsGoal(p_goalX, p_goalY);
        }

        v_posX = l_nextPosX;
        v_posY = l_nextPosY;

        v_nextMoveTime = SDL_GetTicks64() + static_cast<uint64_t>(50);

        if (v_posX == p_goalX && v_posY == p_goalY)
            return e_pfMoveResult::ev_reachedGoal;

        return e_pfMoveResult::ev_continue;
    }

#endif

}