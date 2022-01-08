#pragma once
#include "Data.h"
#include "Pomocna.h"

class SpravaPriatelstiev {
public:
    //bool dosloKProblemu
    static bool posliZiadostOPriatelstvo(Pouzivatel* pouzivatel, Data* data, SocketAMutex* socketAMutex) {
        bool ziadostPoslana = false;
        while(!ziadostPoslana) {
            char sprava[4096];
            std::string stringSprava = "Zadajte username pouzivatela, ktoreho chcete pridat\n";
            PomocnaTrieda::odosliSpravu(stringSprava, socketAMutex);

            if(PomocnaTrieda::prijmiSpravu(sprava, socketAMutex->getSocket()) == 1) {
                return true;
            }
            std::string meno = sprava;

            if(data->posliZiadostOPriatelstvo(pouzivatel, meno)) {
                stringSprava = "Ziadost bola uspesne odoslana\n";
                PomocnaTrieda::odosliSpravu(stringSprava, socketAMutex);


                ziadostPoslana = true;
            } else {
                stringSprava = "Ziadost sa nepodarilo odoslat\n";
                stringSprava += "Uzivatel, ktoremu sa ju snazite poslat vas uz "
                                "ma v priateloch alebo taky neexistuje\n";
                stringSprava += "ak chcete pokracovat, tak [1]\n";
                PomocnaTrieda::odosliSpravu(stringSprava, socketAMutex);

                if(PomocnaTrieda::prijmiSpravu(sprava, socketAMutex->getSocket()) == 1) {
                    return true;
                }
                if(sprava[0] != '1') {
                    return false;
                }
            }
        }
        return false;
    }

    static bool odoberZPriatelov(Pouzivatel* pouzivatel, Data* data, SocketAMutex* socketAMutex) {
        bool koniec = false;
        while(!koniec) {
            std::string stringSprava = "Zoznam priatelov, ktorych mozete odobrat: \n";
            stringSprava+= "[-1] Krok spat\n";
            stringSprava+= data->dajZoznamPriatelov(pouzivatel);
            PomocnaTrieda::odosliSpravu(stringSprava, socketAMutex);


            char sprava[4096];
            if(PomocnaTrieda::prijmiSpravu(sprava, socketAMutex->getSocket()) == 1) {
                return true;
            }
            int index = atoi(sprava) ;

            if(index == -1) {
                return false;
            }





            if(data->spracujOdobratieZPriatelov(pouzivatel, index)) {
                stringSprava+= "Akcia prebehla uspesne\n";
                PomocnaTrieda::odosliSpravu(stringSprava, socketAMutex);
            } else {
                stringSprava+= "Zadali ste zly index\n";
                PomocnaTrieda::odosliSpravu(stringSprava, socketAMutex);
            }
        }
        return false;

    }

    static bool potvrdenieAleboZamietnutieZiadosti(Pouzivatel* pouzivatel, Data* data, SocketAMutex* socketAMutex) {
        bool koniec = false;
        while(!koniec) {
            std::string stringSprava = "Ziadosti od priatelstvo od tychto pouzivatelov: \n";
            stringSprava+= "[-1] Krok spat\n";
            stringSprava+= data->dajZoznamZiadostiOPriatelstvo(pouzivatel);
            PomocnaTrieda::odosliSpravu(stringSprava, socketAMutex);


            char sprava[4096];
            if(PomocnaTrieda::prijmiSpravu(sprava, socketAMutex->getSocket()) == 1) {
                return true;
            }
            int index = atoi(sprava) ;

            if(index == -1) {
                return false;
            }

            stringSprava = "Vyberte, co sa ma so ziadostou stat: \n";
            stringSprava += "[P] prijat \n";
            stringSprava += "[O] odobrat \n";
            PomocnaTrieda::odosliSpravu(stringSprava, socketAMutex);

            if(PomocnaTrieda::prijmiSpravu(sprava, socketAMutex->getSocket()) == 1) {
                return true;
            }
            bool prijal = false;
            if(sprava[0] == 'P') {
                prijal = true;
            } else {
                prijal = false;
            }
            if(data->spracujZiadostOPriatelstvo(pouzivatel, index, prijal)) {
                stringSprava = "Akcia prebehla uspesne\n";
                PomocnaTrieda::odosliSpravu(stringSprava, socketAMutex);
            } else {
                stringSprava = "Zadali ste zly index\n";
                PomocnaTrieda::odosliSpravu(stringSprava, socketAMutex);
            }
        }

        return false;
    }

};

