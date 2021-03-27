#include "agent_dll.h"
#include<bits/stdc++.h>

using namespace std;
const size_t NUM_ACTIONS = 9;
enum actions { NOOP = 0, UP, DOWN, LEFT, RIGHT, UP_LEFT, UP_RIGHT, DOWN_LEFT, DOWN_RIGHT };


/*
`Hyper-parameters for the agent
*/
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
int check_over_speed(int target, int target_radius, int agent, int agent_radius, int agent_v);



/*
    * This function convert accelerations to action
    * param:
    *   x_target (int): acceleration in x coordinate.
    *   y_target (int): acceleration in y coordinate.
    * return:
    *   action (int): the action of the current state
*/
int direction(int x_target, int y_target){
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


/*
    * This function check if the agent will stop at the target location.
    * It may trying to avoid the agent hitting the wall and over spped 
    * when they trying to eat other agnets or resources
    * param:
    *   target (int):           target location 
    *   target_radius (int):    radius of target
    *   agent (int):            agent location 
    *   agent_radius (int) :    radius of agent radius
    *   agent_v (int) :         velocity of the agent
    * return:
    *   (int): return if the acceleration should go opisite side or not.
*/
int check_over_speed(int target, int target_radius, int agent, int agent_radius, int agent_v){
    if ((abs(target-agent)-target_radius-agent_radius) / abs(agent_v) < abs(agent_v) )
        return ((-agent_v < 0) ? -1 : 1);
    else
        return ((agent_v < 0) ? -1 : 1);
}


/*
    * This function check if the agent will hit the wall.
    * As the premise that the ball do the action what we dicide to.
    * param:
    *   agent_x (int):      location of the agent in x coordinate
    *   agnet_y (int):      location of the agent in y coordinate
    *   radius (int) :      radius of agent
    *   agent_vx (int) :    velocity of the agent in x coordinate
    *   agent_vy (int) :    velocity of the agent in y coordinate
    * return:
    *   (pair<int, int>): return the acceleration if the agent need to get away from the wall.
*/
pair<int, int> check_velocity(int agent_x, int agent_y, int radius, int agent_vx, int agent_vy) {
    int action_x = 0, action_y = 0;

    //check x
    if (agent_vx != 0) {
        if (agent_vx > 0)
            action_x = check_over_speed(MAX_X, 1, agent_x, radius, agent_vx);
        else if (agent_vx < 0)
            action_x = check_over_speed(0, 1, agent_x, radius, agent_vx);
    }

    //check y
    if (agent_vy != 0) {
        if (agent_vy > 0) 
            action_y = check_over_speed(MAX_Y, 1, agent_y, radius, agent_vy);
        else if (agent_vy < 0)
            action_y = check_over_speed(0, 1, agent_y, radius, agent_vy);
    }

    return make_pair(action_x, action_y);
}

/*
    * This function will decide the action for the agent while trying to eat the target ball.
    * param:
    *   agent_x (int):      location of the agent in x coordinate
    *   agnet_y (int):      location of the agent in y coordinate
    *   target_x (int):     location of the target in x coordinate
    *   target_y (int):     location of the target in y coordinate
    *   radius (int) :      radius of agent
    *   agent_vx (int) :    velocity of the agent in x coordinate
    *   agent_vy (int) :    velocity of the agent in y coordinate
    * return:
    *   (pair<int, int>): return the acceleration of the agent.
*/
pair<int, int> action_for_target(int agent_x, int agent_y, int target_x, int target_y, int radius, int agent_vx, int agent_vy) {
    int action_x = target_x-agent_x, action_y = target_y-agent_y;

    //check x
    if (agent_vx != 0) {
        // if the action_x and agent_vx goes with same direction
        if ((action_x * agent_vx) > 0)
            action_x *= check_over_speed(target_x, 0, agent_x, 0, agent_vx);
    }

    //check y
    if (agent_vy != 0) {
        if ((action_y * agent_vy) > 0)
            action_y *= check_over_speed(target_y, 0, agent_y, 0, agent_vy);
    }

    return normalize_action(action_x, action_y);
}

/*
    * This function check if the target ball will be eaten by the agent
    * param:
    *   action_x (int):         acceleration of the action in x coordinate
    *   action_y (int):         acceleration of the action in y coordinate
    *   agent_x (int):          location of the agent in x coordinate
    *   agnet_y (int):          location of the agent in y coordinate
    *   agent_vx (int) :        velocity of the agent in x coordinate
    *   agent_vy (int) :        velocity of the agent in y coordinate
    *   agent_radius (int) :    radius of agent
    *   target_x (int):         location of the target in x coordinate
    *   target_y (int):         location of the target in y coordinate
    *   target_radius (int):    radius of target
    * return:
    *   (bool): if the target ball will be eaten or not
*/
bool check_if_eat_next_step(int action_x, int action_y, int agent_x, int agent_y, int agent_vx, int agent_vy, int agent_radius, int target_x, int target_y, int target_radius) {
    agent_vx += action_x;
    agent_vy += action_y;

    agent_x += agent_vx;
    agent_y += agent_vy;

    if (calculate_distance(agent_x, agent_y, target_x, target_y) <= (agent_radius + target_radius))
        return true;
    else
        return false;
}

/*
    * This function check if the agent will hit other balls.
    * If so, get away from the balls.
    * param:
    *   xCoordinate (const int*):   coordinate of all the agents in x dimension
    *   Cyoordinate (const int*):   coordinate of all the agents in y dimension
    *   curcleRadius (const int*):  radius of all the agents
    *   agent_vx (int) :        velocity of the agent in x coordinate
    *   agent_vy (int) :        velocity of the agent in y coordinate
    *   action_x (int):         acceleration of the action in x coordinate
    *   action_y (int):         acceleration of the action in y coordinate
    * return:
    *   (bool): the acceleration to avoid hitting other balls.
*/
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

/*
    * This function normalize the vector in unit vector.
    * param:
    *   action_x (int):      acceleration of the action in x coordinate
    *   action_y (int):      acceleration of the action in y coordinate
    * return:
    * (pair<int, int>): unit vector
*/
pair<int, int> normalize_action(float action_x, float action_y) {
    action_x = (action_x != 0) ? ((action_x > 0) ? 1 : -1) : 0;
    action_y = (action_y != 0) ? ((action_y > 0) ? 1 : -1) : 0;

    return make_pair((int)action_x, (int)action_y);
}

/*
    * This function calculates the Euclidean Distance of agent and target
    * param:
    *   agent_x (int):      location of the agent in x coordinate
    *   agnet_y (int):      location of the agent in y coordinate
    *   target_x (int):     location of the target in x coordinate
    *   target_y (int):     location of the target in y coordinate
    * return:
    * (int): Euclidean Distance of agent and target
*/
int calculate_distance(int x_self, int y_self, int x_other, int y_other) {
    int x_distance = (x_other - x_self) * (x_other - x_self);
    int y_distance = (y_other - y_self) * (y_other - y_self);
    return sqrt(x_distance + y_distance);
}

/*
    * This function check if any resouces exist
    * param:
    *   radius (const int*): the array of the radius of agents and resources
    * return:
    * (bool): if any resources exist
*/
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
    int selfx = xCoordinate[0]; //ball 1's xCoordinate
    int selfy = yCoordinate[0]; //ball 1's yCoordinate
    int location = 10; //first black ball's location

    int maximum_blackball = 1;
    int min_distance = 1e9;

    for (size_t i = ((circleRadius[0] < RADIUS_THRESHOLD && resource_exist(circleRadius)) ? 10 : 1); i < 15; i++) {
        int two_distance = calculate_distance(xCoordinate[0], yCoordinate[0], xCoordinate[i], yCoordinate[i]);

        // The agent follows "less distance first strategy" when radius less than the threshold.
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
    3. action_x, action_y
    */
    int action_x = 0, action_y = 0;
    tie(action_x, action_y) = action_for_target(selfx, selfy, xCoordinate[location], yCoordinate[location], circleRadius[0], xVelocity[0], yVelocity[0]);

    bool eat_flag = check_if_eat_next_step(action_x, action_y, selfx, selfy, xVelocity[0], yVelocity[0], circleRadius[0], xCoordinate[location], yCoordinate[location], circleRadius[location]);

    int avoid_ball_x = 0, avoid_ball_y = 0;
    tie(avoid_ball_x, avoid_ball_y) = avoid_bigger_ball(xCoordinate, yCoordinate, circleRadius, xVelocity[0], yVelocity[0], action_x, action_y);

    int avoid_hit_x = 0, avoid_hit_y = 0;
    tie(avoid_hit_x, avoid_hit_y) = check_velocity(selfx, selfy, circleRadius[0] + eat_flag, xVelocity[0] + action_x + avoid_ball_x, yVelocity[0] + action_y + avoid_ball_y);

    // Calcalate the priority for the action
    action_x = avoid_hit_x * 3 + avoid_ball_x + action_x;
    action_y = avoid_hit_y * 3 + avoid_ball_y + action_y;

    action = direction(action_x, action_y);

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
