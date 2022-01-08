//
// Created by Vlado OMEN-PC on 7. 1. 2022.
//
#pragma once
#include "string"

#define MESSAGE_HASH 8
#define SEED_SIZE 30
#define HASH_RANGE 30

class Hash {
private:
    static int seeds[SEED_SIZE];

public:
    static int* getSeeds() {
        return Hash::seeds;
    }

    static void zasifrujSpravu(std::string& sprava);

    static void odsifrujSpravu(std::string& zasifrovanaSprava);

    static void odsifrujSpravu(char* retazec, size_t pocetZnakov);

    static void zahashujHeslo(std::string& heslo);

    static bool overHeslo(std::string& zasifrovaneHeslo, std::string& hesloNaOverenie);

};
