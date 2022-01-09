#pragma once
#include "Data.h"
#include "Pomocna.h"

class SpravaPriatelstiev {

public:
    static bool posliZiadostOPriatelstvo(Pouzivatel* pouzivatel, Data* data, SocketAMutex* socketAMutex);

    static bool odoberZPriatelov(Pouzivatel* pouzivatel, Data* data, SocketAMutex* socketAMutex);

    static bool potvrdenieAleboZamietnutieZiadosti(Pouzivatel* pouzivatel, Data* data, SocketAMutex* socketAMutex);

};

