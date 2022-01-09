//
// Created by Andrej on 4. 1. 2022.
//

#include "Pomocna.h"

int PomocnaTrieda::prijmiSpravu(char* sprava, int* socket, int velkost) {
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

void PomocnaTrieda::odosliSpravu(std::string sprava, SocketAMutex* socketAMutex) {
    Hash::zasifrujSpravu(sprava);

    pthread_mutex_lock(socketAMutex->getMutex());
    send(*socketAMutex->getSocket(), sprava.c_str(), sprava.size(), 0);
    pthread_mutex_unlock(socketAMutex->getMutex());
}