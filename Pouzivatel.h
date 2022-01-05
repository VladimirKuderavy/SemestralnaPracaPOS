#pragma once
#include <string>
#include <vector>
//
// Created by Andrej on 3. 1. 2022.
//


class Pouzivatel {
private:
    int id;
    std::string pouzivatelskeMeno;
    std::string heslo;
    std::vector<Pouzivatel*> priatelia;

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

};
