//
// Created by Vlado OMEN-PC on 5. 1. 2022.
//
#pragma once
#include <string>

#define PORT 6469
#define POCET_KLIENTOV 2

class Konstanty {

public:
    static std::string getNazovSuboruPouzivatelov() {
        return "pouzivatelia.txt";
    }

    static std::string getNazovSuboruPriatelov() {
        return "priatelia.txt";
    }

    static std::string getNazovSuboruNeprecitanychSprav() {
        return "spravy.txt";
    }

    static std::string getNazovSuboruZiadosti() {
        return "ziadosti.txt";;
    }
};

