//
// Created by Andrej on 3. 1. 2022.
//
#pragma once
#include <vector>
#include "Pouzivatel.h"
#include "Konverzacia.h"
#include "pthread.h"

#include <sys/socket.h>
#include <iostream>



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
    std::vector<Konverzacia*> konverzacie;




public:

    Data () {
        std::string meno = "Andrej";
        std::string priezvisko = "123";
        Pouzivatel* pouzivatel = new Pouzivatel(1, meno, priezvisko);
        pouzivatelia.push_back(pouzivatel);

        meno = "Vladimir";
        priezvisko = "123";
        pouzivatel = new Pouzivatel(1, meno, priezvisko);
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

    void vytvorKonverzaciu(Pouzivatel* pouzivatel, int& indexKonverzacie, std::string& nazovKonverzacie) {
        Konverzacia* konverzacia = new Konverzacia(nazovKonverzacie);
        konverzacia->pridajUcastnika(*pouzivatel->getMeno());
        pouzivatel->getKonverzacie()->push_back(konverzacia);
        indexKonverzacie = pouzivatel->getKonverzacie()->size() -1;
        this->konverzacie.push_back(konverzacia);

    }

    bool pridajDoKonverzacie(Pouzivatel* pouzivatel, int& indexKonverzacie, int indexVPoliJehoPriatelov) {
        if(pouzivatel->getKonverzacie()->size() <= indexKonverzacie) {
            return false;
        }
        if(pouzivatel->getPriatelia()->size() <= indexVPoliJehoPriatelov) {
            return false;
        }
        Pouzivatel* pridajTohto = (*pouzivatel->getPriatelia())[indexVPoliJehoPriatelov];
        Konverzacia* konverzacia = (*pouzivatel->getKonverzacie())[indexKonverzacie];

        for(int i = 0; i < konverzacia->getZoznamUcastnikov()->size(); i++) {
            if((*konverzacia->getZoznamUcastnikov())[i] ==  *pridajTohto->getMeno()) {
                return false;
            }
        }
        pridajTohto->getKonverzacie()->push_back(konverzacia);
        (*pouzivatel->getKonverzacie())[indexKonverzacie]->pridajUcastnika(*(pridajTohto->getMeno()));
        return true;
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

    //true ak sa podarilo, false ak sa nepodarilo
    bool posliZiadostOPriatelstvo(Pouzivatel* odoslalZiadost, std::string menoKomuOdoslal) {
        Pouzivatel* posliTomuto =nullptr;
        for(int i = 0; i < pouzivatelia.size(); i++) {
            if(*pouzivatelia[i]->getMeno() == menoKomuOdoslal) {
                posliTomuto = pouzivatelia[i];
            }
        }
        if(posliTomuto == nullptr) {
            return false;
        }
        if(posliTomuto->jeVZiadostiach(odoslalZiadost) || posliTomuto->uzJeVPriateloch(odoslalZiadost) ) {
            return false;
        }

        //TODO poslanie spravy tom ze bola odoslana
        posliTomuto->pridajNovuZiadost(odoslalZiadost);
        return true;
    }

    void posliOdobratieZPriatelov(Pouzivatel* odoslalZiadost, Pouzivatel* chceOdobratTohto) {
        //TODO poslanie spravy uzivatelovi -> chce odobrat tohto
        odoberZPriatelov(odoslalZiadost, chceOdobratTohto);

    }

    std::string dajZoznamZiadostiOPriatelstvo(Pouzivatel* pouzivatel) {
        std::string vrat = "";
        //zamknut mutex
        for(int i = 0; i < pouzivatel->getZiadostiOPriatelstvo()->size(); i++) {
            vrat += "["  + std::to_string(i) + "]" + *(*pouzivatel->getZiadostiOPriatelstvo())[i]->getMeno() + "\n";
        }
        //unlock mutex
        return vrat;
    }



    //vrati true, ak prebehlo vporiadku
    bool spracujZiadostOPriatelstvo(Pouzivatel* pouzivatel,int index,bool prijal) {
        if(pouzivatel->getZiadostiOPriatelstvo()->size() <= index) {
            return false;
        }
        if(prijal) {
            Pouzivatel* pridajTohto = pouzivatel->vymazZoZiadostiOPriatelstvo(index);
            //TODO posli spravu ze prial

            pridajDoPriatelov(pouzivatel, pridajTohto);

        } else {
            //TODO posli spravu ze neprial
            pouzivatel->vymazZoZiadostiOPriatelstvo(index);

        }
        return true;
    }

    std::string dajZoznamPriatelov(Pouzivatel* pouzivatel) {
        std::string vrat = "";
        for(int i = 0; i < pouzivatel->getPriatelia()->size(); i++) {
            vrat += "["  + std::to_string(i) + "]" + *(*pouzivatel->getPriatelia())[i]->getMeno() + "\n";
        }
        return vrat;
    }

    std::string dajZoznamKonverzaciiPouzivatela(Pouzivatel* pouzivatel) {
        std::string vrat = "";
        for(int i = 0; i < pouzivatel->getKonverzacie()->size(); i++) {
            vrat += "["  + std::to_string(i) + "]" + *(*pouzivatel->getKonverzacie())[i]->getNazov() + "\n";
            vrat+="   Ucastnici: \n";
            for(int j = 0; j < (*pouzivatel->getKonverzacie())[i]->getZoznamUcastnikov()->size(); j++) {
                vrat+= "," + (*(*pouzivatel->getKonverzacie())[i]->getZoznamUcastnikov())[j];
            }
        }
        return vrat;
    }

    //vracia true ak prebehlo OK
    bool spracujOdobratieZPriatelov(Pouzivatel* pouzivatel,int index) {
        if(pouzivatel->getPriatelia()->size() <= index) {
            return false;
        }

        Pouzivatel* odoberTohto = (*pouzivatel->getPriatelia())[index];
        //TODO posli spravu ze odobral

        this->odoberZPriatelov(pouzivatel, odoberTohto);

        return true;
    }



private:
    void pridajDoPriatelov(Pouzivatel* pouzivatel1, Pouzivatel* pouzivatel2) {
        pouzivatel1->pridajDoPriatelov(pouzivatel2);
        pouzivatel2->pridajDoPriatelov(pouzivatel1);
    }
    void odoberZPriatelov(Pouzivatel* pouzivatel1, Pouzivatel* pouzivatel2) {
        pouzivatel1->odoberPriatela(pouzivatel2);
        pouzivatel2->odoberPriatela(pouzivatel1);
    }





};

