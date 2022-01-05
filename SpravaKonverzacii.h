#pragma once
#include "Data.h"
#include "Pomocna.h"




class SpravaKonverzacii {
public:


    static bool vyberKonverzaciu(Pouzivatel* pouzivatel, Data* data, int* socket) {
        bool koniec = false;
        while(!koniec) {
            std::string stringSprava = "Vyberte si konverzaciu: \n";
            stringSprava+= "[-2] Vytvorit novu konverzaciu\n";
            stringSprava+= "[-1] Krok spat\n";
            stringSprava+= data->dajZoznamKonverzaciiPouzivatela(pouzivatel);
            send(*socket, stringSprava.c_str(), stringSprava.size(), 0);


            char sprava[4096];
            if(PomocnaTrieda::prijmiSpravu(sprava, socket) == 1) {
                return true;
            }
            int index = atoi(sprava) ;

            if(index == -1) {
                return false;
            }
            if(index == -2) {
                vytvorKonverzaciu(pouzivatel, data, socket);
                continue;
            }

            stringSprava = "Zadajte obsah spravy: \n";
            send(*socket, stringSprava.c_str(), stringSprava.size(), 0);

            if(PomocnaTrieda::prijmiSpravu(sprava, socket) == 1) {
                return true;
            }
            std::string obsahSpravy = sprava;

            std::string tymtoSaNepodariloOdoslat = "";

            if(data->posliSpravu(pouzivatel, index, obsahSpravy, tymtoSaNepodariloOdoslat)) {
                if(tymtoSaNepodariloOdoslat != "") {
                    stringSprava = "Tymto ucastnikom sa nepodarilo spravu odoslat, lebo ich nemate v priateloch:\n";
                    stringSprava+= tymtoSaNepodariloOdoslat; + "\n";
                    stringSprava+= "Ostatnym ucastnikom konverzacie sa sprava poslala\n";
                    send(*socket, stringSprava.c_str(), stringSprava.size(), 0);
                } else {
                    stringSprava = "Sprava odoslana\n";
                    send(*socket, stringSprava.c_str(), stringSprava.size(), 0);
                }

            } else {
                stringSprava = "Zadali ste zly index\n";
                send(*socket, stringSprava.c_str(), stringSprava.size(), 0);
            }
        }
        return false;

        //FINISH nasej semky:
        // HOTOVO odhlasenie
        // HOTOVO zrusenie uctu - vsetkym zrusi priatelstvo
            // HHOTOVO vymaze pouzivatela

       // mutexy
       // HOTOVO - beh servera - aby sa dal ukoncit korektne a
       // pamat aby bola cista po vypnuti

        //ukladanie do suboru

    }

    static bool vytvorKonverzaciu(Pouzivatel* pouzivatel, Data* data, int* socket) {
        std::string stringSprava = "Zadajte nazov konverzacie: \n";
        send(*socket, stringSprava.c_str(), stringSprava.size(), 0);

        char sprava[4096];
        if(PomocnaTrieda::prijmiSpravu(sprava, socket) == 1) {
            return true;
        }
        int indexNasejKonverzacie = -1;
        std::string nazovKonverzacie = sprava;
        data->vytvorKonverzaciu(pouzivatel, indexNasejKonverzacie, nazovKonverzacie);

        bool koniec = false;
        while(!koniec) {

            stringSprava = "Vyber priatelov, ktorych chces pridat do konverzacie: \n";
            stringSprava+= "[-1] Krok spat\n";
            stringSprava+= data->dajZoznamPriatelov(pouzivatel);
            send(*socket, stringSprava.c_str(), stringSprava.size(), 0);


            if(PomocnaTrieda::prijmiSpravu(sprava, socket) == 1) {
                return true;
            }
            int index = atoi(sprava) ;

            if(index == -1) {
                return false;
            }


            if(data->pridajDoKonverzacie(pouzivatel, indexNasejKonverzacie, index)) {
                stringSprava = "Priatel bol pridany do konverzacie\n";
                send(*socket, stringSprava.c_str(), stringSprava.size(), 0);
                koniec = true;
            } else {
                stringSprava = "Zadali ste zly index alebo priatel uz je v konverzacii\n";
                send(*socket, stringSprava.c_str(), stringSprava.size(), 0);
            }
        }
        return false;
    }


};


