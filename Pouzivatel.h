#pragma once
#include <string>
#include <vector>
#include "Konverzacia.h"
//
// Created by Andrej on 3. 1. 2022.
//

class Pouzivatel {
private:
    std::string pouzivatelskeMeno;
    std::string heslo;
    std::vector<Pouzivatel*> priatelia;
    std::vector<Pouzivatel*> ziadostiOPriatelstvo;
    std::vector<Konverzacia*> konverzacie;
    std::vector<std::string> neprecitaneSpravy;

public:
    Pouzivatel(std::string& meno, std::string& heslo) {
        this->pouzivatelskeMeno = meno;
        this->heslo = heslo;
    }
    void vymazNeprecitaneSpravy() {
        this->neprecitaneSpravy.clear();
    }

    void pridajNeprecitanuSpravu(std::string& sprava) {
        neprecitaneSpravy.push_back(sprava);
    }

    std::vector<std::string>* dajNeprecitaneSpravy() {
        return &this->neprecitaneSpravy;
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

    std::string toString() {
        return this->pouzivatelskeMeno + "\n" + this->heslo + "\n";
    }

};
