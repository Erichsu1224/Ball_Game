g++.exe -Wall -DBUILD_DLL -g  -c C:\\Users\\MSCL\\Documents\\Ball_Game\\agent_dll\\agent_dll.cpp -o obj\\Debug\\agent_dll.o
g++.exe -shared -Wl,--output-def=bin\\Debug\\libagent_dll.def -Wl,--out-implib=bin\\Debug\\libagent_dll.a -Wl,--dll  obj\\Debug\\agent_dll.o  -o bin\\Debug\\agent_dll.dll  -luser32

cp ./bin/Debug/agent_dll.dll ./../2D_simple_ball_game
	

read -n 1 -p "Press any key to continue..."------------------------
