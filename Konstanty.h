//
// Created by Vlado OMEN-PC on 5. 1. 2022.
//
#pragma once
#include <string>

#define PORT 6461
#define POCET_KLIENTOV 2

class Konstanty {

public:
    static std::string getNazovSuboruPouzivatelov() {
        return "/tmp/tmp.bnEpdsMGjj/pouzivatelia.txt";
    }

    static std::string getNazovSuboruPriatelov() {
        return "/tmp/tmp.bnEpdsMGjj/priatelia.txt";
    }

    static std::string getNazovSuboruNeprecitanychSprav() {
        return "/tmp/tmp.bnEpdsMGjj/spravy.txt";
    }

    static std::string getNazovSuboruZiadosti() {
        return "/tmp/tmp.bnEpdsMGjj/ziadosti.txt";
    }

    static std::string getTextVstupDoModuNacitajSubor() {
        return "<#nacitajSubor#>";
    }


    static std::string getTextSuborSaNepodariloNacitat() {
        return "<#nenacitalSa#>";
    }

    static std::string getTextUlozSubor() {
        return "<#ulozSubor#>";
    }

    static std::string getRelativnaCesta() {
        return "/tmp/tmp.bnEpdsMGjj/clientPriecinok/";
    }
};

