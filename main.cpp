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
#include "Data.h"
#include "Prihlasenie.h"
#define PORT 6969

#define POCET_KLIENTOV 2

typedef struct dataACisloPortu {
    int cislo;
    Data* data;
} DATAACISLOKPORTU;







void* vlaknoFunkcia(void* param) {
    DATAACISLOKPORTU* dataacislokportu = (DATAACISLOKPORTU *) param;
    Data* data = dataacislokportu->data;
    int plus = dataacislokportu->cislo;
    //vytvorit socket


    //novy socket sa vytvori pomocou sys volania socket
    //1. paraameter - komunikacna domena - AF_INET = sockety umožňujúce komunikáciu pomocou protokolov založených na IPv4
    //2. parameter - typ socketu - SOCK_STREAM = spoľahlivá, obojsmerná, spojovo-orientovaná služba na prenos zoradenej sekvencie bajtov.
    //3. protokol - mohol by specifikovat dalsie vlastnosti socketu

    int listening = socket(AF_INET, SOCK_STREAM, 0);
    if(listening == -1) {
        std::cerr << "Nemozem vytvorit socket";
        return NULL;
    }
    //socket ktory sa vytvoril nema adresu,
    //ak chcemee na sockete prijimat spojenia, musime mu adresu priradit pomocou bind


    sockaddr_in hint;   //toto sa musi pouzit ak je AF_INET
    hint.sin_family = AF_INET;    //musi byt inicializovane na to co je v sockete
    hint.sin_port = htons(PORT + plus);    //cislo portu
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);  //IP v4 adresa

    //funkcia bind ma tri parametre:
    //1. sockfd - platny deskriptor sockety (ktory sme ziskali volanim funkcie socket
    //2. addr - ukazovatel na instanciu struktury sockaddr obsahujucej adresu servera
    //          v skutocnosti by mal program odovzdat pretypovany ukazovatel na spravne
    //          zinicializovanu instanciu niektoreho z potomkov tejto struktury
    //3. adrlen - skutocna velkost instancie, na ktoru ukakzuje parameter addr.

    if (bind(listening, (sockaddr*)&hint, sizeof(hint)) == -1) {
        std::cerr << "Nemozem nabindovat na IP /port";
        return NULL;
    }
    //Aby mohol zacat prijimat spojenia od klientov, musi sa oznacit ako pasivny pomocou listen
    //1. parametrom je socket, na ktorom chceme zacat prijimat spojenia od klientov
    //2. parameter je dlzka fronty, kam sa budu zaradzovvat poziadavky o vytvorenie spojenia, ktore nie je mozne
    //  okamzite obsluzit
    if(listen(listening, SOMAXCONN) == -1) {
        std:std::cerr << "Nemozem pocuvat";

    }


    sockaddr_in client;
    socklen_t clientSize = sizeof(client);
    char host[NI_MAXHOST];
    char svc[NI_MAXSERV];

    //cakanie a prijatie poziadavky na vytvorenie spojenia s klientom
    //funkcia bude blokovat vykonavanie procesu az kym klient nepoziada o vytvorenie spojenia
    //1. parameter - sockfd - platný deskriptor pasívneho socketu, s priradenou adresou.
    //          Po úspešnom vytvorení spojenia funkcia accept do tejto inštancie
    //          (ak to má v danej komunikačnej doméne význam) vyplní adresu klienta, ktorý požiadal o spojenie.
    //2. parameter - addr - ukazovateľ na inštanciu štruktúry sockaddr
    //3. parameter - addrlen - skutocna velkost instancie, na ktoru ukazuje param addr.
    //                  odovzdava sa adresa premennej obsahujucej sizeof
    int clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

    if(clientSocket == -1) {
        std::cerr << "Problem s pripojenim klienta";
        return NULL;
    }

    //Po úspešnom vytvorení spojenia vráti accept deskriptor socketu reprezentujúceho spojenie s konkrétnym klientom.
    // V danom čase môže byť takýchto spojení vytvorených viacero súčasne a každé je identifikované vlastným deskriptorom.

    //close the listening
    close(listening);


    memset(host, 0, NI_MAXHOST);
    memset(svc, 0, NI_MAXSERV);


    int result = getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, svc, NI_MAXSERV, 0);

    if(result) {
        std::cout << host << "connected on " << svc << std::endl;
    }
    else {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        std::cout << host << "conected on " << ntohs(client.sin_port) << std::endl;
    }
    char  buf[4096];


    Prihlasenie::prihlasenie(data,&clientSocket );
    /*
    while(true) {
        //ak sa niekto prihlasi a odpoji sa, tak ho treba zmazat z prihlasenych
        //vycistit bufer





        /*
        memset(buf, 0, 4096);
        int bytesRecv = recv(clientSocket, buf, 4096, 0);

        if(buf == "kkk") {
            return NULL;
        }

        if(bytesRecv == -1) {
            std::cerr << "Problem so spojenim";
            break;
        }
        if(bytesRecv == 0) {
            std::cout << "Klient sa odpojil" << "\n";
            break;
        }

        std::cout << "Received: " << std::string(buf, 0, bytesRecv) << std::endl;

        std::string s = "nieco";


        send(clientSocket, s.c_str(), s.size(), 0);



    }
    */

    close(clientSocket);



    return NULL;
}




int main() {



    Data* data = new Data();

    DATAACISLOKPORTU dataacislokportu[POCET_KLIENTOV];
    for(int i = 0; i < POCET_KLIENTOV; i++) {
        dataacislokportu[i].data = data;
        dataacislokportu[i].cislo = i;
    }




    pthread_t vlakna[POCET_KLIENTOV];


    for(int i = 0; i < POCET_KLIENTOV; i++) {
        pthread_create(&vlakna[i], NULL, &vlaknoFunkcia, &dataacislokportu[i]);
    }

    for(int i = 0; i < POCET_KLIENTOV; i++) {
        pthread_join(vlakna[i], NULL);
    }





    delete data;
    return 0;
}


