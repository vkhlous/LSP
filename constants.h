//
// Created by vk on 11.01.20.
//

#ifndef SERVER_CONSTANTS_H
#define SERVER_CONSTANTS_H

#define MAP_H 31
#define MAP_W 122

#define A_CLIENT_CHAR 'A'
#define A_CLIENT_NR 1
#define A_CLIENT_X 1
#define A_CLIENT_Y 1

#define B_CLIENT_CHAR 'B'
#define B_CLIENT_NR 2
#define B_CLIENT_X 119
#define B_CLIENT_Y 29

#define C_CLIENT_CHAR 'C'
#define C_CLIENT_NR 3
#define C_CLIENT_X 30
#define C_CLIENT_Y 1

#define D_CLIENT_CHAR 'D'
#define D_CLIENT_NR 4
#define D_CLIENT_X 1
#define D_CLIENT_Y 30

#define PLAYERS_COUNT 4

//<%s>,<%s>,<%s>,<%s>,<%s>,<%s>,<%s>
#define Ns_PLAYER_NAME "<%s>,"
#define Ns_PLAYER_NAME_SIZE 19

#define MSG_GAME_UPDATE_C 7

#define MSG_LOBBY_INFO_C 2
#define MSG_LOBBY_INFO "2<%d>{%s}"
#define MSG_LOBBY_INFO_SIZE 24

#define MSG_GAME_IN_PROCESS "3"
#define MSG_GAME_IN_PROCESS_SIZE 1

#define MSG_USERNAME_TAKEN "4"
#define MSG_USERNAME_TAKEN_SIZE 1

//5<spēlētāju_skaits>{<ntā_spēlētāja_segvārds>}<kartes_platums><kartes_augstums>
#define MSG_GAME_START_C 5
#define MSG_GAME_START "5<%d>{%s}<%d><%d>"
#define MSG_GAME_START_SIZE 17

//<ntā_spēlētāja_x_koordināta><ntā_spēlētāja_y_koordināta><ntā_spēlētāja_punkti>
#define Ns_PLAYER_UPDATE "<%d><%d><%d>,"
#define Ns_PLAYER_UPDATE_SIZE 16

//<ntā_ēdiena_x_koordināta><ntā_ēdiena_y_koordināta>
#define Ns_FOOD_UPDATE "<%d><%d>,"
#define Ns_FOOD_UPDATE_SIZE 11

//    7<spēlētāju_skaits>{<ntā_spēlētāja_x_koordināta><ntā_spēlētāja_y_koordināta><ntā_spēlētāja_punkti>}
//    <ēdienu_skaits>{<ntā_ēdiena_x_koordināta><ntā_ēdiena_y_koordināta>}
#define MSG_GAME_UPDATE_C 7
#define MSG_GAME_UPDATE "7<%d>{%s}<%d>{%s}"

//6<rindas_nummurs><kartes_rinda>
#define MSG_MAP_ROW_C 6
#define MSG_MAP_ROW "6<%d><%s>"
#define MSG_MAP_ROW_SIZE 9+MAP_W

#define MSG_JOIN_GAME_C '0'

#define SEGVARDS_SIZE 16

#define MSG_MOVE_C '1'
#define MOVE_LEFT 'L'
#define MOVE_RIGHT 'R'
#define MOVE_DOWN 'D'
#define MOVE_UP 'U'


//1<virziens>

#endif //SERVER_CONSTANTS_H
