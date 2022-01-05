#pragma once
#include "Data.h"
#include "Pomocna.h"
#include "SpravaPriatelstiev.h"
#include "SpravaKonverzacii.h"

class Moznosti {
public:
    static bool vyberSiMoznost(Pouzivatel* pouzivatel, Data* data, int* socket) {

        bool koniec = false;
        while(!koniec) {
            std::string stringSprava = "Vyberte si, co chcete robit: \n";
            stringSprava+="[1]Pridat pouzivatela do priatelov\n";
            stringSprava+="[2]Odstranit pouzivatela z priatelov\n";
            stringSprava+="[3]Pozriet si ziadosti o pritelstvo\n";
            stringSprava+="[4]Poslat spravu\n";
            stringSprava+="[5]Odhlasit sa\n";

            char sprava[4096];
            send(*socket, stringSprava.c_str(), stringSprava.size(), 0);

            if(PomocnaTrieda::prijmiSpravu(sprava, socket) == 1) {
                return true;
            }

            if(sprava[0] == '1') {
                //Pridat pouzivatela do priatelov
                if(SpravaPriatelstiev::posliZiadostOPriatelstvo(pouzivatel,data, socket)) {
                    return true;
                }
            } else if(sprava[0] == '2') {
                if(SpravaPriatelstiev::odoberZPriatelov(pouzivatel, data, socket)) {
                    return true;
                }
            } else if(sprava[0] == '3') {
                if(SpravaPriatelstiev::potvrdenieAleboZamietnutieZiadosti(pouzivatel, data, socket)) {
                    return true;
                }
            } else if(sprava[0] == '4')  {
                //Poslat spravu
                if(SpravaKonverzacii::vyberKonverzaciu(pouzivatel, data, socket)) {
                    return true;
                }

            } else if(sprava[0] == '5') {
                //odhlasenie

            } else {
                    stringSprava = "Zadali ste zlu moznost, prosim zopakujte: \n";
                    send(*socket, stringSprava.c_str(), stringSprava.size(), 0);
            }

        }
        return false;
    }
};


