//
// Created by Andrej on 4. 1. 2022.
//


#include "Moznosti.h"

bool Moznosti::vyberSiMoznost(Pouzivatel* pouzivatel, Data* data, SocketAMutex* socketAMutex) {

    bool koniec = false;
    while(!koniec) {
        std::string stringSprava = "Vyberte si, co chcete robit: \n";
        stringSprava+="[1]Pridat pouzivatela do priatelov\n";
        stringSprava+="[2]Odstranit pouzivatela z priatelov\n";
        stringSprava+="[3]Pozriet si ziadosti o priatelstvo\n";
        stringSprava+="[4]Poslat spravu alebo subor\n";
        stringSprava+="[5]Odhlasit sa\n";
        stringSprava+="[6]Zrusit ucet\n";

        char sprava[4096];
        PomocnaTrieda::odosliSpravu(stringSprava, socketAMutex);

        if(PomocnaTrieda::prijmiSpravu(sprava, socketAMutex->getSocket()) == 1) {
            return true;
        }

        if(sprava[0] == '1') {
            //Pridat pouzivatela do priatelov
            if(SpravaPriatelstiev::posliZiadostOPriatelstvo(pouzivatel,data, socketAMutex)) {

                return true;
            }
        } else if(sprava[0] == '2') {
            if(SpravaPriatelstiev::odoberZPriatelov(pouzivatel, data, socketAMutex)) {
                return true;
            }
        } else if(sprava[0] == '3') {
            if(SpravaPriatelstiev::potvrdenieAleboZamietnutieZiadosti(pouzivatel, data, socketAMutex)) {
                return true;
            }
        } else if(sprava[0] == '4')  {
            //Poslat spravu
            if(SpravaKonverzacii::vyberKonverzaciu(pouzivatel, data, socketAMutex)) {
                return true;
            }

        } else if(sprava[0] == '5') {
            //odhlasenie


            stringSprava = "Boli ste uspesne odhlaseny\n";
            PomocnaTrieda::odosliSpravu(stringSprava, socketAMutex);
            return false;

        } else if(sprava[0] == '6') {
            //zrusenie uctu
            stringSprava = "Naozaj chcete zrusit svoj ucet?\n";
            stringSprava = "[1] ano\n";
            PomocnaTrieda::odosliSpravu(stringSprava, socketAMutex);


            if(PomocnaTrieda::prijmiSpravu(sprava, socketAMutex->getSocket()) == 1) {
                return true;
            }
            if(sprava[0] == '1') {
                data->odstranPouzivateloviVsetkychPriatelov(pouzivatel);
                stringSprava = "Ucet bol uspesne odstraneny\n";
                PomocnaTrieda::odosliSpravu(stringSprava, socketAMutex);

                data->odhlasPouzivatela(pouzivatel);
                data->odstranUcet(pouzivatel);
                return false;
            }
        } else {
            stringSprava = "Zadali ste zlu moznost, prosim zopakujte: \n";
            PomocnaTrieda::odosliSpravu(stringSprava, socketAMutex);
        }


    }
    return false;
}
