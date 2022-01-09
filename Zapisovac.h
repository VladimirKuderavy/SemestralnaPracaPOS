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
    static void zapisPozivatelov(std::string& pouzivatelia);

    static void zapisPriatelovPouzivatelov(std::string& priateliaPouzivatelov);

    static void zapisZiadostiOPriatelstvoPouzivatelov(std::string& ziadostiOPriatelstvo);

    static void zapisNeprecitanychSpravPouzivatelov(std::string& neprecitaneSpravy);
};
