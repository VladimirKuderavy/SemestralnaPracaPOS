#pragma once
#include "Data.h"
#include "Pomocna.h"
#include "SpravaPriatelstiev.h"
#include "SpravaKonverzacii.h"

class Moznosti {
public:
    static bool vyberSiMoznost(Pouzivatel* pouzivatel, Data* data, SocketAMutex* socketAMutex);
};



