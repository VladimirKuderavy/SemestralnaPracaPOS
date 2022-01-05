#pragma once
#include <iostream>
#include <sys/socket.h>
#include <cstring>


class PomocnaTrieda {
public:

    //tu to mozes dat podla socketu
    //ze najde v datach pouzivatela podla socketu ktory sem prichadza
    //a ak doslo k problemu tak ho odtial vyhodi

    static int prijmiSpravu(char* sprava, int* socket) {
        memset(sprava, 0, 4096);

        ssize_t bytesRecv = recv(*socket, sprava, 4096, 0);
        if(bytesRecv == -1) {
            std::cerr << "Problem so spojenim";
            return 1;
        }
        if(bytesRecv == 0) {
            std::cout << "Klient sa odpojil" << "\n";
            return 1;
        }
        return 0;

    }


};

