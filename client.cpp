#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string>
#include <string.h>
#include <functional>
#include <pthread.h>
#include <errno.h>

#include<fstream>
#include <sstream>

#include "Konstanty.h"

typedef struct data {
    int* socket;
    bool trebaOdoslatSubor;
} DATA;

bool prijmanieSuboru(int& zmazRiadkov, std::string& sprava, char* buff, int* sock) {
    //odpili ostatne spravy
    for(int i = 0; i < zmazRiadkov; i++) {
        sprava.erase(0, sprava.find("\n") + 1);
    }

    sprava.erase(0, sprava.find("\n") + 1);

    std::string nazovSuboru;
    std::string obsahSuboru;
    int zostavaPocetZnakov;


    std::istringstream stringstream(sprava);

    //Informacia o tom ze bol doruceny subor
    std::string vypis;
    std::getline(stringstream, vypis);
    std::cout << vypis << "\n";

    //v riadku nazov suboru
    std::getline(stringstream, nazovSuboru);

    std::string zostavaPocetZnakovString;
    std::getline(stringstream, zostavaPocetZnakovString);
    zostavaPocetZnakov = std::stoi(zostavaPocetZnakovString);


    obsahSuboru = sprava;
    obsahSuboru.erase(0, obsahSuboru.find("\n") + 1);
    obsahSuboru.erase(0, obsahSuboru.find("\n") + 1);
    obsahSuboru.erase(0, obsahSuboru.find("\n") + 1);

    zostavaPocetZnakov = zostavaPocetZnakov - obsahSuboru.size();


    while(zostavaPocetZnakov > 0) {
        int bytesReceived = recv(*sock, buff, 4096, 0);
        if(bytesReceived == 0) {
            return true;
        }
        std::string obsahSpravy = std::string(buff, bytesReceived);

        obsahSuboru += obsahSpravy;
        zostavaPocetZnakov -= obsahSpravy.size();

    }

    nazovSuboru = "doruceny" + nazovSuboru;

    std::ofstream ofstream(Konstanty::getRelativnaCesta() + nazovSuboru);

    ofstream << obsahSuboru;

    ofstream.close();

    sprava = "";

    return false;
}

bool odosielanieSuboru(std::string& userInput, int& sock) {

    bool opakovat = true;

    while(opakovat) {
        opakovat = false;

        std::ifstream citac(Konstanty::getRelativnaCesta() + userInput);
        if(citac.fail()) {
            std::cout << "Zadali ste zly nazov suboru\n";
            std::cout << "Chcete skusit znova? [1]\n";
            getline(std::cin, userInput);
            if(userInput == "1") {
                opakovat = true;
                getline(std::cin, userInput);
                continue;
            }

            std::string spravaNaServer =  Konstanty::getTextSuborSaNepodariloNacitat();
            if(send(sock, spravaNaServer.c_str(), spravaNaServer.size(), MSG_NOSIGNAL) == -1) {
                std::cout << "nebolo mozne odoslat na server";
                return true;
            }
            return true;
        }

        //tunak nacita cely subor a poposiela ho na server
        std::cout << "Pockajte kym sa dokonci nacitavanie zo suboru\n";
        std::string obsahSuboru ;
        std::string riadokSuboru ;

        while(getline(citac, riadokSuboru)) {
            riadokSuboru += "\n";

            obsahSuboru += riadokSuboru;
        }

        //odosle mu to v tvare
        //nazov suboru
        //velkost
        //zvysok suboru


        std::string hlavickaSuboru = userInput + "\n";
        hlavickaSuboru += std::to_string(obsahSuboru.size()) + "\n";

        if(send(sock, hlavickaSuboru.c_str(), hlavickaSuboru.size(), MSG_NOSIGNAL) == -1) {
            std::cout << "nebolo mozne odoslat na server";
            return true;
        }
        int poslanePo = 0;

        while(poslanePo < obsahSuboru.size()) {
            int hornaHranica = 4096;
            if(poslanePo + 4096 > obsahSuboru.size()) {
                hornaHranica = obsahSuboru.size() - poslanePo;
            }

            std::string poslat = obsahSuboru.substr(poslanePo, hornaHranica);

            if(send(sock, poslat.c_str(), poslat.size(), MSG_NOSIGNAL) == -1) {
                std::cout << "nebolo mozne odoslat na server";
                return true;
            }

            poslanePo += hornaHranica;
        }

    }

    return false;
}

void* vlaknoZobrazovacFunkcia(void* dataPar) {
    DATA* data = (DATA*) dataPar;
    int* sock = data->socket;

    char buff[4096];
    memset(buff, 0, 4096);

    while(true) {
        int bytesReceived = recv(*sock, buff, 4096, 0);
        if(bytesReceived == 0) {
            break;
        }
        std::string sprava = std::string(buff, bytesReceived);

        std::istringstream stringstream(sprava);
        std::string riadok;

        int zmazRiadkov = 0;

        while(std::getline(stringstream, riadok)) {

            if(riadok == Konstanty::getTextVstupDoModuNacitajSubor()) {
                sprava.erase(0, sprava.find("\n") + 1);
                data->trebaOdoslatSubor = true;
                memset(buff, 0, 4096);
                continue;
            }

            /*
             * std::string hlavickaSuboru = Konstanty::getTextUlozSubor() + "\n";
                hlavickaSuboru += "Pouzivatel " + *pouzivatel->getMeno() + "Vam posiela subor: " +
                nazovSuboru + " v konverzacii: " + *konverzacia->getNazov() + "\n";
                hlavickaSuboru += nazovSuboru + "\n";
                hlavickaSuboru += std::to_string(obsahSuboru.size()) + "\n";
             *
             *
             */

            if(riadok == Konstanty::getTextUlozSubor()) {
                if (prijmanieSuboru(zmazRiadkov, sprava, buff, sock)) {
                    return nullptr;
                } else {
                    break;
                }
            }

            zmazRiadkov++;
        }


        std::cout << ">: " << sprava <<  "\n";

        memset(buff, 0, 4096);

    }

    return nullptr;
}

int main(int argc, char* argv[]) {

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1) {
        std::cerr << "problem 1";
        return -1;
    }

    //vytvorenie hint struktury
    int port = PORT + atoi(argv[1]);
    std::string ipAdresa = "127.0.0.1";

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    inet_pton(AF_INET, ipAdresa.c_str(), &hint.sin_addr);

    //funkcia connect - nadviazanie spojenia pre klienta
    //ma rovnake parametre ako bind
    //sockfd - deskriptor socketu, ktory vznikol z volania socket
    //addr - ukazovateľ na inštanciu štruktúry sockaddr obsahujúcej adresu servera na ktorý sa chceme pripojiť
    //addrlen - skutočná veľkosť inštancie, na ktorú ukazuje parameter addr

    int connectResult = connect(sock, (sockaddr*)&hint, sizeof(sockaddr_in));
    if(connectResult == -1) {
        std::cerr << "problem 2";
        return -2;
    }

    pthread_t zobrazovacVlakno;

    DATA data = {
            &sock,
            false
    };

    pthread_create(&zobrazovacVlakno, NULL, &vlaknoZobrazovacFunkcia, &data);

    char buff[4096];
    std::string userInput;


    while(true) {

        //std::cout << "Zadaj text:";
        getline(std::cin, userInput);

        if(data.trebaOdoslatSubor) {
            data.trebaOdoslatSubor = false;

            if (odosielanieSuboru(userInput, sock)) {
                break;
            } else {
                continue;
            }
        }

        if(userInput == "koniec") {
            break;
        }

        int sendVysledok = send(sock, userInput.c_str(), userInput.size(), MSG_NOSIGNAL);
        //std:: cout << "Chyba: " << std::to_string(errno) << "\n";
        //std:: cout << "Vysledok je takyto : " << std::to_string(sendVysledok) << "\n";
        if(sendVysledok == -1) {
            std::cout << "nebolo mozne odoslat na server";

            break;
        }

        memset(buff, 0, 4096);

    }

    pthread_cancel(zobrazovacVlakno);
    pthread_join(zobrazovacVlakno, NULL);

    close(sock);

    return 0;
}





