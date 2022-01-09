cmake_minimum_required(VERSION 3.18)
project(SemestralnaPracaPOS)

set(CMAKE_CXX_STANDARD 17)


add_executable(server server.cpp Pouzivatel.h Data.h Data.cpp Prihlasenie.h Prihlasenie.cpp
        Pomocna.h Pomocna.cpp SpravaPriatelstiev.h SpravaPriatelstiev.cpp Moznosti.h Moznosti.cpp SpravaKonverzacii.h
        SpravaKonverzacii.cpp Konverzacia.h Konstanty.h Zapisovac.h Nacitavac.h Nacitavac.cpp Hash.h Hash.cpp Zapisovac.cpp)
add_executable(client client.cpp Konstanty.h Hash.h Hash.cpp)

target_link_libraries(server pthread)
target_link_libraries(client pthread)