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
#include <unordered_map>
#include "Sprava.h"
#include <string>
#include "unistd.h"

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
    //std::vector<Pouzivatel*> pouzivatelia;
    std::unordered_map<std::string, Pouzivatel*> pouzivatelia;
    std::vector<Prihlaseny*> prihlaseni;
    std::vector<Konverzacia*> konverzacie;

    std::vector<int> otvoreneSockety;




    //kriticka sekcia so spravou
    std::vector<Sprava*> spravy;

    pthread_mutex_t* mutexSpravy;
    pthread_cond_t* spravyPlus;
public:


    Data (pthread_mutex_t* mutexSpravy, pthread_cond_t* spravyPlus) {
        this->mutexSpravy = mutexSpravy;
        this->spravyPlus = spravyPlus;

        std::string meno = "Andrej";
        std::string priezvisko = "123";
        Pouzivatel* pouzivatel = new Pouzivatel(meno, priezvisko);
        pouzivatelia.insert({*pouzivatel->getMeno(), pouzivatel});

        meno = "Vladimir";
        priezvisko = "123";
        pouzivatel = new Pouzivatel(meno, priezvisko);
        pouzivatelia.insert({*pouzivatel->getMeno(), pouzivatel});
    }
    ~Data() {
        for(int i = 0; i < prihlaseni.size(); i++) {
            delete prihlaseni[i];
        }
        for (auto pouzivatel : this->pouzivatelia) {
            delete pouzivatel.second;
        }

        for(int i = 0; i < konverzacie.size(); i++) {
            delete konverzacie[i];
        }
    }

    void pridajOtvorenySocket(int socket) {


    }

    void vymazOtvorenySocket(int socket) {
        for(int i = 0; i < this->otvoreneSockety.size(); i++) {
            if(otvoreneSockety[i] == socket) {
                otvoreneSockety.erase(otvoreneSockety.begin() + i);
                break;
            }
        }
    }
    void zatvorSocket(int socket) {
        vymazOtvorenySocket(socket);
        close(socket);
    }
    void zatvorVsetkyOtvoreneSockety() {
        for(int i = 0; i < this->otvoreneSockety.size(); i++) {
            close(this->otvoreneSockety[i]);
        }
    }




    pthread_mutex_t* getMutexSpravy() {
        return this->mutexSpravy;
    }

    pthread_cond_t* getCondSpravy() {
        return this->spravyPlus;
    }
    std::vector<Sprava*>* getSpravy() {
        return &this->spravy;
    }

    //zobrazi zoznam konverzacii daneho pouzivatela... vytvorit novu konverzaciu
    //vytvorit novu -> zada nazov konverzacie ->vytvori sa prazdna konverzacia-> moznost pridat pridatelov

    //              ->zada v ktorej konv chce pisat ->vypyta obsah spravy ->potvrdit



    bool posliSpravu(std::string& adresat, std::string obsahSpravy) {
        pthread_mutex_lock(this->mutexSpravy);
        this->spravy.push_back(new Sprava(adresat, obsahSpravy));
        pthread_mutex_unlock(this->mutexSpravy);
        pthread_cond_signal(this->spravyPlus);
        return true;
    }

    bool posliSpravu(Pouzivatel* pouzivatel, int indexKonverzacie, std::string& obsahSpravy, std::string& zoznamKtorymSaNepodariloPoslat) {
        //prejde vsetkych pouzivatelov danej konverzacie, pre kazdeho vytvori spravu a odosle ju
        if(pouzivatel->getKonverzacie()->size() <= indexKonverzacie ) {
            return false;
        }



        Konverzacia* konverzacia = (*pouzivatel->getKonverzacie())[indexKonverzacie];

        std::string hlavickaSpravy = "Nova sprava: " ;
        hlavickaSpravy+= "Od pouzivatela: " + *pouzivatel->getMeno() + "\n";
        hlavickaSpravy+= "v konverzacii: " + *konverzacia->getNazov() + "\n";

        hlavickaSpravy+= obsahSpravy;
        obsahSpravy = hlavickaSpravy;

        for(int i = 0; i < konverzacia->getZoznamUcastnikov()->size(); i++) {
            std::string adresat = (*konverzacia->getZoznamUcastnikov())[i];
            if(adresat != *pouzivatel->getMeno()) {
                //kontrola ci ho ma odosielatel vo svojich priateloch, ak nie, tak to nieco vrati
                bool jeVPriateloch = false;
                for(int j = 0; j < pouzivatel->getPriatelia()->size(); j++) {
                    if(*(*pouzivatel->getPriatelia())[j]->getMeno() == adresat) {
                        jeVPriateloch = true;
                    }
                }
                if(jeVPriateloch) {
                    posliSpravu(adresat, obsahSpravy);
                    std::cout << "pridal som spravu do pola a zavolal pthread cond\n";
                } else {
                    zoznamKtorymSaNepodariloPoslat+= adresat + "\n";
                }

            }
        }


        return true;
    }

    void odosliSpravuCezSocket(std::string menoUzivatela, std::string obsahSpravy) {
        int socket = -1;
        for(int i = 0; i < prihlaseni.size(); i++) {
            if(*(prihlaseni[i]->getPouzivatel()->getMeno()) == menoUzivatela) {
                socket = prihlaseni[i]->getSocket();
            }
        }
        if(socket == -1) {
            //TODO pridaj do zoznamu sprav ktore sa maju zobrazit uzivatelovi ked sa prihlasi
        }
        std::cout << "Odoslal som spravu\n";
        std::cout << "Socket: " << std::to_string(socket);
        send(socket, obsahSpravy.c_str(), obsahSpravy.size(), 0);

    }



    Pouzivatel* prihlas(std::string* meno, std::string* heslo, int* socket) {

        auto pouzivatelIterator = this->pouzivatelia.find(*meno);

        if (pouzivatelIterator != this->pouzivatelia.end()) {
            if(pouzivatelIterator->second->jeDobreHeslo(heslo)) {
                prihlaseni.push_back(new Prihlaseny(pouzivatelIterator->second, socket));
                return pouzivatelIterator->second;
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
        Pouzivatel* pouzivatel = new Pouzivatel(*meno, *heslo);
        pouzivatelia.insert({*pouzivatel->getMeno(), pouzivatel});
    }

    bool jeMenoUnikatne(std::string& meno) {
        auto pouzivatelIterator = this->pouzivatelia.find(meno);

        if (pouzivatelIterator != this->pouzivatelia.end()) {
            return false;
        }

        return true;
    }

    //true ak sa podarilo, false ak sa nepodarilo
    bool posliZiadostOPriatelstvo(Pouzivatel* odoslalZiadost, std::string menoKomuOdoslal) {

        auto posliTomutoIterator = this->pouzivatelia.find(menoKomuOdoslal);

        if (posliTomutoIterator == this->pouzivatelia.end()) {
            return false;
        }

        if(posliTomutoIterator->second->jeVZiadostiach(odoslalZiadost)
            || posliTomutoIterator->second->uzJeVPriateloch(odoslalZiadost) ) {

            return false;

        }

        //poslanie spravy tom ze bola odoslana
        std::string obsahSpravy = "Nova ziadost o priatelstvo od pouzivatela: " + *odoslalZiadost->getMeno();
        this->posliSpravu(menoKomuOdoslal, obsahSpravy);

        posliTomutoIterator->second->pridajNovuZiadost(odoslalZiadost);
        return true;
    }


    /*
    void posliOdobratieZPriatelov(Pouzivatel* odoslalZiadost, Pouzivatel* chceOdobratTohto) {
        //poslanie spravy uzivatelovi -> chce odobrat tohto
        std::string obsahSpravy = "Uzivatel " + *odoslalZiadost->getMeno() + "si Vas odobral zo svojich priatelov\n";
        this->posliSpravu(*chceOdobratTohto->getMeno(), obsahSpravy);



        odoberZPriatelov(odoslalZiadost, chceOdobratTohto);

    }*/

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

        Pouzivatel* pridajTohto = pouzivatel->vymazZoZiadostiOPriatelstvo(index);

        if(prijal) {

            //posli spravu ze prial
            std::string obsahSpravy = "Pouzivatel " + *pouzivatel->getMeno() + " prial Vasu ziadost o priatelstvo.\n";
            this->posliSpravu(*pridajTohto->getMeno(), obsahSpravy);

            pridajDoPriatelov(pouzivatel, pridajTohto);

        } else {
            //posli spravu ze neprial
            std::string obsahSpravy = "Pouzivatel " + *pouzivatel->getMeno() + " odmietol Vasu ziadost o priatelstvo.\n";
            this->posliSpravu(*pridajTohto->getMeno(), obsahSpravy);


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
    bool spracujOdobratieZPriatelov(Pouzivatel* pouzivatel,int index, std::string dovod = "") {
        if(pouzivatel->getPriatelia()->size() <= index) {
            return false;
        }

        Pouzivatel* odoberTohto = (*pouzivatel->getPriatelia())[index];
        //posli spravu ze odobral
        std::string obsahSpravy = "Uzivatel " + *pouzivatel->getMeno() + " si Vas odobral zo svojich priatelov " + dovod + "\n";
        this->posliSpravu(*odoberTohto->getMeno(), obsahSpravy);


        this->odoberZPriatelov(pouzivatel, odoberTohto);

        return true;
    }


    void odstranPouzivateloviVsetkychPriatelov(Pouzivatel* pouzivatel) {
        int pocetNaOdobratie = pouzivatel->getPriatelia()->size();
        for(int i = 0; i < pocetNaOdobratie; i++) {
            spracujOdobratieZPriatelov(pouzivatel, 0, "z dovodu zrusenia uctu");
        }
    }
    void odstranUcet(Pouzivatel* pouzivatel) {
        pouzivatelia.erase(*pouzivatel->getMeno());
        delete pouzivatel;
    }


    void odhlasPouzivatela(Pouzivatel* odhlasMna) {
        for(int i = 0; i < this->prihlaseni.size(); i++) {
            if(this->prihlaseni[i]->getPouzivatel() == odhlasMna) {
               this->prihlaseni.erase(this->prihlaseni.begin() + i);
               break;
            }
        }
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

