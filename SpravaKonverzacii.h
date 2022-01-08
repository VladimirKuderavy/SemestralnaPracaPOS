#pragma once
#include "Data.h"
#include "Pomocna.h"
#include "Konstanty.h"

#include<fstream>
#include <sstream>



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
            //volba ci chce posla5 subor
            stringSprava = "[1]Chcem poslat textovu spravu\n";
            stringSprava += "[2]Chcem poslat subor\n";
            send(*socket, stringSprava.c_str(), stringSprava.size(), 0);


            if(PomocnaTrieda::prijmiSpravu(sprava, socket) == 1) {
                return true;
            }
            if(sprava[0] == '2') {
                //odoslanie suboru
                if(posliSubor(pouzivatel, data, socket, index)) {
                    return true;
                }
                continue;
            }
            //poslanie textovej spravy
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

    static bool posliSubor(Pouzivatel* pouzivatel, Data* data, int* socket, int index) {

        std::string stringSprava = "Naozaj chcete poslat subor? [1]\n";
        stringSprava += "krok spat [2]";
        send(*socket, stringSprava.c_str(), stringSprava.size(), 0);

        char sprava[4096];
        if(PomocnaTrieda::prijmiSpravu(sprava, socket) == 1) {
            return true;
        }

        if(sprava[0] != '1') {
            return false;
        }

        stringSprava = "Zadajte nazov suboru: \n";
        send(*socket, stringSprava.c_str(), stringSprava.size(), 0);

        stringSprava= Konstanty::getTextVstupDoModuNacitajSubor()+"\n";
        send(*socket, stringSprava.c_str(), stringSprava.size(), 0);

        if(PomocnaTrieda::prijmiSpravu(sprava, socket) == 1) {
            return true;
        }
        std::string obsahSpravy = sprava;

        if(obsahSpravy == Konstanty::getTextSuborSaNepodariloNacitat()) {
            return false;
        }



        std::string nazovSuboru;
        std::string obsahSuboru;
        int zostavaPocetZnakov;


        std::istringstream stringstream(obsahSpravy);

        //v riadku nazov suboru
        std::getline(stringstream, nazovSuboru);

        std::string zostavaPocetZnakovString;
        std::getline(stringstream, zostavaPocetZnakovString);
        zostavaPocetZnakov = std::stoi(zostavaPocetZnakovString);


        obsahSuboru = obsahSpravy;
        obsahSuboru.erase(0, obsahSuboru.find("\n") + 1);
        obsahSuboru.erase(0, obsahSuboru.find("\n") + 1);

        zostavaPocetZnakov = zostavaPocetZnakov - obsahSuboru.size();


        while(zostavaPocetZnakov > 0) {

            if(PomocnaTrieda::prijmiSpravu(sprava, socket) == 1) {
                return true;
            }
            obsahSpravy = sprava;
            obsahSuboru += obsahSpravy;
            zostavaPocetZnakov -= obsahSpravy.size();


        }

        //logika poslania suboru
        std::string tymtoSaNepodariloOdoslat = "";

        if(data->posliSubor(pouzivatel, index, nazovSuboru, obsahSuboru, tymtoSaNepodariloOdoslat)) {
            if(tymtoSaNepodariloOdoslat != "") {
                stringSprava = "Tymto ucastnikom sa nepodarilo subor odoslat, lebo ich nemate v priateloch:\n";
                stringSprava+= tymtoSaNepodariloOdoslat; + "\n";
                stringSprava+= "Ostatnym ucastnikom konverzacie sa subor poslal\n";
                send(*socket, stringSprava.c_str(), stringSprava.size(), 0);
            } else {
                stringSprava = "Subor odoslany\n";
                send(*socket, stringSprava.c_str(), stringSprava.size(), 0);
            }

        } else {
            stringSprava = "Zadali ste zly index\n";
            send(*socket, stringSprava.c_str(), stringSprava.size(), 0);
        }





        return false;
    }


};


