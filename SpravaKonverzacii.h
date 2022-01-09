#pragma once
#include "Data.h"
#include "Pomocna.h"
#include "Konstanty.h"

#include<fstream>
#include <sstream>



class SpravaKonverzacii {
    
public:
    static bool vyberKonverzaciu(Pouzivatel* pouzivatel, Data* data, SocketAMutex* socketAMutex);

    static bool vytvorKonverzaciu(Pouzivatel* pouzivatel, Data* data, SocketAMutex* socketAMutex);

    static bool posliSubor(Pouzivatel* pouzivatel, Data* data, SocketAMutex* socketAMutex, int index);
};


