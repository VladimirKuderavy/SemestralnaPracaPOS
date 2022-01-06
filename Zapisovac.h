//
// Created by Vlado OMEN-PC on 6. 1. 2022.
//
#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include "Konstanty.h"

class Zapisovac {


public:
    static void zapisPozivatelov(std::string& pouzivatelia) {
        std::ofstream subor(Konstanty::getNazovSuboruPouzivatelov());

        subor << pouzivatelia;

        subor.close();
    }

    static void zapisPriatelovPouzivatelov(std::string& priateliaPouzivatelov) {
        std::ofstream subor(Konstanty::getNazovSuboruPriatelov());

        subor << priateliaPouzivatelov;

        subor.close();
    }

    static void zapisZiadostiOPriatelstvoPouzivatelov(std::string& ziadostiOPriatelstvo) {
        std::ofstream subor(Konstanty::getNazovSuboruZiadosti());

        subor << ziadostiOPriatelstvo;

        subor.close();
    }

    static void zapisNeprecitanychSpravPouzivatelov(std::string& neprecitaneSpravy) {
        std::ofstream subor(Konstanty::getNazovSuboruNeprecitanychSprav());

        subor << neprecitaneSpravy;

        subor.close();
    }
};
