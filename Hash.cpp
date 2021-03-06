//
// Created by Vlado OMEN-PC on 8. 1. 2022.
//

#include "Hash.h"

int Hash::seeds[] {
        97662, 18316, 50540, 65009, 62938, 35921, 38567, 58927, 89785, 7009,
        9298, 93552, 39239, 96460, 10000, 18001, 13625, 50711, 45306, 44925,
        15412, 34985, 17469, 22234, 44003, 7520, 73656, 97731, 5795, 45310
};

void Hash::zasifrujSpravu(std::string& sprava) {
    int hash = MESSAGE_HASH;

    for (int i = 0; i < sprava.size(); ++i) {
        sprava[i] += hash;
    }
}

void Hash::odsifrujSpravu(std::string& zasifrovanaSprava) {
    int hash = MESSAGE_HASH;

    for (int i = 0; i < zasifrovanaSprava.size(); ++i) {
        zasifrovanaSprava[i] -= hash;
    }
}

void Hash::odsifrujSpravu(char* retazec, size_t pocetZnakov) {
    int hash = MESSAGE_HASH;

    for (int i = 0; i < pocetZnakov; ++i) {
        retazec[i] -= hash;
    }
}

void Hash::zahashujHeslo(std::string& heslo) {
    srand(time(nullptr));
    srand((Hash::getSeeds())[rand() % SEED_SIZE]);
    int hash = (rand() % HASH_RANGE) - (HASH_RANGE / 2);

    while (hash == 0) {
        hash = (rand() % HASH_RANGE) - (HASH_RANGE / 2);
    }

    for (int i = 0; i < heslo.size(); ++i) {
        heslo[i] += hash;
    }
}

bool Hash::overHeslo(std::string& zasifrovaneHeslo, std::string& hesloNaOverenie) {

    for (int i = 0; i < SEED_SIZE; ++i) {
        std::string hladaneHeslo = hesloNaOverenie;

        int hash = i - (HASH_RANGE / 2);

        for (int j = 0; j < hesloNaOverenie.size(); ++j) {
            hladaneHeslo[j] += hash;
        }

        if (zasifrovaneHeslo == hladaneHeslo) {
            return true;
        }
    }

    return false;
}
