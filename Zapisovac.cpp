//
// Created by Vlado OMEN-PC on 9. 1. 2022.
//

#include "Zapisovac.h"

void Zapisovac::zapisPozivatelov(std::string& pouzivatelia) {
    std::ofstream subor(Konstanty::getNazovSuboruPouzivatelov());

    subor << pouzivatelia;

    subor.close();
}

void Zapisovac::zapisPriatelovPouzivatelov(std::string& priateliaPouzivatelov) {
    std::ofstream subor(Konstanty::getNazovSuboruPriatelov());

    subor << priateliaPouzivatelov;

    subor.close();
}

void Zapisovac::zapisZiadostiOPriatelstvoPouzivatelov(std::string& ziadostiOPriatelstvo) {
    std::ofstream subor(Konstanty::getNazovSuboruZiadosti());

    subor << ziadostiOPriatelstvo;

    subor.close();
}

void Zapisovac::zapisNeprecitanychSpravPouzivatelov(std::string& neprecitaneSpravy) {
    std::ofstream subor(Konstanty::getNazovSuboruNeprecitanychSprav());

    subor << neprecitaneSpravy;

    subor.close();
}