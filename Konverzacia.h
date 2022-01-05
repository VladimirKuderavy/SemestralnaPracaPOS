#pragma once
#include <vector>
#include <string>


class Konverzacia {
private:

    std::vector<std::string> ucastnici;
    std::string nazovKonverzacie;
public:
    Konverzacia(std::string& nazovKonverzacie) {
        this->nazovKonverzacie = nazovKonverzacie;
    }

    void pridajUcastnika(std::string pouzivatel) {
        ucastnici.push_back(pouzivatel);
    }
    std::string* getNazov() {
        return &this->nazovKonverzacie;
    }
    std::vector<std::string>* getZoznamUcastnikov() {
        return &this->ucastnici;
    }

};

