#pragma once
#include <iostream>
#include <sys/socket.h>
#include <cstring>
#include "Data.h"
#include "Hash.h"



class PomocnaTrieda {
public:
    static int prijmiSpravu(char* sprava, int* socket, int velkost = 4096);

    static void odosliSpravu(std::string sprava, SocketAMutex* socketAMutex);
};

