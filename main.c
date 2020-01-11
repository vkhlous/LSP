#include <stdio.h>
#include <pthread.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include "main.h"
#include "constants.h"

void process_arg(int argc, char*     []);
void create_socket();
void run();
void initialize_players();
void * listen_for_new_players(void * ptr);
int name_exist(char* name);
void process_JOIN_GAME_MSG(struct client *client, char *buffer);
void * process(void * ptr);
void send_LOBBY_INFO_MSG(struct client *client);
void register_new_player(connection_t * connection);
void send_GAME_IN_PROGRESS_MSG(connection_t * connection);
void send_MSG(connection_t * connection, char * msg_buffer);

int main(int argc, char* argv[])
{

    process_arg(argc, argv);

    create_socket();

    run();

    return 0;
}

void create_socket()
{
    struct sockaddr_in address;
    int result;

    SOCK = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (SOCK <= 0)
    {
        fprintf(stderr, "error: cannot create socket\n");
        perror("Error description: ");
        exit(0);
    }

    printf("Socket created. Socket descriptor: %d\n", SOCK);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    result = bind(SOCK, (struct sockaddr *)&address, sizeof(struct sockaddr_in));

    if (result != 0)
    {
        fprintf(stderr, "error: cannot bind socket\n");
        perror("Error description: ");
        exit(0);
    }

    printf("Socket binded. \n");

    result = listen(SOCK, 5); // 5 allowed connected clients,

    if (result != 0)
    {
        fprintf(stderr, "error: cannot mark socket as passive\n");
        perror("Error description: ");
        exit(0);
    }

    printf("Socket is marked as passive.");

    return;
}

void process_arg(int argc, char* argv[])
{
    if (argc == 3)
    {
        HOST = gethostbyname(argv[1]); // Can be mistake?
        if (!HOST)
        {
            fprintf(stderr, "error: unknown host %s\n", argv[1]);
            perror("Error description: ");
            exit(1);
        }

        sscanf(argv[2], "%d", &PORT);
        printf("Arguments processed.\n");

    }
    else
    {
        printf("Not enough arguments.\n");
        exit(1);
    }

    return;
}

void initialize_players()
{
    int i = 1;
    for (; i <= PLAYERS_COUNT; i++)
    {
        if (i == A_CLIENT_NR)
        {
            players[i].symbol = A_CLIENT_CHAR;
            players[i].active = 0;
            players[i].x = A_CLIENT_X;
            players[i].y = A_CLIENT_Y;
        }
        else if (i == B_CLIENT_NR)
        {
            players[i].symbol = B_CLIENT_CHAR;
            players[i].active = 0;
            players[i].x = B_CLIENT_X;
            players[i].y = B_CLIENT_Y;
        }
        else if (i == C_CLIENT_NR)
        {
            players[i].symbol = C_CLIENT_CHAR;
            players[i].active = 0;
            players[i].x = C_CLIENT_X;
            players[i].y = C_CLIENT_Y;

        }
        else if (i == D_CLIENT_NR)
        {
            players[i].symbol = D_CLIENT_CHAR;
            players[i].active = 0;
            players[i].x = D_CLIENT_X;
            players[i].y = D_CLIENT_Y;

        }

    }

}

void run()
{
    pthread_t thread;

    //Initialize all players
    initialize_players();

    pthread_create(&thread, 0, listen_for_new_players, NULL);
    pthread_detach(thread);

    for(;;){

    }

    return;
}

void * listen_for_new_players(void * ptr)
{
    connection_t * connection;

    while (1)
    {
        /* accept incoming connections*/
        connection = (connection_t *)malloc(sizeof(connection_t));
        connection->sock_desc = accept(SOCK, &connection->address, &connection->addr_len);
        printf("Connection %d\n", connection->sock_desc);
        if (connection->sock_desc <= 0)
        {
            free(connection);
        }
        else
        {
            if (client_count >= 8 || GAME_IN_PROCESS == 1)
            {
                send_GAME_IN_PROGRESS_MSG(connection);
            } else {
                register_new_player(connection);
            }
        }
    }
}

void send_GAME_IN_PROGRESS_MSG(connection_t * connection)
{
    int len;
    char* msg_buffer = NULL;

    msg_buffer = (char *)malloc(MSG_GAME_IN_PROCESS_SIZE + 1);
    snprintf(msg_buffer, MSG_GAME_IN_PROCESS_SIZE, MSG_GAME_IN_PROCESS); // TODO change
    send_MSG(connection, msg_buffer);

    free(msg_buffer);
}

void send_MSG(connection_t * connection, char * msg_buffer)
{
    int len;

    printf("Message to send: %s\n",msg_buffer);

    len = strlen(msg_buffer);
    msg_buffer[len] = '\0';
    send(connection->sock_desc, &len, sizeof(int), 0);
    send(connection->sock_desc, msg_buffer, sizeof(msg_buffer), 0);
}

/* start a new thread but do not wait for it */
void register_new_player(connection_t * connection)
{
    struct client * new_player;
    pthread_t thread;

    client_count++;

    new_player = &players[client_count - 1];
    new_player -> connection = connection;

    printf("Connected to the client with socket descriptor\n");
    printf("Client: on socket %d.\n", connection -> sock_desc);

    pthread_create(&thread, 0, process, (void *)new_player);
    pthread_detach(thread);
}

int name_exist(char* name)
{
    int i = 1;
    for (; i <= PLAYERS_COUNT; i++)
    {
        if (strcmp())
    }

    return 0;
}

void process_JOIN_GAME_MSG(struct client *client, char *buffer)
{

    if (name_exist(buffer) == 1)
    {
        // send name exist message
        return;
    }

    client -> active = 1;
    client -> segvards = (char *)malloc(sizeof(buffer));
    strcpy(client -> segvards, buffer);

    send_LOBBY_INFO_MSG(client);
}

void * process(void * ptr)
{
    char * buffer;
    int len = 0;
    connection_t * conn;
    struct client * client;
    long addr = 0;

    client = (struct client *) ptr; // Getting client from the main thread
    conn = client -> connection; //  Getting connection from thread

    read (conn->sock_desc, &len, sizeof(int));

    if (len > 0)
    {
        buffer = (char *)malloc((len+1)*sizeof(char));
        buffer[len] = 0;
        read(conn->sock_desc, buffer, len);

        if (buffer[0] == '0')
        {
            process_JOIN_GAME_MSG(client, buffer);
        }

        free(buffer);
    }

    close(conn->sock_desc);
    free(conn);
    printf("\n");
    pthread_exit(0);
}

void send_LOBBY_INFO_MSG(struct client *client)
{
    char* msg_buffer = NULL;
    connection_t * conn;

    conn = (connection_t *)client -> connection;

    msg_buffer = (char *)malloc(MSG_LOBBY_INFO_SIZE + 1);
    snprintf(msg_buffer, MSG_LOBBY_INFO_SIZE, MSG_LOBBY_INFO, client_count, client -> segvards); // TODO change
    printf("Message to send: %s\n",msg_buffer);

    send_MSG(conn, msg_buffer);

    free(msg_buffer);
}
