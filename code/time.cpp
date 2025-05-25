/***********************************************************************************************************************
 * @file
 * @brief For time-related things.
 **********************************************************************************************************************/

#include "time.hpp"

#include <cmath>
#include <cstdint>
#include <stdexcept>

#include <SDL.h>

using namespace std;




namespace n_tdg::n_time
{
    
    namespace
    {
        
        float g_realDTime     {};    //!< The real delta time.
        float g_scaledDTime   {};    //!< The scaled delta time.
        float g_dTimeScale    {1.f}; //!< The delta-time scale.
        
        float g_tickRateCap {-1.f}; //!< The tick-rate cap. -1 means that the cap is disabled.
        
    }
    
    
    
    
    void fg_handleTimingOfMainTick()
    {
        static uint64_t ls_prevTime {SDL_GetTicks64()};
        g_realDTime = static_cast<float>(SDL_GetTicks64() - ls_prevTime) * .001f;
        
        if (g_tickRateCap != -1.f)
        {
            // If the current tick rate is higher than the cap.
            if (g_realDTime < 1.f / g_tickRateCap)
                // While waiting like this may lead to slightly incorrect timing, it reduces CPU usage.
                SDL_Delay((1.f / g_tickRateCap - g_realDTime) * 1000.f);
        }
        
        g_realDTime = static_cast<float>(SDL_GetTicks64() - ls_prevTime) * .001f;
        g_scaledDTime = g_realDTime * g_dTimeScale;
        ls_prevTime = SDL_GetTicks64();
    }
    
    float fg_getRealDTime()
    {
        return g_realDTime;
    }
    
    float fg_getDTime()
    {
        return g_scaledDTime;
    }
    
    float fg_getDTimeScale()
    {
        return g_dTimeScale;
    }
    
    void  fg_setDTimeScale(float p_val)
    {
        g_dTimeScale = p_val;
    }
    
    float fg_getTickRateCap()
    {
        return g_tickRateCap;
    }
    
    void fg_setTickRateCap(float p_val)
    {
        if (p_val <= 0.f && p_val != -1.f)
            throw invalid_argument {"Failed to set the tick-rate cap; the given value is invalid."};
        
        g_tickRateCap = p_val;
    }
    
}