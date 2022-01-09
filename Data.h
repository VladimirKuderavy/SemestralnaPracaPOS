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
    Data ();

    ~Data();

    pthread_mutex_t* getMutex() {
        return &this->mutexData;
    }

    void pridajOtvorenySocket(SocketAMutex* socketAMutex);

    void vymazOtvorenySocket(int socket);

    void zatvorVsetkyOtvoreneSockety();

    pthread_mutex_t* dajMutexSocketu(int& socket);

    void odosliSuborCezSocket(std::string& menoUzivatela, std::string hlavickaSuboru, std::string obsahSuboru);

    bool posliSubor(Pouzivatel* pouzivatel ,int indexKonverzacie ,std::string& nazovSuboru,std::string& obsahSuboru,std::string& zoznamKtorymSaNepodariloPoslat);

    bool posliSpravu(Pouzivatel* pouzivatel, int indexKonverzacie, std::string& obsahSpravy, std::string& zoznamKtorymSaNepodariloPoslat);

    void odosliSpravuCezSocket(std::string menoUzivatela, std::string obsahSpravy);

    Pouzivatel* prihlas(std::string* meno, std::string* heslo, int* socket, std::string& dovodPrecoProblem);

    void odosliUzivateloviNeprecitaneSpravy(Pouzivatel* pouzivatel);

    void vytvorKonverzaciu(Pouzivatel* pouzivatel, int& indexKonverzacie, std::string& nazovKonverzacie);

    bool pridajDoKonverzacie(Pouzivatel* pouzivatel, int& indexKonverzacie, int indexVPoliJehoPriatelov);

    void registruj(std::string* meno, std::string* heslo);

    bool jeMenoUnikatne(std::string& meno);

    bool posliZiadostOPriatelstvo(Pouzivatel* odoslalZiadost, std::string menoKomuOdoslal);

    std::string dajZoznamZiadostiOPriatelstvo(Pouzivatel* pouzivatel);

    bool spracujZiadostOPriatelstvo(Pouzivatel* pouzivatel,int index,bool prijal);

    std::string dajZoznamPriatelov(Pouzivatel* pouzivatel);

    std::string dajZoznamKonverzaciiPouzivatela(Pouzivatel* pouzivatel);

    bool spracujOdobratieZPriatelov(Pouzivatel* pouzivatel,int index, std::string dovod = "");

    void odstranPouzivateloviVsetkychPriatelov(Pouzivatel* pouzivatel);

    void odstranUcet(Pouzivatel* pouzivatel);

    void odhlasPouzivatela(Pouzivatel* odhlasMna);

    void ulozVsetko();

    void nacitajVsetko();

private:
    void pridajDoPriatelov(Pouzivatel* pouzivatel1, Pouzivatel* pouzivatel2);

    void odoberZPriatelov(Pouzivatel* pouzivatel1, Pouzivatel* pouzivatel2);

    void zapisPouzivatelov();

    void zapisPriatelovPouzivatelov();

    void zapisZiadostiOPriatelstvoPouzivatelov();

    void zapisNeprecitaneSpravyPoouzivatelov();

    void nacitajPouzivatelov();

    void nacitajPriatelovPouzivatelov();

    void nacitajZiadostiOPriatelstvoPouzivatelov();

    void nacitajNeprecitaneSpravyPoouzivatelov();

};

