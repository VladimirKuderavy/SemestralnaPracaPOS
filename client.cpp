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
#include <signal.h>

#include "Konstanty.h"

void zachytavac_chyby(int signum){

}

void* vlaknoZobrazovacFunkcia(void* data) {
    int* sock = (int*) data;
    char buff[4096];
    memset(buff, 0, 4096);

    while(true) {
        int bytesReceived = recv(*sock, buff, 4096, 0);
        if(bytesReceived == 0) {
            break;
        }
        std::cout << ">: " << std::string(buff, bytesReceived) << "\n";

        memset(buff, 0, 4096);

    }

    return NULL;
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

    pthread_create(&zobrazovacVlakno, NULL, &vlaknoZobrazovacFunkcia, &sock);

    char buff[4096];
    std::string userInput;

    signal(SIGPIPE, zachytavac_chyby);

    while(true) {

        //std::cout << "Zadaj text:";
        getline(std::cin, userInput);
        if(userInput == "koniec") {
            break;
        }

        int sendVysledok = send(sock, userInput.c_str(), userInput.size(), 0);
        std:: cout << "Chyba: " << std::to_string(errno) << "\n";
        //std:: cout << "Vysledok je takyto : " << std::to_string(sendVysledok) << "\n";
        if(errno == __SIGRTMIN) {
            std::cout << "nebolo mozne odoslat na server";
            break;
        }


        memset(buff, 0, 4096);

    }
    pthread_cancel(zobrazovacVlakno);

    close(sock);

    return 0;
}





