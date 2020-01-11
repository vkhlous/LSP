//
// Created by vk on 11.01.20.
//
#include <sys/socket.h>

#ifndef SERVER_MAIN_H
#define SERVER_MAIN_H

typedef struct
{
    int sock_desc;
    struct sockaddr address;
    int addr_len;
} connection_t;

typedef struct
{
    int x;
    int y;

} position_t;

typedef struct client
{
    connection_t * connection;
    char* segvards;
    char symbol;
    int active; // 1 - active; 0- non active
    int x;
    int y;

} client_t;


int PORT;
struct hostent * HOST;
int SOCK;
int client_count = 0;

struct client players[8];

int const GAME_IN_PROCESS = 0;

#endif //SERVER_MAIN_H