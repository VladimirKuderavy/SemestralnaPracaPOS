#pragma once
//
// Created by Andrej on 3. 1. 2022.
//
#include <sys/socket.h>
#include "Data.h"
#include <iostream>
#include <string>
#include <cstring>


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
            memset(sprava, 0, 4096);
            ssize_t bytesRecv = recv(*socket, sprava, 4096, 0);

            if(bytesRecv == -1) {
                std::cerr << "Problem so spojenim";
                return nullptr;
            }
            if(bytesRecv == 0) {
                std::cout << "Klient sa odpojil" << "\n";
                return nullptr;
            }

            if(sprava[0] == '1') {
                pouzivatel = prihlasSa(data, socket);
                if(pouzivatel != nullptr) {
                    bolUspesnePrihlaseny = true;
                }
            } else if(sprava[0] == '2') {
                pouzivatel = registrujSa(data, socket);
                if(pouzivatel != nullptr) {
                    bolUspesnePrihlaseny = true;
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
            std::string stringSprava = "Zadajte meno: \n";
            send(*socket, stringSprava.c_str(), stringSprava.size(), 0);

            char sprava[4096];
            memset(sprava, 0, 4096);

            ssize_t bytesRecv = recv(*socket, sprava, 4096, 0);
            if(bytesRecv == -1) {
                std::cerr << "Problem so spojenim";
                return nullptr;
            }
            if(bytesRecv == 0) {
                std::cout << "Klient sa odpojil" << "\n";
                return nullptr;
            }
            std::string meno = sprava;


            memset(sprava, 0, 4096);

            bytesRecv = recv(*socket, sprava, 4096, 0);
            if(bytesRecv == -1) {
                std::cerr << "Problem so spojenim";
                return nullptr;
            }
            if(bytesRecv == 0) {
                std::cout << "Klient sa odpojil" << "\n";
                return nullptr;
            }
            std::string heslo = sprava;
            pouzivatel = data->prihlas(&meno, &heslo, socket);
            if(pouzivatel == nullptr) {
                stringSprava = "Zadali ste zle meno alebo heslo, chcete skusit znova [1 = ano]?: \n";
                send(*socket, stringSprava.c_str(), stringSprava.size(), 0);


                memset(sprava, 0, 4096);
                bytesRecv = recv(*socket, sprava, 4096, 0);
                if(bytesRecv == -1) {
                    std::cerr << "Problem so spojenim";
                    return nullptr;
                }
                if(bytesRecv == 0) {
                    std::cout << "Klient sa odpojil" << "\n";
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

    static Pouzivatel* registrujSa(Data* data, int* socket) {




        return nullptr;
    }

};
