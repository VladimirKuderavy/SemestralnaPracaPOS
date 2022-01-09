//
// Created by Vlado OMEN-PC on 9. 1. 2022.
//

#include "Nacitavac.h"

void Nacitavac::nacitajPozivatelov(std::string& pouzivatelia) {
    std::ifstream subor(Konstanty::getNazovSuboruPouzivatelov());

    std::string riadok;

    while (std::getline (subor, riadok)) {
        pouzivatelia += riadok + "\n";
    }

    subor.close();
}

void Nacitavac::nacitajPriatelovPouzivatelov(std::string& priateliaPouzivatelov) {
    std::ifstream subor(Konstanty::getNazovSuboruPriatelov());

    std::string riadok;

    while (std::getline (subor, riadok)) {
        priateliaPouzivatelov += riadok + "\n";
    }

    subor.close();
}

void Nacitavac::nacitajZiadostiOPriatelstvoPouzivatelov(std::string& ziadostiOPriatelstvo) {
    std::ifstream subor(Konstanty::getNazovSuboruZiadosti());

    std::string riadok;

    while (std::getline (subor, riadok)) {
        ziadostiOPriatelstvo += riadok + "\n";
    }

    subor.close();
}

void Nacitavac::nacitajNeprecitaneSpravyPouzivatelov(std::string& neprecitaneSpravy) {
    std::ifstream subor(Konstanty::getNazovSuboruNeprecitanychSprav());

    std::string riadok;

    while (std::getline (subor, riadok)) {
        neprecitaneSpravy += riadok + "\n";
    }

    subor.close();
}

