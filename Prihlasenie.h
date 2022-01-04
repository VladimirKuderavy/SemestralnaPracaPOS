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



    static Pouzivatel* prihlasenie(Data* data, int* socket) {

        bool bolUspesnePrihlaseny = false;
        char sprava[4096];
        Pouzivatel* pouzivatel = nullptr;
        while(!bolUspesnePrihlaseny) {


            std::string stringSprava = "Zvoľte si možnosť: \n";
            stringSprava+= "[1] Prihlásiť sa\n";
            stringSprava+= "[2] Zaregistrovať sa\n";

            send(*socket, stringSprava.c_str(), stringSprava.size(), 0);

            //vycistit bufer
            if(PomocnaTrieda::prijmiSpravu(sprava, socket) == 1) {
                return nullptr;
            }



            if(sprava[0] == '1') {
                pouzivatel = prihlasSa(data, socket);
                if(pouzivatel != nullptr) {
                    bolUspesnePrihlaseny = true;
                }
            } else if(sprava[0] == '2') {
                bool dosloKChybe = registrujSa(data, socket);
                if(dosloKChybe) {
                    return nullptr;
                }
            }

            stringSprava = "Zadali ste zlu moznost \n";
        }
        return pouzivatel;
    }
private:
    static Pouzivatel* prihlasSa(Data* data, int* socket) {
        bool uspesnePrihlaseny = false;
        Pouzivatel* pouzivatel = nullptr;
        while(!uspesnePrihlaseny) {
            std::string stringSprava = "Zadajte pouzivatelske meno: \n";
            send(*socket, stringSprava.c_str(), stringSprava.size(), 0);

            char sprava[4096];
            if(PomocnaTrieda::prijmiSpravu(sprava, socket) == 1) {
                return nullptr;
            }
            std::string meno = sprava;


            stringSprava = "Zadajte heslo: \n";
            send(*socket, stringSprava.c_str(), stringSprava.size(), 0);

            if(PomocnaTrieda::prijmiSpravu(sprava, socket) ==1) {
                return nullptr;
            }

            std::string heslo = sprava;
            pouzivatel = data->prihlas(&meno, &heslo, socket);

            if(pouzivatel == nullptr) {
                stringSprava = "Zadali ste zle pouzivatelske meno alebo heslo, chcete skusit znova [1 = ano]?: \n";
                send(*socket, stringSprava.c_str(), stringSprava.size(), 0);


                if(PomocnaTrieda::prijmiSpravu(sprava, socket) == 1) {
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
        send(*socket, sprava.c_str(), sprava.size(), 0);


        return pouzivatel;
    }

    static bool registrujSa(Data* data, int* socket) {

        bool uspesneZaregistrovany  = false;
        while(!uspesneZaregistrovany) {
            char sprava[4096];
            std::string stringSprava = "Zadajte svoje pouzivatelske meno: \n";
            send(*socket, stringSprava.c_str(), stringSprava.size(), 0);

            if(PomocnaTrieda::prijmiSpravu(sprava, socket) == 1) {
                return true;
            }
            std::string meno = sprava;
            if(data->jeMenoUnikatne(meno)) {
                stringSprava = "Zadajte svoje heslo: \n";
                send(*socket, stringSprava.c_str(), stringSprava.size(), 0);

                if(PomocnaTrieda::prijmiSpravu(sprava, socket) == 1) {
                    return true;
                }
                std::string heslo = sprava;
                data->registruj(&meno, &heslo);

                stringSprava = "Registracia prebehla uspesne. \n";
                stringSprava += "Ak chcete, mozete sa prihlasit\n";
                send(*socket, stringSprava.c_str(), stringSprava.size(), 0);
                uspesneZaregistrovany = true;
            } else {
                stringSprava = "Pouzivatelske meno musi byt unikatne, chcete skusit znova?: \n";
                stringSprava += "[1] ano chcem.\n";
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

};
