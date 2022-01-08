#pragma once
#include <iostream>
#include <sys/socket.h>
#include <cstring>
#include "Data.h"
#include "Hash.h"



class PomocnaTrieda {
public:


    static int prijmiSpravu(char* sprava, int* socket, int velkost = 4096) {
        memset(sprava, 0, velkost);

        ssize_t bytesRecv = recv(*socket, sprava, velkost, 0);
        if(bytesRecv == -1) {
            std::cerr << "Problem so spojenim";
            return 1;
        }
        if(bytesRecv == 0) {
            std::cout << "Klient sa odpojil" << "\n";
            return 1;
        }
        Hash::odsifrujSpravu(sprava, bytesRecv);
        return 0;

    }

    static void odosliSpravu(std::string sprava, SocketAMutex* socketAMutex) {
        Hash::zasifrujSpravu(sprava);

        pthread_mutex_lock(socketAMutex->getMutex());
        send(*socketAMutex->getSocket(), sprava.c_str(), sprava.size(), 0);
        pthread_mutex_unlock(socketAMutex->getMutex());
    }





//klient buffer

//

};

