/***********************************************************************************************************************
 * @file
 * @brief The header file of @c c_playerCharacter.
 * @todo Needs comments.
 **********************************************************************************************************************/

#pragma once

#include "main.hpp"

#include <array>
#include <memory>
#include <vector>




namespace n_tdg
{

/***********************************************************************************************************************
 * @brief A player character with pathfinding capabilities.
 **********************************************************************************************************************/
class c_playerCharacter
{
    private:

    enum class e_pfNodeDir {ev_right, ev_down, ev_left, ev_up};

    class c_pfNode
    {
        public:
        int v_health {};
        e_pfNodeDir v_dir {};
    };

    using t_pfNodesArr = std::array<std::array<std::unique_ptr<c_pfNode>, g_worldH>, g_worldW>;
    using t_pfNodePositions = std::vector<std::pair<int, int>>;

    std::unique_ptr<t_pfNodesArr> v_pfPath {};
    uint64_t v_nextMoveTime {};

    int v_posX {};
    int v_posY {};

    int v_pfGoalX {};
    int v_pfGoalY {};

    static bool fs_pfIsPosNearWall(int p_x, int p_y);

    static std::pair<int, int> fs_pfDirToOffset(e_pfNodeDir p_dir);

    static e_pfNodeDir fs_pfOffsetToDir(int p_offsetX, int p_offsetY);

    static e_pfNodeDir fs_pfReverseDir(e_pfNodeDir p_dir);

    static bool
    fs_pfSpreadNode(int p_fromX, int p_fromY, e_pfNodeDir p_dir, int p_creatorHealth, int p_goalX, int p_goalY,
    t_pfNodesArr &p_pfNodes, t_pfNodePositions &p_newPfNodePositions);

    void f_pfProcessNodesIntoPath(int p_goalX, int p_goalY, t_pfNodesArr &p_pfNodes);

    bool f_pfBuildPathTo(int p_goalX, int p_goalY);

    public:

    c_playerCharacter(int p_posX, int p_posY);

    std::pair<int, int> f_getPos() const;

    void f_setPos(int p_posX, int p_posY);

    enum class e_pfMoveResult {ev_continue, ev_reachedGoal, ev_cannotReachGoal};

    e_pfMoveResult f_pfMoveTowardsGoal(int p_goalX, int p_goalY);
};

}