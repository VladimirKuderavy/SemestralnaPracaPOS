//
// Created by Andrej on 3. 1. 2022.
//
#pragma once
#include <vector>
#include "Pouzivatel.h"

class Prihlaseny {
private:
    Pouzivatel* pouzivatel;
    int* socket;
public:
    Prihlaseny(Pouzivatel* pouzivatel, int* socket) {
        this->pouzivatel = pouzivatel;
        this->socket = socket;
    }

    int getSocket() {
        return *socket;
    }
    Pouzivatel* getPouzivatel() {
        return this->pouzivatel;
    }
};

class Data {
private:
    std::vector<Pouzivatel*> pouzivatelia;
    std::vector<Prihlaseny*> prihlaseni;

public:

    Data () {
        std::string meno = "Andrej";
        std::string priezvisko = "123";
        Pouzivatel* pouzivatel = new Pouzivatel(1, meno, priezvisko);
        pouzivatelia.push_back(pouzivatel);
    }



    Pouzivatel* prihlas(std::string* meno, std::string* heslo, int* socket) {

        for(int i = 0; i < pouzivatelia.size(); i++) {
            if(*pouzivatelia[i]->getMeno() == *meno) {
                if(pouzivatelia[i]->jeDobreHeslo(heslo)) {
                    prihlaseni.push_back(new Prihlaseny(pouzivatelia[i], socket));
                    return pouzivatelia[i];
                }
            }
        }
        return nullptr;
    }

    void registruj(std::string* meno, std::string* heslo) {
        Pouzivatel* pouzivatel = new Pouzivatel(pouzivatelia[pouzivatelia.size()-1]->getId()+1, *meno, *heslo);
        pouzivatelia.push_back(pouzivatel);
    }

    bool jeMenoUnikatne(std::string meno) {
        for(int i = 0; i < pouzivatelia.size(); i++) {
            if(*pouzivatelia[i]->getMeno() == meno) {
                return false;
            }
        }
        return true;
    }





};

