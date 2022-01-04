cmake_minimum_required(VERSION 3.18)
project(SemestralnaPracaPOS)

set(CMAKE_CXX_STANDARD 17)

add_executable(server main.cpp Pouzivatel.h Pouzivatel.cpp Data.h Data.cpp Prihlasenie.h Prihlasenie.cpp Pomocna.h Pomocna.cpp )
add_executable(client client.cpp)

target_link_libraries(server pthread)
target_link_libraries(client pthread)