cmake_minimum_required(VERSION 3.18)
project(SemestralnaPracaPOS)

set(CMAKE_CXX_STANDARD 17)


add_executable(server server.cpp Pouzivatel.h Pouzivatel.cpp Data.h Data.cpp Prihlasenie.h Prihlasenie.cpp
        Pomocna.h Pomocna.cpp SpravaPriatelstiev.h SpravaPriatelstiev.cpp Moznosti.h Moznosti.cpp SpravaKonverzacii.h SpravaKonverzacii.cpp Konverzacia.h Konverzacia.cpp Sprava.h Sprava.cpp Konstanty.h)
add_executable(client client.cpp Konstanty.h)
add_executable(client2 client.cpp Konstanty.h)

target_link_libraries(server pthread)
target_link_libraries(client pthread)
target_link_libraries(client2 pthread)