#include <stdio.h>
#include <pthread.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <winsock.h>
#include "main.h"
#include "constants.h"
#include <stdio.h>


void process_arg(int argc, char*     []);
void create_socket();
void run();
void initialize_players();
void * listen_for_new_players(void * ptr);
int name_exist(char* name);
void process_JOIN_GAME_MSG(struct client *client, char *buffer);
void * process_new_player_thread(void * ptr);
void send_LOBBY_INFO_MSG(struct client *client);
void register_new_player(connection_t * connection);
void send_GAME_IN_PROGRESS_MSG(connection_t * connection);
void send_MSG(connection_t * connection, char * msg_buffer);
void send_USERNAME_TAKEN_MSG(connection_t * connection);
void start_game();
void * start_countdown(void * ptr);
void send_GAME_START_MSG(struct client * client);
void send_updated_LOBBY_INFO();
void read_send_game_MAP();
void send_MAP_ROW_MSG(struct client * client, char *line, int line_nr);
void send_MAP_line(char *map_line, int line_nr);
void add_MAP_line_to_array(char *map_line, int line_nr);

void send_game_update();

void send_first_GAME_UPDATE_MSG(struct client *client);

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

    while (GAME_IN_PROCESS == 0)
    {
        printf("Waiting for game to start.");
    }

    start_game();

    for(;;)
    {

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
    pthread_t countdown_thread;

    client_count++;

    new_player = &players[client_count - 1];
    new_player -> connection = connection;

    printf("Connected to the client with socket descriptor\n");
    printf("Client: on socket %d.\n", connection -> sock_desc);

    pthread_create(&thread, 0, process_new_player_thread, (void *) new_player);
    pthread_detach(thread);

    send_updated_LOBBY_INFO();

    if (client_count >= 2 && client_count < 8)
    {
        if (COUNTDOWN_STARTED == 0)
        {
            COUNTDOWN_STARTED = 1;
            pthread_create(&countdown_thread, 0, start_countdown, NULL);
            pthread_join(countdown_thread, NULL);
            COUNTDOWN_STARTED = 0;
            start_game();
        }
        else
        {
            pthread_cancel(countdown_thread);
            pthread_create(&countdown_thread, 0, start_countdown, NULL);
            pthread_join(countdown_thread, NULL);
            COUNTDOWN_STARTED = 0;
            start_game();
        }
    }
    else if (client_count == 8)
    {
        GAME_IN_PROCESS = 1;
    }
}

void  send_updated_LOBBY_INFO()
{
    int i = 1;
    for (; i <= PLAYERS_COUNT; i++)
    {
        if (players[i].active == 1)
        {
            send_LOBBY_INFO_MSG(&players[i]);
        }
    }
}

void start_game()
{
    //MSG_GAME_START
    int i = 1;
    for (; i <= PLAYERS_COUNT; i++)
    {
        if (players[i].active == 1)
        {
            send_GAME_START_MSG(&players[i]);
        }
    }

    // Send map to each
    read_send_game_MAP();

    // Send positions to each
    send_game_update();
    // Start new thread for each player.
}

void send_game_update()
{
    int i = 1;
    for (; i <= client_count; i++)
    {
        if (players[i].active == 0)
        {
            send_first_GAME_UPDATE_MSG(&players[i]);
        }
    }
}

void send_first_GAME_UPDATE_MSG(struct client *client) {
    char* msg_buffer = NULL;
    char* list_of_players_updates = NULL;
    connection_t * conn;

    conn = (connection_t *)client -> connection;

    msg_buffer = (char *)malloc(MSG_GAME_START_SIZE + (client_count * Ns_PLAYER_UPDATE_SIZE) + (client_count * Ns_FOOD_UPDATE_SIZE));
    list_of_players_updates = (char *)malloc(client_count * Ns_PLAYER_UPDATE_SIZE);

    /////////////////////////////////
    int i = 0;
    char temp[Ns_PLAYER_UPDATE_SIZE];
    for (; i < client_count - 1; i++)
    {
        sprintf(temp, Ns_PLAYER_UPDATE, players[i].x, players[i].y, players[i].points);
        strcat(list_of_players_updates, temp);
    }

    sprintf(temp, "<%d><%d><%d>", players[i].x, players[i].y, players[i].points);
    strcat(list_of_players_updates, temp);
    //////////////////////////////////
    //"7<%d>{%s}%d{%s}"
    // 5<spēlētāju_skaits>{<ntā_spēlētāja_segvārds>}<kartes_platums><kartes_augstums>
    sprintf(msg_buffer, MSG_GAME_UPDATE , client_count, list_of_players_updates, 0, NULL);

    printf("Message to send: %s\n", msg_buffer);

    send_MSG(conn, msg_buffer);

    free(msg_buffer);
    free(list_of_players_updates);
}

void read_send_game_MAP()
{
    static char buffer[MAP_W+1];
    int line_count = 0;
    FILE * fp;
    char * line = NULL;

    fp = fopen("./map.txt", "r");
    if (fp == NULL)
    {
        exit(EXIT_FAILURE);
    }

    while(!feof (fp))
    {
        if (fgets(buffer, MAP_W, fp))
        {
            send_MAP_line(buffer, line_count);
            add_MAP_line_to_array(buffer, line_count);
            printf("%s", buffer);
            line_count++;
        }
    }

    fclose(fp);
}

void add_MAP_line_to_array(char *map_line, int line_nr)
{
    int i = 0;
    for (; i < MAP_W; i++)
    {
        game_map[line_nr][i] = map_line[i];
    }
}

void send_MAP_line(char *map_line, int line_nr)
{
    int i = 1;
    for (; i <= PLAYERS_COUNT; i++)
    {
        if (players[i].active == 1)
        {
            send_MAP_ROW_MSG(&players[i], map_line, line_nr);
        }
    }
}

//6<rindas_nummurs><kartes_rinda>
void send_MAP_ROW_MSG(struct client * client, char *line, int line_nr)
{
    char* msg_buffer = NULL;
    connection_t * conn;

    conn = (connection_t *)client -> connection;

    msg_buffer = (char *)malloc(MSG_MAP_ROW_SIZE + 1);
    //6<rindas_nummurs><kartes_rinda>
    snprintf(msg_buffer, MSG_MAP_ROW_SIZE, MSG_MAP_ROW, line_nr, line);
    printf("Message to send: %s\n",msg_buffer);

    send_MSG(conn, msg_buffer);

    free(msg_buffer);
}

void send_GAME_START_MSG(struct client * client)
{
    char* msg_buffer = NULL;
    char* list_of_players = NULL;
    connection_t * conn;

    conn = (connection_t *)client -> connection;

    msg_buffer = (char *)malloc(MSG_GAME_START_SIZE + (client_count * Ns_PLAYER_NAME_SIZE));
    list_of_players = (char *)malloc(client_count * Ns_PLAYER_NAME_SIZE);

    int i = 0;
    char temp[Ns_PLAYER_NAME_SIZE];
    for (; i < client_count - 1; i++)
    {
        sprintf(temp, Ns_PLAYER_NAME, players[i].segvards);
        strcat(list_of_players, temp);
    }

    sprintf(temp, "<%s>", players[client_count - 1].segvards);
    strcat(list_of_players, temp);

   // 5<spēlētāju_skaits>{<ntā_spēlētāja_segvārds>}<kartes_platums><kartes_augstums>
    sprintf(msg_buffer, MSG_GAME_START, client_count, list_of_players, MAP_W, MAP_H);

    printf("Message to send: %s\n", msg_buffer);

    send_MSG(conn, msg_buffer);

    free(msg_buffer);
    free(list_of_players);
}

void * start_countdown(void * ptr)
{

}

int name_exist(char* name)
{
    int i = 1;
    for (; i <= PLAYERS_COUNT; i++)
    {
        if (strcmp(name, players[i].segvards) == 0)
        {
            return 1;
        }
    }

    return 0;
}

void process_JOIN_GAME_MSG(struct client *client, char *buffer)
{
    if (name_exist(buffer) == 1)
    {
        send_USERNAME_TAKEN_MSG(client->connection);
        return;
    }

    client -> active = 1;
    client -> segvards = (char *)malloc(sizeof(buffer));
    strcpy(client -> segvards, buffer);

    send_LOBBY_INFO_MSG(client);
}

void send_USERNAME_TAKEN_MSG(connection_t * connection)
{
    int len;
    char* msg_buffer = NULL;

    msg_buffer = (char *)malloc(MSG_USERNAME_TAKEN_SIZE + 1);
    snprintf(msg_buffer, MSG_USERNAME_TAKEN_SIZE, MSG_USERNAME_TAKEN); // TODO change
    send_MSG(connection, msg_buffer);

    free(msg_buffer);
}

void * process_new_player_thread(void * ptr)
{
    char * buffer;
    int len = 0;
    connection_t * conn;
    struct client * client;
    long addr = 0;

    client = (struct client *) ptr; // Getting client from the main thread
    conn = client -> connection; //  Getting connection from thread

    while (GAME_IN_PROCESS == 0) {
        read(conn->sock_desc, &len, sizeof(int));

        if (len > 0) {
            buffer = (char *) malloc((len + 1) * sizeof(char));
            buffer[len] = 0;
            read(conn->sock_desc, buffer, len);

            if (buffer[0] == '0') {
                process_JOIN_GAME_MSG(client, buffer);
            }

            free(buffer);
        }
    }

    close(conn->sock_desc);
    free(conn);
    printf("\n");
    pthread_exit(0);
}





void send_LOBBY_INFO_MSG(struct client *client)
{
    char* msg_buffer = NULL;
    int len = MSG_LOBBY_INFO_SIZE + (client_count * Ns_PLAYER_NAME_SIZE);
    char* list_of_players = NULL;
    connection_t * conn;

    conn = (connection_t *)client -> connection;

    msg_buffer = (char *)malloc(len);
    list_of_players = (char *)malloc(client_count * Ns_PLAYER_NAME_SIZE);

    int i = 0;
    char temp[Ns_PLAYER_NAME_SIZE];
    for (; i < client_count - 1; i++)
    {
        sprintf(temp, Ns_PLAYER_NAME, players[i].segvards);
        strcat(list_of_players, temp);
    }

    sprintf(temp, "<%s>", players[client_count - 1].segvards);
    strcat(list_of_players, temp);

    //"2<%d>{%s}"
    sprintf(msg_buffer, MSG_LOBBY_INFO, client_count, list_of_players);

    printf("Message to send: %s\n", msg_buffer);

    send_MSG(conn, msg_buffer);

    free(msg_buffer);
    free(list_of_players);
}
