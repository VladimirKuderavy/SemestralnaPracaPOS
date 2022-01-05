#pragma once
#include "Data.h"
#include "Pomocna.h"

class SpravaPriatelstiev {
public:
    //bool dosloKProblemu
    static bool posliZiadostOPriatelstvo(Pouzivatel* pouzivatel, Data* data, int* socket) {
        bool ziadostPoslana = false;
        while(!ziadostPoslana) {
            char sprava[4096];
            std::string stringSprava = "Zadajte username pouzivatela, ktoreho chcete pridat\n";
            send(*socket, stringSprava.c_str(), stringSprava.size(), 0);

            if(PomocnaTrieda::prijmiSpravu(sprava, socket) == 1) {
                return true;
            }
            std::string meno = sprava;

            if(data->posliZiadostOPriatelstvo(pouzivatel, meno)) {
                stringSprava = "Ziadost bola uspesne odoslana\n";
                send(*socket, stringSprava.c_str(), stringSprava.size(), 0);

                ziadostPoslana = true;
            } else {
                stringSprava = "Ziadost sa nepodarilo odoslat\n";
                stringSprava += "Uzivatel, ktoremu sa ju snazite poslat vas uz "
                                "ma v priateloch alebo taky neexistuje\n";
                stringSprava += "ak chcete pokracovat, tak [1]\n";
                send(*socket, stringSprava.c_str(), stringSprava.size(), 0);

                if(PomocnaTrieda::prijmiSpravu(sprava, socket) == 1) {
                    return true;
                }
                if(sprava[0] != '1') {
                    return false;
                }
            }
        }
        return false;
    }

    static bool odoberZPriatelov(Pouzivatel* pouzivatel, Data* data, int* socket) {
        bool koniec = false;
        while(!koniec) {
            std::string stringSprava = "Zoznam priatelov, ktorych mozete odobrat: \n";
            stringSprava+= "[-1] Krok spat\n";
            stringSprava+= data->dajZoznamPriatelov(pouzivatel);
            send(*socket, stringSprava.c_str(), stringSprava.size(), 0);


            char sprava[4096];
            if(PomocnaTrieda::prijmiSpravu(sprava, socket) == 1) {
                return true;
            }
            int index = atoi(sprava) ;

            if(index == -1) {
                return false;
            }





            if(data->spracujOdobratieZPriatelov(pouzivatel, index)) {
                stringSprava+= "Akcia prebehla uspesne\n";
                send(*socket, stringSprava.c_str(), stringSprava.size(), 0);
            } else {
                stringSprava+= "Zadali ste zly index\n";
                send(*socket, stringSprava.c_str(), stringSprava.size(), 0);
            }
        }
        return false;

    }

    static bool potvrdenieAleboZamietnutieZiadosti(Pouzivatel* pouzivatel, Data* data, int* socket) {
        bool koniec = false;
        while(!koniec) {
            std::string stringSprava = "Ziadosti od priatelstvo od tychto pouzivatelov: \n";
            stringSprava+= "[-1] Krok spat\n";
            stringSprava+= data->dajZoznamZiadostiOPriatelstvo(pouzivatel);
            send(*socket, stringSprava.c_str(), stringSprava.size(), 0);


            char sprava[4096];
            if(PomocnaTrieda::prijmiSpravu(sprava, socket) == 1) {
                return true;
            }
            int index = atoi(sprava) ;

            if(index == -1) {
                return false;
            }

            stringSprava = "Vyberte, co sa ma so ziadostou stat: \n";
            stringSprava += "[P] prijat \n";
            stringSprava += "[O] odobrat \n";
            send(*socket, stringSprava.c_str(), stringSprava.size(), 0);

            if(PomocnaTrieda::prijmiSpravu(sprava, socket) == 1) {
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
                send(*socket, stringSprava.c_str(), stringSprava.size(), 0);
            } else {
                stringSprava = "Zadali ste zly index\n";
                send(*socket, stringSprava.c_str(), stringSprava.size(), 0);
            }
        }

        return false;
    }

};

