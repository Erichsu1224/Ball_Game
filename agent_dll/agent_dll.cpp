#include "agent_dll.h"
#include<bits/stdc++.h>

using namespace std;
const size_t NUM_ACTIONS = 9;
enum actions { NOOP = 0, UP, DOWN, LEFT, RIGHT, UP_LEFT, UP_RIGHT, DOWN_LEFT, DOWN_RIGHT };

#define MAX_X 1598
#define MAX_Y  898
#define RADIUS_THRESHOLD 50
#define AVOID_DIS 200

#define max_velcovity 2000

#ifdef __cplusplus
extern "C"
{
#endif

    pair<int, int> check_velocity(int agent_x, int agent_y, int radius, int agent_vx, int agent_vy);
    pair<int, int> check_ball(int agent_x, int agent_y, int action_x, int action_y, int radius, int agent_vx, int agent_vy);
    pair<int, int> normalize_action(float action_x, float action_y);
    pair<int, int> avoid_bigger_ball(const int* xCoordinate, const int* yCoordinate, const int* circleRadius, int agent_vx, int agent_vy, int action_x, int action_y);
    bool check_if_eat_next_step(int action_x, int action_y, int agent_x, int agent_y, int agent_vx, int agent_vy, int agent_radius, int target_x, int target_y, int target_radius);
    int direction(int x_target, int y_target);
    int calculate_distance(int x_self, int y_self, int x_other, int y_other);


    int direction(int x_target, int y_target)
    {
        int action;
        if (x_target == 0 && y_target > 0) //the target ball is above
            action = DOWN;
        else if (x_target == 0 && y_target < 0) //the target ball is under
            action = UP;
        else if (x_target > 0 && y_target == 0) //the target ball is on the right
            action = RIGHT;
        else if (x_target < 0 && y_target == 0) //the target ball is on the left
            action = LEFT;
        else if (x_target > 0 && y_target > 0) //the target ball is at the bottom right
            action = DOWN_RIGHT;
        else if (x_target > 0 && y_target < 0) //the Ltarget ball is at the top right
            action = UP_RIGHT;
        else if (x_target < 0 && y_target > 0) //the target ball is at the bottom left
            action = DOWN_LEFT;
        else if (x_target < 0 && y_target < 0) //the target ball is at the up right
            action = UP_LEFT;

        return action;
    }

    pair<int, int> check_velocity(int agent_x, int agent_y, int radius, int agent_vx, int agent_vy) {
        int action_x = 0, action_y = 0;

        //check x
        if (agent_vx != 0) {
            if (agent_vx > 0) {
                if ((MAX_X - radius - agent_x) / agent_vx < agent_vx) {
                    action_x = -1;
                }
            }
            else if (agent_vx < 0) {
                if ((agent_x - radius - 1) / (-agent_vx) < (-agent_vx)) {
                    action_x = 1;
                }
            }
        }

        //check y
        if (agent_vy != 0) {
            if (agent_vy > 0) {
                if ((MAX_Y - agent_y - radius) / agent_vy < agent_vy) {
                    action_y = -1;
                }
            }
            else if (agent_vy < 0) {
                if ((agent_y - radius - 1) / (-agent_vy) < (-agent_vy)) {
                    action_y = 1;
                }
            }
        }

        return make_pair(action_x, action_y);
    }

    pair<int, int> check_ball(int agent_x, int agent_y, int target_x, int target_y, int radius, int agent_vx, int agent_vy) {
        int action_x = target_x - agent_x, action_y = target_y - agent_y;

        //check x
        if (agent_vx != 0) {
            if ((action_x * agent_vx) > 0) { //¦P¦V
                if ((target_x - agent_x) / agent_vx <= abs(agent_vx)) {
                    action_x *= -1;
                }
            }
        }

        //check y
        if (agent_vy != 0) {
            if ((action_y * agent_vy) > 0) {
                if ((target_y - agent_y) / agent_vy <= abs(agent_vy)) {
                    action_y *= -1;
                }
            }
        }

        return make_pair(action_x, action_y);
    }

    bool check_if_eat_next_step(int action_x, int action_y, int agent_x, int agent_y, int agent_vx, int agent_vy, int agent_radius, int target_x, int target_y, int target_radius) {
        /*
        Check if the ball will hit the wall, because of eating other balls.
        */

        // Normalize
        agent_vx += action_x;
        agent_vy += action_y;

        agent_x += agent_vx;
        agent_y += agent_vy;

        if (calculate_distance(agent_x, agent_y, target_x, target_y) <= (agent_radius + target_radius))
            return true;

        else
            return false;
    }

    pair<int, int> avoid_bigger_ball(const int* xCoordinate, const int* yCoordinate, const int* circleRadius, int agent_vx, int agent_vy, int action_x, int action_y) {

        int agent_x = xCoordinate[0];
        int agent_y = yCoordinate[0];

        agent_vx += action_x;
        agent_vy += action_y;

        agent_x += agent_vx;
        agent_y += agent_vy;

        int tmp_x, tmp_y;
        action_x = 0, action_y = 0;

        for (int i = 1; i < 10; i++) {
            if (calculate_distance(agent_x, agent_y, xCoordinate[i], yCoordinate[i]) <= (circleRadius[0] + circleRadius[i])) {
                tie(tmp_x, tmp_y) = normalize_action(-(xCoordinate[i] - agent_x), -(yCoordinate[i] - agent_y));
                action_x += tmp_x;
                action_y += tmp_y;
            }
        }

        return normalize_action(action_x, action_y);
    }

    pair<int, int> normalize_action(float action_x, float action_y) {
        action_x = (action_x != 0) ? ((action_x > 0) ? 1 : -1) : 0;
        action_y = (action_y != 0) ? ((action_y > 0) ? 1 : -1) : 0;

        return make_pair((int)action_x, (int)action_y);
    }


    int calculate_distance(int x_self, int y_self, int x_other, int y_other) {
        int x_distance = (x_other - x_self) * (x_other - x_self);
        int y_distance = (y_other - y_self) * (y_other - y_self);
        int two_distance = sqrt(x_distance + y_distance);
        return two_distance;
    }

    bool resource_exist(const int* radius) {
        for (int i = 10; i < 15; i++) {
            if (radius[i] > 0)
                return true;
        }

        return false;
    }

    __declspec(dllexport) void controller(int& action, const size_t agent, const size_t num_agents, const size_t num_resources, const int* circleRadius,
        const int* xCoordinate, const int* yCoordinate, const int* xVelocity, const int* yVelocity) // the coordinates of  balls and resource centers are in turn placed in the array xCoordinate, and yCoordinate
    {
        freopen("log.log", "a+", stdout);
        int selfx = xCoordinate[0]; //ball 1's xCoordinate
        int selfy = yCoordinate[0]; //ball 1's yCoordinate
        int location = 10; //first black ball's location

        int maximum_blackball = 1;
        int min_distance = 1e9;

        for (size_t i = ((circleRadius[0] < RADIUS_THRESHOLD && resource_exist(circleRadius)) ? 10 : 1); i < 15; i++) {
            int two_distance = calculate_distance(xCoordinate[0], yCoordinate[0], xCoordinate[i], yCoordinate[i]);

            if (circleRadius[0] < RADIUS_THRESHOLD) {
                if (circleRadius[i] > 0 && circleRadius[i] < circleRadius[0] && two_distance <= min_distance) {
                    if (two_distance == min_distance) {
                        maximum_blackball = max(maximum_blackball, circleRadius[i]);
                        min_distance = min_distance;
                        location = ((maximum_blackball >= circleRadius[i]) ? location : i);
                    }
                    else {
                        maximum_blackball = circleRadius[i];
                        min_distance = two_distance;
                        location = i;
                    }
                }
            }

            else {
                if (circleRadius[i] > 0 && circleRadius[i] < circleRadius[0] && maximum_blackball <= circleRadius[i]) {
                    if (maximum_blackball == circleRadius[i]) {
                        min_distance = min(min_distance, two_distance);
                        location = ((min_distance <= two_distance) ? location : i);
                    }
                    else {
                        maximum_blackball = circleRadius[i];
                        min_distance = two_distance;
                        location = i;
                    }
                }
            }
        }

        /*
        Priority
        1. avoid_hit_x, avoid_hit_y
        2. avoid_ball_x, avoid_ball_y
        3. target_x, target_y
        */
        int target_x = 0, target_y = 0;
        tie(target_x, target_y) = check_ball(selfx, selfy, xCoordinate[location], yCoordinate[location], circleRadius[0], xVelocity[0], yVelocity[0]);
        tie(target_x, target_y) = normalize_action(target_x, target_y);


        bool eat_flag = check_if_eat_next_step(target_x, target_y, selfx, selfy, xVelocity[0], yVelocity[0], circleRadius[0], xCoordinate[location], yCoordinate[location], circleRadius[location]);

        int avoid_ball_x = 0, avoid_ball_y = 0;
        tie(avoid_ball_x, avoid_ball_y) = avoid_bigger_ball(xCoordinate, yCoordinate, circleRadius, xVelocity[0], yVelocity[0], target_x, target_y);


        int avoid_hit_x = 0, avoid_hit_y = 0;
        tie(avoid_hit_x, avoid_hit_y) = check_velocity(selfx, selfy, circleRadius[0] + eat_flag, xVelocity[0] + target_x + avoid_ball_x, yVelocity[0] + target_y + avoid_ball_y);

        target_x = avoid_hit_x * 3 + avoid_ball_x + target_x;
        target_y = avoid_hit_y * 3 + avoid_ball_y + target_y;


        //decide the direction
        action = direction(target_x, target_y);

        return;
    }

#ifdef __cplusplus
}
#endif

extern "C" DLL_EXPORT BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        // attach to process
        // return FALSE to fail DLL load
        break;

    case DLL_PROCESS_DETACH:
        // detach from process
        break;

    case DLL_THREAD_ATTACH:
        // attach to thread
        break;

    case DLL_THREAD_DETACH:
        // detach from thread
        break;
    }
    return TRUE; // succesful
}
