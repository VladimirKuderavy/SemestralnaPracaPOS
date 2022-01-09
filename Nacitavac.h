//
// Created by Vlado OMEN-PC on 6. 1. 2022.
//
#pragma once


#include "Konstanty.h"
#include <fstream>

class Nacitavac {

public:
    static void nacitajPozivatelov(std::string& pouzivatelia);

    static void nacitajPriatelovPouzivatelov(std::string& priateliaPouzivatelov);

    static void nacitajZiadostiOPriatelstvoPouzivatelov(std::string& ziadostiOPriatelstvo);

    static void nacitajNeprecitaneSpravyPouzivatelov(std::string& neprecitaneSpravy);
};
