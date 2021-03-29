#ifndef __MAIN_H__
#define __MAIN_H__

#include <windows.h>

#include <bits/stdc++.h>

#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __declspec(dllimport)
#endif


#ifdef __cplusplus
extern "C"
{
#endif


std::pair<int, int> check_velocity(int agent_x, int agent_y, int radius, int agent_vx, int agent_vy);
std::pair<int, int> check_ball(int agent_x, int agent_y, int action_x, int action_y, int radius, int agent_vx, int agent_vy);
std::pair<int, int> normalize_action(float action_x, float action_y);
std::pair<int, int> avoid_bigger_ball(const int* xCoordinate, const int* yCoordinate, const int* circleRadius, int agent_vx, int agent_vy, int action_x, int action_y);
bool check_if_eat_next_step(int action_x, int action_y, int agent_x, int agent_y, int agent_vx, int agent_vy, int agent_radius, int target_x, int target_y, int target_radius);
int direction(int x_target, int y_target);
int calculate_distance(int x_self, int y_self, int x_other, int y_other);
bool check_over_speed(int target, int target_radius, int agent, int agent_radius, int agent_v);
int continual_eating(const int* xCoordinate, const int* yCoordinate, const int* circleRadius);
bool resource_exist(const int* radius);

void controller(int &action, const size_t agent, const size_t num_agents, const size_t num_resources, const int* circleRadius, const int* xCoordinate, const int* yCoordinate, const int* xVelocity, const int* yVelocity);

#ifdef __cplusplus
}
#endif

#endif // __MAIN_H__
