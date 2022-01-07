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

#include "Moznosti.h"
#include "Konstanty.h"



typedef struct dataACisloPortu {
    int cislo;
    int clientSocket;
    Data* data;
} DATAACISLOKPORTU;

typedef struct dataAVlakna {
    Data* data;
    std::vector<pthread_t>* vlaknaKlientov;
} DATAAVLAKNA;

void* vlaknoFunkcia(void* param) {


    DATAACISLOKPORTU* dataacislokportu = (DATAACISLOKPORTU *) param;
    Data* data = dataacislokportu->data;
    int plus = dataacislokportu->cislo;
    //vytvorit socket
    int clientSocket = dataacislokportu->clientSocket;




    data->pridajOtvorenySocket(clientSocket);


    //1 kilneme to spojenie manualne pri zabiti  toho vlakna
    //2 bool premenna, ktora sa zastavi -> porty na ktorych su uzivatelia sa vypnu ked sa odhlasia
            //porty na ktorych nie su sa vypnu, ked sa niekto pripoji

    if (clientSocket == -1) {
        std::cerr << "Problem s pripojenim klienta";
        return NULL;
    }

    //Po úspešnom vytvorení spojenia vráti accept deskriptor socketu reprezentujúceho spojenie s konkrétnym klientom.
    // V danom čase môže byť takýchto spojení vytvorených viacero súčasne a každé je identifikované vlastným deskriptorom.

    //close the listening



   bool odpojiloHo = false;
    while(!odpojiloHo) {
    Pouzivatel *pouzivatel = Prihlasenie::prihlasenie(data, &clientSocket);
    if(pouzivatel == nullptr) {
        break;
    }

    //std::cout << *pouzivatel->getMeno() << " " << std::to_string(clientSocket);

    if(Moznosti::vyberSiMoznost(pouzivatel, data, &clientSocket)) {
        odpojiloHo = true;
    };

    data->odhlasPouzivatela(pouzivatel);

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



        //data->zatvorSocket(clientSocket);
    }
    data->zatvorSocket(clientSocket);
    return NULL;
}

void* funkciaPosielacSprav(void* parData) {
    Data* data = (Data*) parData;

    while(true) {

        pthread_mutex_lock(data->getMutexSpravy());
        while(data->getSpravy()->size() == 0) {
            pthread_cond_wait(data->getCondSpravy(), data->getMutexSpravy());
        }
        Sprava* sprava = (*data->getSpravy())[data->getSpravy()->size()-1];
        data->getSpravy()->pop_back();
        pthread_mutex_unlock(data->getMutexSpravy());

        std::cout << "Vlakno bezi";

        data->odosliSpravuCezSocket(sprava->getAdresat(), sprava->getObsah());


        delete sprava;
    }


    return NULL;
}


void* funckiaVytvaracKlientov(void* dataPar) {
    DATAAVLAKNA* dataavlakna = (DATAAVLAKNA*) dataPar;
    Data* data = dataavlakna->data;

    //---------------------------------------------
    //---------------------------------------------
    //----------ZACIATOK PRIPOJENIA SOCKETOV------------


    //novy socket sa vytvori pomocou sys volania socket
    //1. paraameter - komunikacna domena - AF_INET = sockety umožňujúce komunikáciu pomocou protokolov založených na IPv4
    //2. parameter - typ socketu - SOCK_STREAM = spoľahlivá, obojsmerná, spojovo-orientovaná služba na prenos zoradenej sekvencie bajtov.
    //3. protokol - mohol by specifikovat dalsie vlastnosti socketu



    int listening = socket(AF_INET, SOCK_STREAM, 0);
    if(listening == -1) {
        std::cerr << "Nemozem vytvorit socket";
        return nullptr;
    }
    //socket ktory sa vytvoril nema adresu,
    //ak chcemee na sockete prijimat spojenia, musime mu adresu priradit pomocou bind


    sockaddr_in hint;   //toto sa musi pouzit ak je AF_INET
    hint.sin_family = AF_INET;    //musi byt inicializovane na to co je v sockete
    hint.sin_port = htons(PORT);    //cislo portu
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);  //IP v4 adresa

    //funkcia bind ma tri parametre:
    //1. sockfd - platny deskriptor sockety (ktory sme ziskali volanim funkcie socket
    //2. addr - ukazovatel na instanciu struktury sockaddr obsahujucej adresu servera
    //          v skutocnosti by mal program odovzdat pretypovany ukazovatel na spravne
    //          zinicializovanu instanciu niektoreho z potomkov tejto struktury
    //3. adrlen - skutocna velkost instancie, na ktoru ukakzuje parameter addr.

    if (bind(listening, (sockaddr*)&hint, sizeof(hint)) == -1) {
        std::cerr << "Nemozem nabindovat na IP /port";
        return nullptr;
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

    data->pridajOtvorenySocket(listening);
    while(true) {
        int clientSocket = accept(listening, (sockaddr *) &client, &clientSize);



        int result = getnameinfo((sockaddr *) &client, sizeof(client), host, NI_MAXHOST, svc, NI_MAXSERV, 0);

        if (result) {
            std::cout << host << "connected on " << svc << std::endl;
        } else {
            inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
            std::cout << host << "conected on " << ntohs(client.sin_port) << std::endl;
        }
        memset(host, 0, NI_MAXHOST);
        memset(svc, 0, NI_MAXSERV);

        pthread_t vlakno;

        DATAACISLOKPORTU dataacislokportu = {
                0,

                clientSocket,
                data
        };

        pthread_create(&vlakno, NULL, &vlaknoFunkcia, &dataacislokportu);
        dataavlakna->vlaknaKlientov->push_back(vlakno);
    }




    return NULL;
}



int main() {
    pthread_mutex_t mutexSpravy;

    pthread_cond_t pdmSpravy;

    pthread_mutex_init(&mutexSpravy, NULL);
    pthread_cond_init(&pdmSpravy, NULL);

    Data* data = new Data(&mutexSpravy, &pdmSpravy);
    data->nacitajVsetko();


    pthread_t pocuvac;
    pthread_create(&pocuvac, NULL, &funkciaPosielacSprav, data);

    std::vector<pthread_t> klienti;
    DATAAVLAKNA dataAVlakna = {
            data,
            &klienti
    };

    pthread_t vytvaracKlientov;
    pthread_create(&vytvaracKlientov, NULL, &funckiaVytvaracKlientov, &dataAVlakna);


    /*
    for(int i = 0; i < POCET_KLIENTOV; i++) {
        pthread_create(&vlakna[i], NULL, &vlaknoFunkcia, &dataacislokportu[i]);
    }
    */


    /*
    for(int i = 0; i < POCET_KLIENTOV; i++) {
        pthread_join(vlakna[i], NULL);
    }
    */


    while(true) {
        std::string koniec;
        std::cout << "Pre ukoncenie servera zadajte koniec\n";
        std::cin >> koniec;
        if(koniec == "koniec") {
            //TODO ulozenie do suboru

            break;
        }
    }
    /*
    for(int i = 0; i < POCET_KLIENTOV; i++) {
        pthread_cancel(vlakna[i]);
        pthread_join(vlakna[i], NULL);
    }
    */


    //TODO Zamknut mutex
    pthread_cancel(pocuvac);
    pthread_join(pocuvac, NULL);

    data->zatvorVsetkyOtvoreneSockety();

    for(int i = 0; i < klienti.size(); i++) {
        pthread_cancel(klienti[i]);
        pthread_join(klienti[i], NULL);
    }
    pthread_cancel(vytvaracKlientov);
    pthread_join(vytvaracKlientov, NULL);


    //TODO unlock mutex
    data->ulozVsetko();


    //pthread_join(pocuvac, NULL);

    pthread_cond_destroy(&pdmSpravy);
    pthread_mutex_destroy(&mutexSpravy);


    // rozposlat nedorucene spravy pouzivatelom

    delete data;
    return 0;



}


