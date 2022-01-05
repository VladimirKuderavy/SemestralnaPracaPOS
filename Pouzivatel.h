#pragma once
#include <string>
#include <vector>
#include "Konverzacia.h"
//
// Created by Andrej on 3. 1. 2022.
//



class Pouzivatel {
private:
    int id;
    std::string pouzivatelskeMeno;
    std::string heslo;
    std::vector<Pouzivatel*> priatelia;

    std::vector<Pouzivatel*> ziadostiOPriatelstvo;
    std::vector<Konverzacia*> konverzacie;

public:
    Pouzivatel(int id, std::string& meno, std::string& heslo) {
        this->id = id;
        this->pouzivatelskeMeno = meno;
        this->heslo = heslo;
    }
    void pridajPriatela(Pouzivatel* pridajTohto) {
        priatelia.push_back(pridajTohto);
    }

    void odoberPriatela(Pouzivatel* odoberTohto) {
        int odober = -1;
        for(int i = 0; i < priatelia.size(); i++) {
            if(priatelia[i] == odoberTohto) {
                odober = i;
            }
        }
        if(odober != -1) {
            priatelia.erase(priatelia.begin() + odober);
        }
    }

    std::string* getMeno() {
        return &this->pouzivatelskeMeno;
    }
    bool jeDobreHeslo(std::string* parHeslo) {
        return *parHeslo == this->heslo;
    }
    int getId() {
        return this->id;
    }


    void pridajDoPriatelov(Pouzivatel* pouzivatel) {
        this->priatelia.push_back(pouzivatel);
    }



    bool uzJeVPriateloch(Pouzivatel* pouzivatel) {
        for(int i = 0; i < priatelia.size(); i++) {
            if(*priatelia[i]->getMeno() == *pouzivatel->getMeno()) {
                return true;
            }
        }

        return false;
    }

    void odoberZPriatelov(Pouzivatel* pouzivatel) {
        for(int i = 0; i < priatelia.size(); i++) {
            if(*priatelia[i]->getMeno() == *pouzivatel->getMeno()) {
                priatelia.erase(priatelia.begin() + i);
                return;
            }
        }
    }



    std::vector<Pouzivatel*>* getZiadostiOPriatelstvo() {
        return &this->ziadostiOPriatelstvo;
    }

    std::vector<Pouzivatel*>* getPriatelia() {
        return &this->priatelia;
    }
    std::vector<Konverzacia*>* getKonverzacie() {
        return &this->konverzacie;
    }

    Pouzivatel* vymazZoZiadostiOPriatelstvo(int index) {
        Pouzivatel* vrat = this->ziadostiOPriatelstvo[index];
        this->ziadostiOPriatelstvo.erase(this->ziadostiOPriatelstvo.begin() + index);
        return vrat;
    }

    bool jeVZiadostiach(Pouzivatel* pouzivatel) {
        for(int i = 0; i < this->ziadostiOPriatelstvo.size(); i++) {
            if(*ziadostiOPriatelstvo[i]->getMeno() == *pouzivatel->getMeno()) {
                return true;
            }
        }
        return false;
    }

    void pridajNovuZiadost(Pouzivatel* pridaj) {
        ziadostiOPriatelstvo.push_back(pridaj);
    }



};
