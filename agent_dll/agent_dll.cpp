#include "agent_dll.h"
#include<bits/stdc++.h>
using namespace std;
const size_t NUM_ACTIONS = 9;
enum actions { NOOP = 0, UP, DOWN, LEFT, RIGHT, UP_LEFT, UP_RIGHT, DOWN_LEFT, DOWN_RIGHT };

#define max_x 1599
#define max_y  899

#define max_velcovity 2000

#ifdef __cplusplus
extern "C"
{
#endif

int direction(int x_target, int y_target)
{
    int action;
    if(x_target == 0 && y_target > 0) //the target ball is above
        action = DOWN;
    else if(x_target == 0 && y_target < 0) //the target ball is under
        action = UP;
    else if(x_target > 0 && y_target == 0) //the target ball is on the right
        action = RIGHT;
    else if(x_target < 0 && y_target == 0) //the target ball is on the left
        action = LEFT;
    else if(x_target > 0 && y_target > 0) //the target ball is at the bottom right
    	action = DOWN_RIGHT;
    else if(x_target > 0 && y_target < 0) //the Ltarget ball is at the top right
    	action = UP_RIGHT;
    else if(x_target < 0 && y_target > 0) //the target ball is at the bottom left
    	action = DOWN_LEFT;
    else if(x_target < 0 && y_target < 0) //the target ball is at the up right
    	action = UP_LEFT;

    return action;
}

pair<int, int> check_velocity(int agent_x, int agent_y, int radius, int agent_vx, int agent_vy){
    int action_x = 0, action_y = 0;

    //check x
    if(agent_vx != 0){
        if(agent_vx > 0){
            if((max_x-radius-agent_x) / agent_vx <= agent_vx){
                action_x = -max_velcovity;
            }
        }
        else if(agent_vx < 0){
            if((agent_x-radius) / (-agent_vx) <= (-agent_vx)){
                action_x = max_velcovity;
            }
        }
    }

    //check y
    if(agent_vy != 0){
        if(agent_vy > 0){
            if((max_y-agent_y-radius) / agent_vy <= agent_vy){
                action_y = -max_velcovity;
            }
        }
        else if(agent_vy < 0){
            if((agent_y-radius) / (-agent_vy) <= (-agent_vy)){
                action_y = max_velcovity;
            }
        }
    }

    return make_pair(action_x, action_y);
}

pair<int, int> check_if_eat_next_step(int action_x, int action_y, int agent_x, int agent_y, int agent_vx, int agent_vy, int radius, int target_x, int target_y){
    '''
    Check if the ball will hit the wall, because of eating other balls.
    '''

    // Normalize
    action_x = (action_x != 0) ? ((action_x > 0) ? 1 : -1) : 0;
    action_y = (action_y != 0) ? ((action_y > 0) ? 1 : -1) : 0;

    agent_vx += action_x;
    agent_vy += action_y;

    int a_x = 0, a_y = 0;

    if(target_x-agent_x-radius <= agent_vx && target_y-agent_y-radius <= agent_vy){
        // check x
        if(agent_x+agent_vx-radius <= 0){
            a_x = 1;
        }
        else if(agent_x+agent_vx+radius >= max_x){
            a_x = -1;
        }

        // check x
        if(agent_y+agent_vy-radius <= 0){
            a_y = 1;
        }
        else if(agent_y+agent_vy+radius >= max_y){
            a_y = -1;
        }
    }

    return make_pair(action_x+a_x, action_y+a_y);
}

__declspec(dllexport) void controller(int &action, const size_t agent, const size_t num_agents, const size_t num_resources, const int* circleRadius,
                                      const int* xCoordinate, const int* yCoordinate, const int* xVelocity, const int* yVelocity) // the coordinates of  balls and resource centers are in turn placed in the array xCoordinate, and yCoordinate
{
    int selfx = xCoordinate[0]; //ball 1's xCoordinate
    int selfy = yCoordinate[0]; //ball 1's yCoordinate
    int location = 10; //first black ball's location
    bool flag = false;
    int maximum_blackball = 5; //assume the biggest black ball that ball 1 need to catch
    int min_distance = 800;
    // find the biggest black ball and the closest, return its location
    for(size_t i = 10; i < 15; i++){
        int x_distance = (xCoordinate[i] - xCoordinate[0]) * (xCoordinate[i] - xCoordinate[0]);
        int y_distance = (yCoordinate[i] - yCoordinate[0]) * (yCoordinate[i] - yCoordinate[0]);
        int two_distance = sqrt(x_distance + y_distance);
        if(circleRadius[i] > 0 && circleRadius[i] >= maximum_blackball && two_distance < min_distance ){
            maximum_blackball = circleRadius[i];
            min_distance = two_distance;
            location = i;
            flag = true;
        }
    }
    int maximum_ball = 0;
    int mini_distance = 1600;
    // if the black ball's radius are all smaller than 5, and the target turn to the ball which is smaller than ball 1 and the biggest in ball 2 to ball 10;
    if(!flag){
        for(size_t i = 10; i < 15; i++){
            if(circleRadius[i] < 5){

                int x_distance = (xCoordinate[i] - xCoordinate[0]) * (xCoordinate[i] - xCoordinate[0]);
                int y_distance = (yCoordinate[i] - yCoordinate[0]) * (yCoordinate[i] - yCoordinate[0]);
                int two_distance = sqrt(x_distance + y_distance);
                for(size_t t = 1; t < 10; t++){
                    if(circleRadius[t] < circleRadius[0] && circleRadius[t] > 0 && circleRadius[t] > maximum_ball && two_distance < mini_distance ){
                        maximum_ball = circleRadius[t];
                        min_distance = two_distance;
                        location = t;
                        break;
                    }
                }
            }
        }
    }

    // find the target in the which side of ball 1
    int x_target = xCoordinate[location] - selfx;
    int y_target = yCoordinate[location] - selfy;


    // check direction and check if the v is too fast
    int a_x, a_y;
    tie(a_x, a_y) = check_velocity(selfx, selfy, circleRadius[0], xVelocity[0], yVelocity[0]);

    tie(x_target, y_target) = check_if_eat_next_step(x_target+a_x, y_target+a_y, selfx, selfy, xVelocity[0], yVelocity[0], circleRadius[0], xCoordinate[location], yCoordinate[location]);

    // decide the direction
    action = direction(x_target, y_target);

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
