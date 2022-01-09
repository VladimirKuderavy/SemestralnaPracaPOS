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



    static Pouzivatel* prihlasenie(Data* data, SocketAMutex* socketAMutex) {

        bool bolUspesnePrihlaseny = false;
        char sprava[4096];
        Pouzivatel* pouzivatel = nullptr;
        while(!bolUspesnePrihlaseny) {


            std::string stringSprava = "Zvoľte si možnosť: \n";
            stringSprava+= "[1] Prihlásiť sa\n";
            stringSprava+= "[2] Zaregistrovať sa\n";
            stringSprava+= "[koniec] Ukoncenie aplikacie\n";
            PomocnaTrieda::odosliSpravu(stringSprava, socketAMutex);


            if(PomocnaTrieda::prijmiSpravu(sprava, socketAMutex->getSocket()) == 1) {
                return nullptr;
            }



            if(sprava[0] == '1') {
                pouzivatel = prihlasSa(data, socketAMutex);
                if(pouzivatel != nullptr) {
                    bolUspesnePrihlaseny = true;
                }
            } else if(sprava[0] == '2') {
                bool dosloKChybe = registrujSa(data, socketAMutex);
                if(dosloKChybe) {
                    return nullptr;
                }
            } else {
                stringSprava = "Zadali ste zlu moznost \n";
                PomocnaTrieda::odosliSpravu(stringSprava, socketAMutex);
            }


        }
        return pouzivatel;
    }
private:
    static Pouzivatel* prihlasSa(Data* data, SocketAMutex* socketAMutex) {
        bool uspesnePrihlaseny = false;
        Pouzivatel* pouzivatel = nullptr;
        while(!uspesnePrihlaseny) {
            std::string stringSprava = "Zadajte pouzivatelske meno: \n";
            PomocnaTrieda::odosliSpravu(stringSprava, socketAMutex);


            char sprava[4096];
            if(PomocnaTrieda::prijmiSpravu(sprava, socketAMutex->getSocket()) == 1) {
                return nullptr;
            }
            std::string meno = sprava;


            stringSprava = "Zadajte heslo: \n";
            PomocnaTrieda::odosliSpravu(stringSprava, socketAMutex);

            if(PomocnaTrieda::prijmiSpravu(sprava, socketAMutex->getSocket()) ==1) {
                return nullptr;
            }

            std::string heslo = sprava;
            std::string dovodPrecoProblem;
            pouzivatel = data->prihlas(&meno, &heslo, socketAMutex->getSocket(), dovodPrecoProblem);



            if(pouzivatel == nullptr) {
                stringSprava = dovodPrecoProblem +  "Chcete skusit znova [1 = ano]? \n";
                PomocnaTrieda::odosliSpravu(stringSprava, socketAMutex);



                if(PomocnaTrieda::prijmiSpravu(sprava, socketAMutex->getSocket()) == 1) {
                    return nullptr;
                }

                if(sprava[0] != '1') {
                    return nullptr;
                }

            }  else {
                uspesnePrihlaseny = true;

            }
        }
        std::string sprava = "Boli ste uspesne prihlaseny: \n";
        PomocnaTrieda::odosliSpravu(sprava, socketAMutex);


        data->odosliUzivateloviNeprecitaneSpravy(pouzivatel);

        return pouzivatel;
    }

    static bool registrujSa(Data* data, SocketAMutex* socketAMutex) {

        bool uspesneZaregistrovany  = false;
        while(!uspesneZaregistrovany) {
            char sprava[4096];
            std::string stringSprava = "Zadajte svoje pouzivatelske meno: \n";
            PomocnaTrieda::odosliSpravu(stringSprava, socketAMutex);

            if(PomocnaTrieda::prijmiSpravu(sprava, socketAMutex->getSocket()) == 1) {
                return true;
            }
            std::string meno = sprava;
            if(data->jeMenoUnikatne(meno)) {
                stringSprava = "Zadajte svoje heslo: \n";
                PomocnaTrieda::odosliSpravu(stringSprava, socketAMutex);

                if(PomocnaTrieda::prijmiSpravu(sprava, socketAMutex->getSocket()) == 1) {
                    return true;
                }
                std::string heslo = sprava;
                data->registruj(&meno, &heslo);

                stringSprava = "Registracia prebehla uspesne. \n";
                stringSprava += "Ak chcete, mozete sa prihlasit\n";
                PomocnaTrieda::odosliSpravu(stringSprava, socketAMutex);
                uspesneZaregistrovany = true;
            } else {
                stringSprava = "Pouzivatelske meno musi byt unikatne, chcete skusit znova?: \n";
                stringSprava += "[1] ano chcem.\n";
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

};
