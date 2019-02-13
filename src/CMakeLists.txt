cmake_minimum_required(VERSION 3.12)
project(TopMusic)

set(CMAKE_CXX_STANDARD 17)

add_executable(TopMusicServer server.cpp Song.h Song.cpp thisUser.cpp thisUser.h Songs.cpp Songs.h)
add_executable(TopMusicClient client.cpp Song.h Song.cpp thisUser.cpp thisUser.h Songs.cpp Songs.h)