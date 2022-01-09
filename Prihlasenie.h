#pragma once
//
// Created by Andrej on 3. 1. 2022.
//
#include <sys/socket.h>
#include "Data.h"
#include <iostream>
#include <string>
#include <cstring>
#include "Pomocna.h"


class Prihlasenie {
public:

    static Pouzivatel* prihlasenie(Data* data, SocketAMutex* socketAMutex);

private:
    static Pouzivatel* prihlasSa(Data* data, SocketAMutex* socketAMutex);

    static bool registrujSa(Data* data, SocketAMutex* socketAMutex);
};
