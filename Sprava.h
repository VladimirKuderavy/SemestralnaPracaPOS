#pragma once
#include <string>


class Sprava {

private:
    std::string adresat;
    std::string obsah;
public:
    Sprava(std::string& adresat, std::string& obsah) {
        this->adresat = adresat;
        this->obsah = obsah;
    }

    std::string getAdresat() {
        return this->adresat;
    }

    std::string getObsah() {
        return this->obsah;
    }

};


