//
// Created by Andrej on 4. 1. 2022.
//

#include "Data.h"

Data::Data () {
    pthread_mutex_init(&mutexData, NULL);
}

Data::~Data() {
    for(int i = 0; i < prihlaseni.size(); i++) {
        delete prihlaseni[i];
    }
    for (auto pouzivatel : this->pouzivatelia) {
        delete pouzivatel.second;
    }

    for(int i = 0; i < konverzacie.size(); i++) {
        delete konverzacie[i];
    }
    pthread_mutex_destroy(&mutexData);
}

//POZRETE
void Data::pridajOtvorenySocket(SocketAMutex* socketAMutex) {

    pthread_mutex_lock(&mutexData);
    this->otvoreneSocketyAMutexy.push_back(socketAMutex);
    pthread_mutex_unlock(&mutexData);
}

//POZRETE
void Data::vymazOtvorenySocket(int socket) {
    pthread_mutex_lock(&mutexData);
    for (int i = 0; i < this->otvoreneSocketyAMutexy.size(); i++) {
        if(*otvoreneSocketyAMutexy[i]->getSocket() == socket) {

            pthread_mutex_t* mutexSocketu = otvoreneSocketyAMutexy[i]->getMutex();
            int socket = *otvoreneSocketyAMutexy[i]->getSocket();
            pthread_mutex_lock(mutexSocketu);
            //lock otestuje, ci je volny
            pthread_mutex_unlock(mutexSocketu);

            pthread_mutex_destroy(mutexSocketu);
            close(socket);
            delete otvoreneSocketyAMutexy[i];

            otvoreneSocketyAMutexy.erase(otvoreneSocketyAMutexy.begin() + i);
            break;
        }
    }
    pthread_mutex_unlock(&mutexData);
}


//POZRETE
void Data::zatvorVsetkyOtvoreneSockety() {
    //Toto sa vola az v maine, ked budu vsetky vlakna ukoncene
    for(int i = 0; i < this->otvoreneSocketyAMutexy.size(); i++) {
        close(*this->otvoreneSocketyAMutexy[i]->getSocket());
        if(this->otvoreneSocketyAMutexy[i]->getMutex() != nullptr) {
            pthread_mutex_destroy(this->otvoreneSocketyAMutexy[i]->getMutex());
        }
        delete this->otvoreneSocketyAMutexy[i];
    }
}
//toto treba volat jedine so zamknutym socketom
pthread_mutex_t* Data::dajMutexSocketu(int& socket) {
    for(int i = 0; i < this->otvoreneSocketyAMutexy.size(); i++) {
        if(*otvoreneSocketyAMutexy[i]->getSocket() == socket) {
            return otvoreneSocketyAMutexy[i]->getMutex();
        }
    }
    return nullptr;
}

//POZRETE
//tunak treba posielat kopiu lebo sa to vola v cykle a keby nebola kopia, tak sa to zasifruje niekolkokrat
void Data::odosliSuborCezSocket(std::string& menoUzivatela, std::string hlavickaSuboru, std::string obsahSuboru) {
    int socket = -1;
    Hash::zasifrujSpravu(obsahSuboru);
    Hash::zasifrujSpravu(hlavickaSuboru);

    pthread_mutex_lock(&this->mutexData);

    for(int i = 0; i < prihlaseni.size(); i++) {
        if(*(prihlaseni[i]->getPouzivatel()->getMeno()) == menoUzivatela) {
            socket = prihlaseni[i]->getSocket();
            break;
        }
    }
    if(socket == -1) {
        Pouzivatel* pouzivatel = pouzivatelia.find(menoUzivatela)->second;
        //pridaj do zoznamu sprav ktore sa maju zobrazit uzivatelovi ked sa prihlasi
        std::string obsahSpravy = "V case Vasej nedostupnosti sa Vam niekto snazil poslat subor:\n";
        pouzivatel->pridajNeprecitanuSpravu(obsahSpravy);
        pthread_mutex_unlock(&this->mutexData);
        return;
    }
    pthread_mutex_t* mutexSocketu = this->dajMutexSocketu(socket);
    pthread_mutex_lock(mutexSocketu);
    pthread_mutex_unlock(&this->mutexData);

    //POSLANIE SUBORU
    send(socket, hlavickaSuboru.c_str(), hlavickaSuboru.size(), 0);

    int poslanePo = 0;

    while(poslanePo < obsahSuboru.size()) {
        int hornaHranica = 4096;
        if(poslanePo + 4096 > obsahSuboru.size()) {
            hornaHranica = obsahSuboru.size() - poslanePo;
        }

        std::string poslat = obsahSuboru.substr(poslanePo, hornaHranica);

        send(socket, poslat.c_str(), poslat.size(), 0);


        poslanePo += hornaHranica;
    }
    pthread_mutex_unlock(mutexSocketu);

}

//POZRETE
bool Data::posliSubor(Pouzivatel* pouzivatel ,int indexKonverzacie ,std::string& nazovSuboru,std::string& obsahSuboru,std::string& zoznamKtorymSaNepodariloPoslat) {
    pthread_mutex_lock(&this->mutexData);
    if(pouzivatel->getKonverzacie()->size() <= indexKonverzacie ) {
        pthread_mutex_unlock(&this->mutexData);
        return false;
    }

    Konverzacia* konverzacia = (*pouzivatel->getKonverzacie())[indexKonverzacie];
    pthread_mutex_unlock(&this->mutexData);

    std::string hlavickaSuboru = Konstanty::getTextUlozSubor() + "\n";
    hlavickaSuboru += "Pouzivatel " + *pouzivatel->getMeno() + " Vam posiela subor: " +
                      nazovSuboru + " v konverzacii: " + *konverzacia->getNazov() + "\n";
    hlavickaSuboru += nazovSuboru + "\n";
    hlavickaSuboru += std::to_string(obsahSuboru.size()) + "\n";

    for(int i = 0; i < konverzacia->getZoznamUcastnikov()->size(); i++) {
        std::string adresat = (*konverzacia->getZoznamUcastnikov())[i];
        if(adresat != *pouzivatel->getMeno()) {
            //kontrola ci ho ma odosielatel vo svojich priateloch, ak nie, tak to nieco vrati
            bool jeVPriateloch = false;



            pthread_mutex_lock(&this->mutexData);

            for(int j = 0; j < pouzivatel->getPriatelia()->size(); j++) {
                if(*(*pouzivatel->getPriatelia())[j]->getMeno() == adresat) {
                    jeVPriateloch = true;
                }
            }
            pthread_mutex_unlock(&this->mutexData);

            if(jeVPriateloch) {
                odosliSuborCezSocket(adresat, hlavickaSuboru, obsahSuboru);

            } else {
                zoznamKtorymSaNepodariloPoslat += adresat + "\n";
            }

        }
    }

    return true;
}


//POZRETE
bool Data::posliSpravu(Pouzivatel* pouzivatel, int indexKonverzacie, std::string& obsahSpravy, std::string& zoznamKtorymSaNepodariloPoslat) {
    //prejde vsetkych pouzivatelov danej konverzacie, pre kazdeho vytvori spravu a odosle ju
    pthread_mutex_lock(&this->mutexData);
    if(pouzivatel->getKonverzacie()->size() <= indexKonverzacie ) {
        pthread_mutex_unlock(&this->mutexData);
        return false;
    }

    Konverzacia* konverzacia = (*pouzivatel->getKonverzacie())[indexKonverzacie];
    pthread_mutex_unlock(&this->mutexData);

    std::string hlavickaSpravy = "Nova sprava: " ;
    hlavickaSpravy += "Od pouzivatela: " + *pouzivatel->getMeno() + "\n";
    hlavickaSpravy += "v konverzacii: " + *konverzacia->getNazov() + "\n";

    hlavickaSpravy += "Napisal Vam: " + obsahSpravy + "\n";
    obsahSpravy = hlavickaSpravy;

    for(int i = 0; i < konverzacia->getZoznamUcastnikov()->size(); i++) {
        std::string adresat = (*konverzacia->getZoznamUcastnikov())[i];

        if(adresat != *pouzivatel->getMeno()) {
            //kontrola ci ho ma odosielatel vo svojich priateloch, ak nie, tak to nieco vrati
            bool jeVPriateloch = false;

            pthread_mutex_lock(&this->mutexData);
            for(int j = 0; j < pouzivatel->getPriatelia()->size(); j++) {
                if(*(*pouzivatel->getPriatelia())[j]->getMeno() == adresat) {
                    jeVPriateloch = true;
                }
            }
            pthread_mutex_unlock(&this->mutexData);

            if(jeVPriateloch) {
                odosliSpravuCezSocket(adresat, obsahSpravy);
            } else {
                zoznamKtorymSaNepodariloPoslat+= adresat + "\n";
            }

        }
    }

    return true;
}

//POZRETE
void Data::odosliSpravuCezSocket(std::string menoUzivatela, std::string obsahSpravy) {
    std::string obsahSpravyZasifrovany = obsahSpravy;
    Hash::zasifrujSpravu(obsahSpravyZasifrovany);

    int socket = -1;
    pthread_mutex_lock(&this->mutexData);
    for(int i = 0; i < prihlaseni.size(); i++) {
        if(*(prihlaseni[i]->getPouzivatel()->getMeno()) == menoUzivatela) {
            socket = prihlaseni[i]->getSocket();
            break;
        }
    }
    if(socket == -1) {
        Pouzivatel* pouzivatel = pouzivatelia.find(menoUzivatela)->second;
        //pridaj do zoznamu sprav ktore sa maju zobrazit uzivatelovi ked sa prihlasi
        pouzivatel->pridajNeprecitanuSpravu(obsahSpravy);
        pthread_mutex_unlock(&this->mutexData);
        return;
    }
    pthread_mutex_t* mutexSocketu = this->dajMutexSocketu(socket);
    pthread_mutex_lock(mutexSocketu);
    pthread_mutex_unlock(&this->mutexData);

/*    std::cout << "Odoslal som spravu\n";
    std::cout << "Socket: " << std::to_string(socket) << "\n";*/
    send(socket, obsahSpravyZasifrovany.c_str(), obsahSpravyZasifrovany.size(), 0);

    pthread_mutex_unlock(mutexSocketu);
}






//POZRETE
Pouzivatel* Data::prihlas(std::string* meno, std::string* heslo, int* socket, std::string& dovodPrecoProblem) {

    pthread_mutex_lock(&this->mutexData);

    auto pouzivatelIterator = this->pouzivatelia.find(*meno);

    if (pouzivatelIterator != this->pouzivatelia.end()) {

        for(int i =0; i < prihlaseni.size(); i++) {
            if(*(prihlaseni[i]->getPouzivatel()->getMeno()) == *meno) {
                pthread_mutex_unlock(&this->mutexData);
                dovodPrecoProblem = "Uz ste prihlaseny z ineho pocitaca.\n";
                return nullptr;
            }
        }


        if(pouzivatelIterator->second->jeDobreHeslo(heslo)) {

            prihlaseni.push_back(new Prihlaseny(pouzivatelIterator->second, socket));


            pthread_mutex_unlock(&this->mutexData);

            return pouzivatelIterator->second;
        }
    }
    pthread_mutex_unlock(&this->mutexData);
    dovodPrecoProblem = "Zle pouzivatelske meno alebo heslo.\n ";
    return nullptr;
}
//POZRETE
void Data::odosliUzivateloviNeprecitaneSpravy(Pouzivatel* pouzivatel) {
    //ZMENA
    //tu by sa nikdy nemali prepisovat neprecitane spravy, lebo sa mozu len vymazat
    //a to vzdy len na konci zavolania tejto metody, takze to nie je v kritickej sekcii
    std::vector<std::string>* neprecitaneSpravy = pouzivatel->dajNeprecitaneSpravy();
    for(int i = 0; i < neprecitaneSpravy->size(); i++) {
        //ALTERNATIVA

        odosliSpravuCezSocket(*pouzivatel->getMeno(), (*neprecitaneSpravy)[i]);

    }
    //ked je prihlaseny nikto mu nemoze poslat neprecitanu spravu
    pouzivatel->vymazNeprecitaneSpravy();
}

//POZRETE
void Data::vytvorKonverzaciu(Pouzivatel* pouzivatel, int& indexKonverzacie, std::string& nazovKonverzacie) {
    Konverzacia* konverzacia = new Konverzacia(nazovKonverzacie);

    konverzacia->pridajUcastnika(*pouzivatel->getMeno());
    pthread_mutex_lock(&this->mutexData);
    pouzivatel->getKonverzacie()->push_back(konverzacia);

    indexKonverzacie = pouzivatel->getKonverzacie()->size() -1;

    this->konverzacie.push_back(konverzacia);
    pthread_mutex_unlock(&this->mutexData);
}


//POZRETE
bool Data::pridajDoKonverzacie(Pouzivatel* pouzivatel, int& indexKonverzacie, int indexVPoliJehoPriatelov) {
    pthread_mutex_lock(&this->mutexData);
    if(pouzivatel->getKonverzacie()->size() <= indexKonverzacie) {
        pthread_mutex_unlock(&this->mutexData);
        return false;
    }

    if(pouzivatel->getPriatelia()->size() <= indexVPoliJehoPriatelov) {
        pthread_mutex_unlock(&this->mutexData);
        return false;
    }

    Pouzivatel* pridajTohto = (*pouzivatel->getPriatelia())[indexVPoliJehoPriatelov];
    Konverzacia* konverzacia = (*pouzivatel->getKonverzacie())[indexKonverzacie];

    pthread_mutex_unlock(&this->mutexData);


    for(int i = 0; i < konverzacia->getZoznamUcastnikov()->size(); i++) {
        if((*konverzacia->getZoznamUcastnikov())[i] ==  *pridajTohto->getMeno()) {
            return false;
        }
    }

    pthread_mutex_lock(&this->mutexData);
    pridajTohto->getKonverzacie()->push_back(konverzacia);
    konverzacia->pridajUcastnika(*(pridajTohto->getMeno()));
    pthread_mutex_unlock(&this->mutexData);

    return true;
}

//POZRETE
void Data::registruj(std::string* meno, std::string* heslo) {
    Hash::zahashujHeslo(*heslo);
    Pouzivatel* pouzivatel = new Pouzivatel(*meno, *heslo);

    pthread_mutex_lock(&this->mutexData);
    pouzivatelia.insert({*pouzivatel->getMeno(), pouzivatel});
    pthread_mutex_unlock(&this->mutexData);
}

//POZRETE
bool Data::jeMenoUnikatne(std::string& meno) {
    pthread_mutex_lock(&this->mutexData);
    auto pouzivatelIterator = this->pouzivatelia.find(meno);

    if (pouzivatelIterator != this->pouzivatelia.end()) {
        pthread_mutex_unlock(&this->mutexData);
        return false;
    }
    pthread_mutex_unlock(&this->mutexData);
    return true;
}

//POZRETE
//true ak sa podarilo, false ak sa nepodarilo
bool Data::posliZiadostOPriatelstvo(Pouzivatel* odoslalZiadost, std::string menoKomuOdoslal) {

    pthread_mutex_lock(&this->mutexData);
    auto posliTomutoIterator = this->pouzivatelia.find(menoKomuOdoslal);


    if (posliTomutoIterator == this->pouzivatelia.end()) {
        pthread_mutex_unlock(&this->mutexData);
        return false;
    }


    if(posliTomutoIterator->second->jeVZiadostiach(odoslalZiadost)
       || posliTomutoIterator->second->uzJeVPriateloch(odoslalZiadost) ) {
        pthread_mutex_unlock(&this->mutexData);
        return false;

    }
    posliTomutoIterator->second->pridajNovuZiadost(odoslalZiadost);
    pthread_mutex_unlock(&this->mutexData);

    //poslanie spravy o tom ze bola odoslana
    std::string obsahSpravy = "Nova ziadost o priatelstvo od pouzivatela: " + *odoslalZiadost->getMeno();

    this->odosliSpravuCezSocket(menoKomuOdoslal, obsahSpravy);

    return true;
}

//POZRETE
std::string Data::dajZoznamZiadostiOPriatelstvo(Pouzivatel* pouzivatel) {
    std::string vrat = "";
    pthread_mutex_lock(&this->mutexData);
    for(int i = 0; i < pouzivatel->getZiadostiOPriatelstvo()->size(); i++) {
        vrat += "["  + std::to_string(i) + "]" + *(*pouzivatel->getZiadostiOPriatelstvo())[i]->getMeno() + "\n";
    }
    pthread_mutex_unlock(&this->mutexData);
    return vrat;
}

//vrati true, ak prebehlo vporiadku
//POZRETE
bool Data::spracujZiadostOPriatelstvo(Pouzivatel* pouzivatel,int index,bool prijal) {
    pthread_mutex_lock(&this->mutexData);
    if(pouzivatel->getZiadostiOPriatelstvo()->size() <= index) {
        pthread_mutex_unlock(&this->mutexData);
        return false;
    }

    Pouzivatel* pridajTohto = pouzivatel->vymazZoZiadostiOPriatelstvo(index);

    if(prijal) {
        pridajDoPriatelov(pouzivatel, pridajTohto);
        pthread_mutex_unlock(&this->mutexData);
        //posli spravu ze prial
        std::string obsahSpravy = "Pouzivatel " + *pouzivatel->getMeno() + " prijal Vasu ziadost o priatelstvo.\n";
        this->odosliSpravuCezSocket(*pridajTohto->getMeno(), obsahSpravy);



    } else {
        pthread_mutex_unlock(&this->mutexData);
        //posli spravu ze neprial
        std::string obsahSpravy = "Pouzivatel " + *pouzivatel->getMeno() + " odmietol Vasu ziadost o priatelstvo.\n";

        //ZMENA
        this->odosliSpravuCezSocket(*pridajTohto->getMeno(), obsahSpravy);

    }

    return true;
}


//POZRETE
std::string Data::dajZoznamPriatelov(Pouzivatel* pouzivatel) {

    std::string vrat = "";
    pthread_mutex_lock(&this->mutexData);
    for(int i = 0; i < pouzivatel->getPriatelia()->size(); i++) {
        vrat += "["  + std::to_string(i) + "]" + *(*pouzivatel->getPriatelia())[i]->getMeno() + "\n";
    }
    pthread_mutex_unlock(&this->mutexData);
    return vrat;
}
//POZRETE
std::string Data::dajZoznamKonverzaciiPouzivatela(Pouzivatel* pouzivatel) {
    std::string vrat = "";

    pthread_mutex_lock(&this->mutexData);
    for(int i = 0; i < pouzivatel->getKonverzacie()->size(); i++) {
        vrat += "["  + std::to_string(i) + "]" + *(*pouzivatel->getKonverzacie())[i]->getNazov() + "\n";
        vrat+="   Ucastnici: \n";
        for(int j = 0; j < (*pouzivatel->getKonverzacie())[i]->getZoznamUcastnikov()->size(); j++) {
            vrat+= (*(*pouzivatel->getKonverzacie())[i]->getZoznamUcastnikov())[j] + ", ";
        }
        vrat+="\n";
    }
    pthread_mutex_unlock(&this->mutexData);

    return vrat;
}

//vracia true ak prebehlo OK
//POZRETE
bool Data::spracujOdobratieZPriatelov(Pouzivatel* pouzivatel,int index, std::string dovod) {
    pthread_mutex_lock(&this->mutexData);
    if(pouzivatel->getPriatelia()->size() <= index) {
        pthread_mutex_unlock(&this->mutexData);
        return false;
    }

    Pouzivatel* odoberTohto = (*pouzivatel->getPriatelia())[index];
    this->odoberZPriatelov(pouzivatel, odoberTohto);
    pthread_mutex_unlock(&this->mutexData);

    //posli spravu ze odobral
    std::string obsahSpravy = "Uzivatel " + *pouzivatel->getMeno() + " si Vas odobral zo svojich priatelov " + dovod + "\n";


    this->odosliSpravuCezSocket(*odoberTohto->getMeno(), obsahSpravy);


    return true;
}

//POZRETE
void Data::odstranPouzivateloviVsetkychPriatelov(Pouzivatel* pouzivatel) {

    int pocetNaOdobratie = pouzivatel->getPriatelia()->size();

    for(int i = 0; i < pocetNaOdobratie; i++) {
        spracujOdobratieZPriatelov(pouzivatel, 0, "z dovodu zrusenia uctu");
    }

}

//POZRETE
void Data::odstranUcet(Pouzivatel* pouzivatel) {
    pthread_mutex_lock(&mutexData);
    pouzivatelia.erase(*pouzivatel->getMeno());
    pthread_mutex_unlock(&mutexData);

    delete pouzivatel;
}

//POZRETE
void Data::odhlasPouzivatela(Pouzivatel* odhlasMna) {
    pthread_mutex_lock(&mutexData);
    for(int i = 0; i < this->prihlaseni.size(); i++) {
        if(this->prihlaseni[i]->getPouzivatel() == odhlasMna) {
            Prihlaseny* prihlaseny = this->prihlaseni[i];
            delete prihlaseny;

            this->prihlaseni.erase(this->prihlaseni.begin() + i);
            break;
        }
    }
    pthread_mutex_unlock(&mutexData);
}

void Data::ulozVsetko() {
    this->zapisPouzivatelov();
    this->zapisPriatelovPouzivatelov();
    this->zapisZiadostiOPriatelstvoPouzivatelov();
    this->zapisNeprecitaneSpravyPoouzivatelov();
}

void Data::nacitajVsetko() {
    this->nacitajPouzivatelov();
    this->nacitajPriatelovPouzivatelov();
    this->nacitajZiadostiOPriatelstvoPouzivatelov();
    this->nacitajNeprecitaneSpravyPoouzivatelov();
}

//POZRETE - POZOR pri volani musi byt zamknuty
void Data::pridajDoPriatelov(Pouzivatel* pouzivatel1, Pouzivatel* pouzivatel2) {
    pouzivatel1->pridajDoPriatelov(pouzivatel2);
    pouzivatel2->pridajDoPriatelov(pouzivatel1);
}
//POZRETE -  POZOR pri volani musi byt zamknuty
void Data::odoberZPriatelov(Pouzivatel* pouzivatel1, Pouzivatel* pouzivatel2) {
    pouzivatel1->odoberPriatela(pouzivatel2);
    pouzivatel2->odoberPriatela(pouzivatel1);
}

void Data::zapisPouzivatelov() {
    std::string stringPouzivatelia;
    for (auto pouzivatelIterator : this->pouzivatelia) {
        stringPouzivatelia += pouzivatelIterator.second->toString();
    }

    Zapisovac::zapisPozivatelov(stringPouzivatelia);
}

void Data::zapisPriatelovPouzivatelov() {
    std::string stringPriatelia;

    for (auto pouzivatelIterator : this->pouzivatelia) {

        stringPriatelia += pouzivatelIterator.first + "\n";
        std::vector<Pouzivatel*>* priatelia = pouzivatelIterator.second->getPriatelia();

        for (int i = 0; i < priatelia->size(); ++i) {
            stringPriatelia += *(*priatelia)[i]->getMeno() + "\n";
        }

        stringPriatelia += "<np>\n";
    }

    Zapisovac::zapisPriatelovPouzivatelov(stringPriatelia);
}

void Data::zapisZiadostiOPriatelstvoPouzivatelov() {
    std::string stringZiadosti;

    for (auto pouzivatelIterator : this->pouzivatelia) {

        stringZiadosti += pouzivatelIterator.first + "\n";
        std::vector<Pouzivatel*>* ziadosti = pouzivatelIterator.second->getZiadostiOPriatelstvo();

        for (int i = 0; i < ziadosti->size(); ++i) {
            stringZiadosti += *(*ziadosti)[i]->getMeno() + "\n";
        }

        stringZiadosti += "<np>\n";
    }

    Zapisovac::zapisZiadostiOPriatelstvoPouzivatelov(stringZiadosti);
}

void Data::zapisNeprecitaneSpravyPoouzivatelov() {
    std::string stringSpravy;

    for (auto pouzivatelIterator : this->pouzivatelia) {

        stringSpravy += pouzivatelIterator.first + "\n";
        std::vector<std::string>* neprecitaneSpravy = pouzivatelIterator.second->dajNeprecitaneSpravy();

        for (int i = 0; i < neprecitaneSpravy->size(); ++i) {
            stringSpravy += (*neprecitaneSpravy)[i] + "\n";

            stringSpravy += "<ns>\n";
        }
        stringSpravy += "<np>\n";
    }

    Zapisovac::zapisNeprecitanychSpravPouzivatelov(stringSpravy);
}

void Data::nacitajPouzivatelov() {
    std::string stringPouzivatelia;
    Nacitavac::nacitajPozivatelov(stringPouzivatelia);

    std::istringstream stringStream(stringPouzivatelia);

    std::string meno;
    std::string heslo;

    while (std::getline(stringStream, meno)) {
        std::getline(stringStream, heslo);

        pouzivatelia.insert({meno, new Pouzivatel(meno, heslo)});
    }
}

void Data::nacitajPriatelovPouzivatelov() {
    std::string stringPriatelia;
    Nacitavac::nacitajPriatelovPouzivatelov(stringPriatelia);

    std::istringstream stringStream(stringPriatelia);

    std::string meno;
    Pouzivatel* pouzivatel = nullptr;
    bool trebaZmenitPouzivatela = true;

    while (std::getline(stringStream, meno)) {

        if (meno == "<np>") {
            trebaZmenitPouzivatela = true;
            continue;
        }

        auto pouzivatelIterator = pouzivatelia.find(meno);

        if (trebaZmenitPouzivatela) {
            pouzivatel = pouzivatelIterator->second;
            trebaZmenitPouzivatela = false;
            continue;
        }

        pouzivatel->pridajDoPriatelov(pouzivatelIterator->second);
    }
}

void Data::nacitajZiadostiOPriatelstvoPouzivatelov() {
    std::string stringZiadosti;
    Nacitavac::nacitajZiadostiOPriatelstvoPouzivatelov(stringZiadosti);

    std::istringstream stringStream(stringZiadosti);

    std::string meno;
    Pouzivatel* pouzivatel = nullptr;
    bool trebaZmenitPouzivatela = true;

    while (std::getline(stringStream, meno)) {

        if (meno == "<np>") {
            trebaZmenitPouzivatela = true;
            continue;
        }

        auto pouzivatelIterator = pouzivatelia.find(meno);

        if (trebaZmenitPouzivatela) {
            pouzivatel = pouzivatelIterator->second;
            trebaZmenitPouzivatela = false;
            continue;
        }

        pouzivatel->pridajNovuZiadost(pouzivatelIterator->second);
    }
}

void Data::nacitajNeprecitaneSpravyPoouzivatelov() {
    std::string stringNeprecitaneSpravy;
    Nacitavac::nacitajNeprecitaneSpravyPouzivatelov(stringNeprecitaneSpravy);

    std::istringstream stringStream(stringNeprecitaneSpravy);

    std::string riadok;
    std::string sprava;
    Pouzivatel* pouzivatel = nullptr;
    bool trebaZmenitPouzivatela = true;

    while (std::getline(stringStream, riadok)) {

        if (riadok == "<np>") {
            trebaZmenitPouzivatela = true;
            continue;
        } else if (riadok == "<ns>") {
            pouzivatel->pridajNeprecitanuSpravu(sprava);
            sprava = "";
            continue;
        }

        auto pouzivatelIterator = pouzivatelia.find(riadok);

        if (trebaZmenitPouzivatela) {
            pouzivatel = pouzivatelIterator->second;
            trebaZmenitPouzivatela = false;
            continue;
        }

        sprava += riadok + "\n";
    }
}
