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
        Pouzivatel* pouzivatel = new Pouzivatel(1, "Andrej", "123");
        pouzivatelia.push_back(pouzivatel);
    }

    void pridajPrihlaseneho(Pouzivatel* pouzivatel, int* socket) {
        Prihlaseny* prihlaseny = new Prihlaseny(pouzivatel, socket);


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


};

