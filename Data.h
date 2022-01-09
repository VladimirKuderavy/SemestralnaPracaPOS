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
#include "Zapisovac.h"
#include "Nacitavac.h"
#include <sstream>
#include "Hash.h"

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

class SocketAMutex {
private:
    int socket;
    pthread_mutex_t* mutex;
public:
    SocketAMutex(int socket, pthread_mutex_t* mutex) {
        this->socket = socket;
        this->mutex = mutex;
    }
    ~SocketAMutex() {
        delete this->mutex;
    }
    int* getSocket() {
        return &this->socket;
    }
    pthread_mutex_t* getMutex() {
        return this->mutex;
    }
};

class Data {
private:
    std::unordered_map<std::string, Pouzivatel*> pouzivatelia;
    std::vector<Prihlaseny*> prihlaseni;
    std::vector<Konverzacia*> konverzacie;
    std::vector<SocketAMutex*> otvoreneSocketyAMutexy;



    pthread_mutex_t mutexData;
public:
    Data () {
        pthread_mutex_init(&mutexData, NULL);


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
        pthread_mutex_destroy(&mutexData);
    }
    pthread_mutex_t* getMutex() {
        return &this->mutexData;
    }

    //TODO hotovo
    void pridajOtvorenySocket(SocketAMutex* socketAMutex) {

        pthread_mutex_lock(&mutexData);
        this->otvoreneSocketyAMutexy.push_back(socketAMutex);
        pthread_mutex_unlock(&mutexData);
    }
    //TODO hotovo

    void vymazOtvorenySocket(int socket) {
        pthread_mutex_lock(&mutexData);
        for(int i = 0; i < this->otvoreneSocketyAMutexy.size(); i++) {
            if(*otvoreneSocketyAMutexy[i]->getSocket() == socket) {

                pthread_mutex_t* mutexSocketu = otvoreneSocketyAMutexy[i]->getMutex();
                int socket = *otvoreneSocketyAMutexy[i]->getSocket();
                pthread_mutex_lock(mutexSocketu);
                //lock otestuje, ci je volny
                pthread_mutex_unlock(mutexSocketu);

                pthread_mutex_destroy(mutexSocketu);
                close(socket);
                delete otvoreneSocketyAMutexy[i];

                otvoreneSocketyAMutexy.erase(otvoreneSocketyAMutexy.begin() + i);
                break;
            }
        }
        pthread_mutex_unlock(&mutexData);

        //posiela na ten mutex
        // caka na odomknutie mutexu lebo chce vymazat
        // chcelo poslat, ale mutex je zamknuty
        //

    }

    /*
    //TODO hotovo
    void zatvorSocket(int socket) {
        vymazOtvorenySocket(socket);
        close(socket);
    }
     */

    //TODO hotovo
    void zatvorVsetkyOtvoreneSockety() {
        //Toto sa vola az v maine, ked budu vsetky vlakna ukoncene
        for(int i = 0; i < this->otvoreneSocketyAMutexy.size(); i++) {
            close(*this->otvoreneSocketyAMutexy[i]->getSocket());
            if(this->otvoreneSocketyAMutexy[i]->getMutex() != nullptr) {
                pthread_mutex_destroy(this->otvoreneSocketyAMutexy[i]->getMutex());
            }
            delete this->otvoreneSocketyAMutexy[i];
        }
    }
    //toto treba volat jedine so zamknutym socketom
    pthread_mutex_t* dajMutexSocketu(int& socket) {
        for(int i = 0; i < this->otvoreneSocketyAMutexy.size(); i++) {
            if(*otvoreneSocketyAMutexy[i]->getSocket() == socket) {
                return otvoreneSocketyAMutexy[i]->getMutex();
            }
        }
        return nullptr;
    }

    //zobrazi zoznam konverzacii daneho pouzivatela... vytvorit novu konverzaciu
    //vytvorit novu -> zada nazov konverzacie ->vytvori sa prazdna konverzacia-> moznost pridat pridatelov

    //              ->zada v ktorej konv chce pisat ->vypyta obsah spravy ->potvrdit

    //TODO hotovo
    //tunak treba posielat kopiu lebo sa to vola v cykle a keby nebola kopia, tak sa to zasifruje niekolkokrat
    void odosliSuborCezSocket(std::string& menoUzivatela, std::string hlavickaSuboru, std::string obsahSuboru) {
        int socket = -1;
        Hash::zasifrujSpravu(obsahSuboru);
        Hash::zasifrujSpravu(hlavickaSuboru);

        pthread_mutex_lock(&this->mutexData);

        for(int i = 0; i < prihlaseni.size(); i++) {
            if(*(prihlaseni[i]->getPouzivatel()->getMeno()) == menoUzivatela) {
                socket = prihlaseni[i]->getSocket();
                break;
            }
        }
        if(socket == -1) {
            Pouzivatel* pouzivatel = pouzivatelia.find(menoUzivatela)->second;
            //pridaj do zoznamu sprav ktore sa maju zobrazit uzivatelovi ked sa prihlasi
            std::string obsahSpravy = "V case Vasej nedostupnosti sa Vam niekto snazil poslat subor:\n";
            pouzivatel->pridajNeprecitanuSpravu(obsahSpravy);
            pthread_mutex_unlock(&this->mutexData);
            return;
        }
        pthread_mutex_t* mutexSocketu = this->dajMutexSocketu(socket);
        pthread_mutex_lock(mutexSocketu);
        pthread_mutex_unlock(&this->mutexData);

        //POSLANIE SUBORU
        send(socket, hlavickaSuboru.c_str(), hlavickaSuboru.size(), 0);

        int poslanePo = 0;

        while(poslanePo < obsahSuboru.size()) {
            int hornaHranica = 4096;
            if(poslanePo + 4096 > obsahSuboru.size()) {
                hornaHranica = obsahSuboru.size() - poslanePo;
            }

            std::string poslat = obsahSuboru.substr(poslanePo, hornaHranica);

            send(socket, poslat.c_str(), poslat.size(), 0);


            poslanePo += hornaHranica;
        }
        pthread_mutex_unlock(mutexSocketu);

    }

    //TODO hotovo
    bool posliSubor(Pouzivatel* pouzivatel ,int indexKonverzacie ,std::string& nazovSuboru,std::string& obsahSuboru,std::string& zoznamKtorymSaNepodariloPoslat) {
        pthread_mutex_lock(&this->mutexData);
        if(pouzivatel->getKonverzacie()->size() <= indexKonverzacie ) {
            pthread_mutex_unlock(&this->mutexData);
            return false;
        }

        Konverzacia* konverzacia = (*pouzivatel->getKonverzacie())[indexKonverzacie];
        pthread_mutex_unlock(&this->mutexData);

        std::string hlavickaSuboru = Konstanty::getTextUlozSubor() + "\n";
        hlavickaSuboru += "Pouzivatel " + *pouzivatel->getMeno() + "Vam posiela subor: " +
                nazovSuboru + " v konverzacii: " + *konverzacia->getNazov() + "\n";
        hlavickaSuboru += nazovSuboru + "\n";
        hlavickaSuboru += std::to_string(obsahSuboru.size()) + "\n";

        for(int i = 0; i < konverzacia->getZoznamUcastnikov()->size(); i++) {
            std::string adresat = (*konverzacia->getZoznamUcastnikov())[i];
            if(adresat != *pouzivatel->getMeno()) {
                //kontrola ci ho ma odosielatel vo svojich priateloch, ak nie, tak to nieco vrati
                bool jeVPriateloch = false;



                pthread_mutex_lock(&this->mutexData);

                for(int j = 0; j < pouzivatel->getPriatelia()->size(); j++) {
                    if(*(*pouzivatel->getPriatelia())[j]->getMeno() == adresat) {
                        jeVPriateloch = true;
                    }
                }
                pthread_mutex_unlock(&this->mutexData);

                if(jeVPriateloch) {
                    odosliSuborCezSocket(adresat, hlavickaSuboru, obsahSuboru);

                } else {
                    zoznamKtorymSaNepodariloPoslat+= adresat + "\n";
                }

            }
        }

        return true;
    }


    //TODO pozreli sme
    bool posliSpravu(Pouzivatel* pouzivatel, int indexKonverzacie, std::string& obsahSpravy, std::string& zoznamKtorymSaNepodariloPoslat) {
        //prejde vsetkych pouzivatelov danej konverzacie, pre kazdeho vytvori spravu a odosle ju
        pthread_mutex_lock(&this->mutexData);
        if(pouzivatel->getKonverzacie()->size() <= indexKonverzacie ) {
            pthread_mutex_unlock(&this->mutexData);
            return false;
        }



        Konverzacia* konverzacia = (*pouzivatel->getKonverzacie())[indexKonverzacie];
        pthread_mutex_unlock(&this->mutexData);

        std::string hlavickaSpravy = "Nova sprava: " ;
        hlavickaSpravy += "Od pouzivatela: " + *pouzivatel->getMeno() + "\n";
        hlavickaSpravy += "v konverzacii: " + *konverzacia->getNazov() + "\n";

        hlavickaSpravy += "Napisal Vam: " + obsahSpravy + "\n";
        obsahSpravy = hlavickaSpravy;





        for(int i = 0; i < konverzacia->getZoznamUcastnikov()->size(); i++) {
            std::string adresat = (*konverzacia->getZoznamUcastnikov())[i];
            if(adresat != *pouzivatel->getMeno()) {
                //kontrola ci ho ma odosielatel vo svojich priateloch, ak nie, tak to nieco vrati
                bool jeVPriateloch = false;

                pthread_mutex_lock(&this->mutexData);
                for(int j = 0; j < pouzivatel->getPriatelia()->size(); j++) {
                    if(*(*pouzivatel->getPriatelia())[j]->getMeno() == adresat) {
                        jeVPriateloch = true;
                    }
                }
                pthread_mutex_unlock(&this->mutexData);

                if(jeVPriateloch) {



                    // 1. kto posiela, 2. v ktorej konverzacii, obsah spravy
                    //ALTERNATIVA
                    //prida do spravy na odoslanie -> vyberie -> odosliSpravuCez socket
                    //odosliSpravuCezSocket

                    odosliSpravuCezSocket(adresat, obsahSpravy);
                    //posliSpravu(adresat, obsahSpravy);
                    std::cout << "pridal som spravu do pola a zavolal pthread cond\n";
                } else {
                    zoznamKtorymSaNepodariloPoslat+= adresat + "\n";
                }

            }
        }


        return true;
    }

    //TODO pozreli sme
    void odosliSpravuCezSocket(std::string menoUzivatela, std::string obsahSpravy) {
        std::string obsahSpravyZasifrovany = obsahSpravy;
        Hash::zasifrujSpravu(obsahSpravyZasifrovany);

        int socket = -1;
        pthread_mutex_lock(&this->mutexData);
        for(int i = 0; i < prihlaseni.size(); i++) {
            if(*(prihlaseni[i]->getPouzivatel()->getMeno()) == menoUzivatela) {
                socket = prihlaseni[i]->getSocket();
                break;
            }
        }
        if(socket == -1) {
            Pouzivatel* pouzivatel = pouzivatelia.find(menoUzivatela)->second;
            //pridaj do zoznamu sprav ktore sa maju zobrazit uzivatelovi ked sa prihlasi
            pouzivatel->pridajNeprecitanuSpravu(obsahSpravy);
            pthread_mutex_unlock(&this->mutexData);
            return;
        }
        pthread_mutex_t* mutexSocketu = this->dajMutexSocketu(socket);
        pthread_mutex_lock(mutexSocketu);
        pthread_mutex_unlock(&this->mutexData);

        std::cout << "Odoslal som spravu\n";
        std::cout << "Socket: " << std::to_string(socket);
        send(socket, obsahSpravyZasifrovany.c_str(), obsahSpravyZasifrovany.size(), 0);

        pthread_mutex_unlock(mutexSocketu);
    }






    //TODO pozreteeeeeeee
    Pouzivatel* prihlas(std::string* meno, std::string* heslo, int* socket, std::string& dovodPrecoProblem) {

        pthread_mutex_lock(&this->mutexData);

        auto pouzivatelIterator = this->pouzivatelia.find(*meno);

        if (pouzivatelIterator != this->pouzivatelia.end()) {

            for(int i =0; i < prihlaseni.size(); i++) {
                if(*(prihlaseni[i]->getPouzivatel()->getMeno()) == *meno) {
                    pthread_mutex_unlock(&this->mutexData);
                    dovodPrecoProblem = "Uz ste prihlaseny z ineho pocitaca.\n";
                    return nullptr;
                }
            }


            if(pouzivatelIterator->second->jeDobreHeslo(heslo)) {

                prihlaseni.push_back(new Prihlaseny(pouzivatelIterator->second, socket));


                pthread_mutex_unlock(&this->mutexData);

                return pouzivatelIterator->second;
            }
        }
        pthread_mutex_unlock(&this->mutexData);
        dovodPrecoProblem = "Zle pouzivatelske meno alebo heslo.\n ";
        return nullptr;
    }
    //TODO pozreli sme
    void odosliUzivateloviNeprecitaneSpravy(Pouzivatel* pouzivatel) {
        //ZMENA
        //tu by sa nikdy nemali prepisovat neprecitane spravy, lebo sa mozu len vymazat
        //a to vzdy len na konci zavolania tejto metody, takze to nie je v kritickej sekcii
        std::vector<std::string>* neprecitaneSpravy = pouzivatel->dajNeprecitaneSpravy();
        for(int i = 0; i < neprecitaneSpravy->size(); i++) {
            //ALTERNATIVA

            odosliSpravuCezSocket(*pouzivatel->getMeno(), (*neprecitaneSpravy)[i]);

        }
        //ked je prihlaseny nikto mu nemoze poslat neprecitanu spravu
        pouzivatel->vymazNeprecitaneSpravy();


    }

    //TODO POZRELI SME
    void vytvorKonverzaciu(Pouzivatel* pouzivatel, int& indexKonverzacie, std::string& nazovKonverzacie) {
        Konverzacia* konverzacia = new Konverzacia(nazovKonverzacie);

        konverzacia->pridajUcastnika(*pouzivatel->getMeno());
        pthread_mutex_lock(&this->mutexData);
        pouzivatel->getKonverzacie()->push_back(konverzacia);

        indexKonverzacie = pouzivatel->getKonverzacie()->size() -1;

        this->konverzacie.push_back(konverzacia);
        pthread_mutex_unlock(&this->mutexData);
    }


    //TODO hotovo
    bool pridajDoKonverzacie(Pouzivatel* pouzivatel, int& indexKonverzacie, int indexVPoliJehoPriatelov) {
        pthread_mutex_lock(&this->mutexData);
        if(pouzivatel->getKonverzacie()->size() <= indexKonverzacie) {
            pthread_mutex_unlock(&this->mutexData);
            return false;
        }

        if(pouzivatel->getPriatelia()->size() <= indexVPoliJehoPriatelov) {
            pthread_mutex_unlock(&this->mutexData);
            return false;
        }

        Pouzivatel* pridajTohto = (*pouzivatel->getPriatelia())[indexVPoliJehoPriatelov];
        Konverzacia* konverzacia = (*pouzivatel->getKonverzacie())[indexKonverzacie];

        pthread_mutex_unlock(&this->mutexData);


        for(int i = 0; i < konverzacia->getZoznamUcastnikov()->size(); i++) {
            if((*konverzacia->getZoznamUcastnikov())[i] ==  *pridajTohto->getMeno()) {
                return false;
            }
        }

        pthread_mutex_lock(&this->mutexData);
        pridajTohto->getKonverzacie()->push_back(konverzacia);
        konverzacia->pridajUcastnika(*(pridajTohto->getMeno()));
        pthread_mutex_unlock(&this->mutexData);

        return true;
    }
    //TODO pozreli sme
    void registruj(std::string* meno, std::string* heslo) {
        Hash::zahashujHeslo(*heslo);
        Pouzivatel* pouzivatel = new Pouzivatel(*meno, *heslo);
        pthread_mutex_lock(&this->mutexData);
        pouzivatelia.insert({*pouzivatel->getMeno(), pouzivatel});
        pthread_mutex_unlock(&this->mutexData);
    }
    //TODO pozreli sme
    bool jeMenoUnikatne(std::string& meno) {
        pthread_mutex_lock(&this->mutexData);
        auto pouzivatelIterator = this->pouzivatelia.find(meno);

        if (pouzivatelIterator != this->pouzivatelia.end()) {
            pthread_mutex_unlock(&this->mutexData);
            return false;
        }
        pthread_mutex_unlock(&this->mutexData);
        return true;
    }

    //TODO pozreli sme
    //true ak sa podarilo, false ak sa nepodarilo
    bool posliZiadostOPriatelstvo(Pouzivatel* odoslalZiadost, std::string menoKomuOdoslal) {

        pthread_mutex_lock(&this->mutexData);
        auto posliTomutoIterator = this->pouzivatelia.find(menoKomuOdoslal);


        if (posliTomutoIterator == this->pouzivatelia.end()) {
            pthread_mutex_unlock(&this->mutexData);
            return false;
        }


        if(posliTomutoIterator->second->jeVZiadostiach(odoslalZiadost)
            || posliTomutoIterator->second->uzJeVPriateloch(odoslalZiadost) ) {
            pthread_mutex_unlock(&this->mutexData);
            return false;

        }
        posliTomutoIterator->second->pridajNovuZiadost(odoslalZiadost);
        pthread_mutex_unlock(&this->mutexData);

        //poslanie spravy tom ze bola odoslana
        std::string obsahSpravy = "Nova ziadost o priatelstvo od pouzivatela: " + *odoslalZiadost->getMeno();

        //ZMENA
        this->odosliSpravuCezSocket(menoKomuOdoslal, obsahSpravy);


        return true;
    }


    /*
    void posliOdobratieZPriatelov(Pouzivatel* odoslalZiadost, Pouzivatel* chceOdobratTohto) {
        //poslanie spravy uzivatelovi -> chce odobrat tohto
        std::string obsahSpravy = "Uzivatel " + *odoslalZiadost->getMeno() + "si Vas odobral zo svojich priatelov\n";
        this->posliSpravu(*chceOdobratTohto->getMeno(), obsahSpravy);



        odoberZPriatelov(odoslalZiadost, chceOdobratTohto);

    }*/
    //TODO POZRELI SME
    std::string dajZoznamZiadostiOPriatelstvo(Pouzivatel* pouzivatel) {
        std::string vrat = "";
        pthread_mutex_lock(&this->mutexData);
        for(int i = 0; i < pouzivatel->getZiadostiOPriatelstvo()->size(); i++) {
            vrat += "["  + std::to_string(i) + "]" + *(*pouzivatel->getZiadostiOPriatelstvo())[i]->getMeno() + "\n";
        }
        pthread_mutex_unlock(&this->mutexData);
        return vrat;
    }



    //vrati true, ak prebehlo vporiadku
    //TODO pozreli sme
    bool spracujZiadostOPriatelstvo(Pouzivatel* pouzivatel,int index,bool prijal) {
        pthread_mutex_lock(&this->mutexData);
        if(pouzivatel->getZiadostiOPriatelstvo()->size() <= index) {
            pthread_mutex_unlock(&this->mutexData);
            return false;
        }

        Pouzivatel* pridajTohto = pouzivatel->vymazZoZiadostiOPriatelstvo(index);

        if(prijal) {
            pridajDoPriatelov(pouzivatel, pridajTohto);
            pthread_mutex_unlock(&this->mutexData);
            //posli spravu ze prial
            std::string obsahSpravy = "Pouzivatel " + *pouzivatel->getMeno() + " prijal Vasu ziadost o priatelstvo.\n";
            this->odosliSpravuCezSocket(*pridajTohto->getMeno(), obsahSpravy);



        } else {
            pthread_mutex_unlock(&this->mutexData);
            //posli spravu ze neprial
            std::string obsahSpravy = "Pouzivatel " + *pouzivatel->getMeno() + " odmietol Vasu ziadost o priatelstvo.\n";

            //ZMENA
            this->odosliSpravuCezSocket(*pridajTohto->getMeno(), obsahSpravy);

        }

        return true;
    }


    //TODO pozreli sme
    std::string dajZoznamPriatelov(Pouzivatel* pouzivatel) {

        std::string vrat = "";
        pthread_mutex_lock(&this->mutexData);
        for(int i = 0; i < pouzivatel->getPriatelia()->size(); i++) {
            vrat += "["  + std::to_string(i) + "]" + *(*pouzivatel->getPriatelia())[i]->getMeno() + "\n";
        }
        pthread_mutex_unlock(&this->mutexData);
        return vrat;
    }
    //TODO pozreli sme
    std::string dajZoznamKonverzaciiPouzivatela(Pouzivatel* pouzivatel) {
        std::string vrat = "";
        pthread_mutex_lock(&this->mutexData);
        for(int i = 0; i < pouzivatel->getKonverzacie()->size(); i++) {
            vrat += "["  + std::to_string(i) + "]" + *(*pouzivatel->getKonverzacie())[i]->getNazov() + "\n";
            vrat+="   Ucastnici: \n";
            for(int j = 0; j < (*pouzivatel->getKonverzacie())[i]->getZoznamUcastnikov()->size(); j++) {
                vrat+= (*(*pouzivatel->getKonverzacie())[i]->getZoznamUcastnikov())[j] + ", ";
            }
            vrat+="\n";
        }
        pthread_mutex_unlock(&this->mutexData);
        return vrat;
    }

    //vracia true ak prebehlo OK
    //TODO pozreli sme
    bool spracujOdobratieZPriatelov(Pouzivatel* pouzivatel,int index, std::string dovod = "") {
        pthread_mutex_lock(&this->mutexData);
        if(pouzivatel->getPriatelia()->size() <= index) {
            pthread_mutex_unlock(&this->mutexData);
            return false;
        }

        Pouzivatel* odoberTohto = (*pouzivatel->getPriatelia())[index];
        this->odoberZPriatelov(pouzivatel, odoberTohto);
        pthread_mutex_unlock(&this->mutexData);

        //posli spravu ze odobral
        std::string obsahSpravy = "Uzivatel " + *pouzivatel->getMeno() + " si Vas odobral zo svojich priatelov " + dovod + "\n";


        this->odosliSpravuCezSocket(*odoberTohto->getMeno(), obsahSpravy);


        return true;
    }

    //TODO hotovo
    void odstranPouzivateloviVsetkychPriatelov(Pouzivatel* pouzivatel) {

        int pocetNaOdobratie = pouzivatel->getPriatelia()->size();

        for(int i = 0; i < pocetNaOdobratie; i++) {
            spracujOdobratieZPriatelov(pouzivatel, 0, "z dovodu zrusenia uctu");
        }

    }
    //TODO hotovo
    void odstranUcet(Pouzivatel* pouzivatel) {
        pthread_mutex_lock(&mutexData);

        pouzivatelia.erase(*pouzivatel->getMeno());
        pthread_mutex_unlock(&mutexData);

        delete pouzivatel;
    }

    //TODO hotovo
    void odhlasPouzivatela(Pouzivatel* odhlasMna) {
        pthread_mutex_lock(&mutexData);
        for(int i = 0; i < this->prihlaseni.size(); i++) {
            if(this->prihlaseni[i]->getPouzivatel() == odhlasMna) {
                Prihlaseny* prihlaseny = this->prihlaseni[i];
                delete prihlaseny;

                this->prihlaseni.erase(this->prihlaseni.begin() + i);
               break;
            }
        }
        pthread_mutex_unlock(&mutexData);
    }



    void ulozVsetko() {
        this->zapisPouzivatelov();
        this->zapisPriatelovPouzivatelov();
        this->zapisZiadostiOPriatelstvoPouzivatelov();
        this->zapisNeprecitaneSpravyPoouzivatelov();
    }

    void nacitajVsetko() {
        this->nacitajPouzivatelov();
        this->nacitajPriatelovPouzivatelov();
        this->nacitajZiadostiOPriatelstvoPouzivatelov();
        this->nacitajNeprecitaneSpravyPoouzivatelov();
    }



private:
    //TODO pozreli sme - POZOR pri volani musi byt zamknuty
    void pridajDoPriatelov(Pouzivatel* pouzivatel1, Pouzivatel* pouzivatel2) {
        pouzivatel1->pridajDoPriatelov(pouzivatel2);
        pouzivatel2->pridajDoPriatelov(pouzivatel1);
    }
    //TODO pozreli sme -  POZOR pri volani musi byt zamknuty
    void odoberZPriatelov(Pouzivatel* pouzivatel1, Pouzivatel* pouzivatel2) {
        pouzivatel1->odoberPriatela(pouzivatel2);
        pouzivatel2->odoberPriatela(pouzivatel1);
    }

    void zapisPouzivatelov() {
        std::string stringPouzivatelia;
        for (auto pouzivatelIterator : this->pouzivatelia) {
            stringPouzivatelia += pouzivatelIterator.second->toString();
        }

        Zapisovac::zapisPozivatelov(stringPouzivatelia);
    }

    void zapisPriatelovPouzivatelov() {
        std::string stringPriatelia;

        for (auto pouzivatelIterator : this->pouzivatelia) {

            stringPriatelia += pouzivatelIterator.first + "\n";
            std::vector<Pouzivatel*>* priatelia = pouzivatelIterator.second->getPriatelia();

            for (int i = 0; i < priatelia->size(); ++i) {
                stringPriatelia += *(*priatelia)[i]->getMeno() + "\n";
            }

            stringPriatelia += "<np>\n";
        }

        Zapisovac::zapisPriatelovPouzivatelov(stringPriatelia);
    }

    void zapisZiadostiOPriatelstvoPouzivatelov() {
        std::string stringZiadosti;

        for (auto pouzivatelIterator : this->pouzivatelia) {

            stringZiadosti += pouzivatelIterator.first + "\n";
            std::vector<Pouzivatel*>* ziadosti = pouzivatelIterator.second->getZiadostiOPriatelstvo();

            for (int i = 0; i < ziadosti->size(); ++i) {
                stringZiadosti += *(*ziadosti)[i]->getMeno() + "\n";
            }

            stringZiadosti += "<np>\n";
        }

        Zapisovac::zapisZiadostiOPriatelstvoPouzivatelov(stringZiadosti);
    }

    void zapisNeprecitaneSpravyPoouzivatelov() {
        std::string stringSpravy;

        for (auto pouzivatelIterator : this->pouzivatelia) {

            stringSpravy += pouzivatelIterator.first + "\n";
            std::vector<std::string>* neprecitaneSpravy = pouzivatelIterator.second->dajNeprecitaneSpravy();

            for (int i = 0; i < neprecitaneSpravy->size(); ++i) {
                stringSpravy += (*neprecitaneSpravy)[i] + "\n";

                stringSpravy += "<ns>\n";
            }
            stringSpravy += "<np>\n";
        }

        Zapisovac::zapisNeprecitanychSpravPouzivatelov(stringSpravy);
    }

    void nacitajPouzivatelov() {
        std::string stringPouzivatelia;
        Nacitavac::nacitajPozivatelov(stringPouzivatelia);

        std::istringstream stringStream(stringPouzivatelia);

        std::string meno;
        std::string heslo;

        while (std::getline(stringStream, meno)) {
            std::getline(stringStream, heslo);

            pouzivatelia.insert({meno, new Pouzivatel(meno, heslo)});
        }
    }

    void nacitajPriatelovPouzivatelov() {
        std::string stringPriatelia;
        Nacitavac::nacitajPriatelovPouzivatelov(stringPriatelia);

        std::istringstream stringStream(stringPriatelia);

        std::string meno;
        Pouzivatel* pouzivatel = nullptr;
        bool trebaZmenitPouzivatela = true;

        while (std::getline(stringStream, meno)) {

            if (meno == "<np>") {
                trebaZmenitPouzivatela = true;
                continue;
            }

            auto pouzivatelIterator = pouzivatelia.find(meno);

            if (trebaZmenitPouzivatela) {
                pouzivatel = pouzivatelIterator->second;
                trebaZmenitPouzivatela = false;
                continue;
            }

            pouzivatel->pridajDoPriatelov(pouzivatelIterator->second);
        }
    }

    void nacitajZiadostiOPriatelstvoPouzivatelov() {
        std::string stringZiadosti;
        Nacitavac::nacitajZiadostiOPriatelstvoPouzivatelov(stringZiadosti);

        std::istringstream stringStream(stringZiadosti);

        std::string meno;
        Pouzivatel* pouzivatel = nullptr;
        bool trebaZmenitPouzivatela = true;

        while (std::getline(stringStream, meno)) {

            if (meno == "<np>") {
                trebaZmenitPouzivatela = true;
                continue;
            }

            auto pouzivatelIterator = pouzivatelia.find(meno);

            if (trebaZmenitPouzivatela) {
                pouzivatel = pouzivatelIterator->second;
                trebaZmenitPouzivatela = false;
                continue;
            }

            pouzivatel->pridajNovuZiadost(pouzivatelIterator->second);
        }
    }

    void nacitajNeprecitaneSpravyPoouzivatelov() {
        std::string stringNeprecitaneSpravy;
        Nacitavac::nacitajNeprecitaneSpravyPouzivatelov(stringNeprecitaneSpravy);

        std::istringstream stringStream(stringNeprecitaneSpravy);

        std::string riadok;
        std::string sprava;
        Pouzivatel* pouzivatel = nullptr;
        bool trebaZmenitPouzivatela = true;

        while (std::getline(stringStream, riadok)) {

            if (riadok == "<np>") {
                trebaZmenitPouzivatela = true;
                continue;
            } else if (riadok == "<ns>") {
                pouzivatel->pridajNeprecitanuSpravu(sprava);
                sprava = "";
                continue;
            }

            auto pouzivatelIterator = pouzivatelia.find(riadok);

            if (trebaZmenitPouzivatela) {
                pouzivatel = pouzivatelIterator->second;
                trebaZmenitPouzivatela = false;
                continue;
            }

            sprava += riadok + "\n";
        }
    }

};

