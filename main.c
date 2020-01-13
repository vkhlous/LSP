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
#include <stdio.h>


void process_arg(int argc, char*     []);
void create_socket();
void run();
void initialize_players();
void * listen_for_new_players(void * ptr);
int name_exist(char* name);
void process_JOIN_GAME_MSG(int client_nr, char *buffer);
//void process_JOIN_GAME_MSG(struct client *client, char *buffer);
void * process_new_player_thread(void * ptr);
void send_LOBBY_INFO_MSG();
void register_new_player(connection_t * connection);
void send_GAME_IN_PROCESS_MSG(connection_t * connection);
void send_MSG(connection_t * connection, char * msg_buffer);
void send_USERNAME_TAKEN_MSG(connection_t * connection);
void start_game();
void * start_countdown(void * ptr);
void send_GAME_START_MSG();
void send_updated_LOBBY_INFO();
void read_send_game_MAP();
void send_MAP_ROW_MSG(char *line, int line_nr);
void send_MAP_line(char *map_line, int line_nr);
void add_MAP_line_to_array(char *map_line, int line_nr);
void set_games_countdown();

void send_game_update();

void send_first_GAME_UPDATE_MSG();

void send_MSG_TO_ALL_ACTIVE(char * msg_buffer);

void change_smth(struct client *pClient);

void smth();

void print_game_map();

void set_players_coordinats();

char * process_income_message(char* buffer);

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

    result = listen(SOCK, 10); // 5 allowed connected clients,

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
    int i = 0;
    for (; i < PLAYERS_COUNT; i++)
    {
        if (i+1 == A_CLIENT_NR)
        {
            players[i].symbol = A_CLIENT_CHAR;
            players[i].active = 0;
            players[i].x = A_CLIENT_X;
            players[i].y = A_CLIENT_Y;
        }
        else if (i+1 == B_CLIENT_NR)
        {
            players[i].symbol = B_CLIENT_CHAR;
            players[i].active = 0;
            players[i].x = B_CLIENT_X;
            players[i].y = B_CLIENT_Y;
        }
        else if (i+1 == C_CLIENT_NR)
        {
            players[i].symbol = C_CLIENT_CHAR;
            players[i].active = 0;
            players[i].x = C_CLIENT_X;
            players[i].y = C_CLIENT_Y;

        }
        else if (i+1 == D_CLIENT_NR)
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

   //change_smth(&players[1]);
 //   printf("CHANGEE: %d\n", players[1].active);

    while (client_count < 2)
    {

    }

    printf("First name : %s\n", players[0].segvards);

   // set_games_countdown();
    GAME_IN_PROCESS = 1;

    while (GAME_IN_PROCESS == 0)
    {
       // printf("Waiting for game to start.");
    }

    start_game();
//    printf("...");
//    printf("here\n");
//    printf("CLIENTCOUNT: \n");
//    printf("!!!!!!!");
//    printf("...");


    for(;;)
    {

    }

    return;
}

void * listen_for_new_players(void * ptr)
{
    connection_t * connection;

    while (client_count <= 8)
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
                printf("Can't register new player. GAME_IN_PROCESS_MSG will be send.");
                send_GAME_IN_PROCESS_MSG(connection);
            } else {
                printf("Registering new player...");
                register_new_player(connection);
            }
        }
    }

    printf("Listen for new players thread is over.\n");
    printf("\n");
    pthread_exit(0);
}

void smth() {
    players[0].active = 1;
    players[0].segvards = (char *)malloc(sizeof("name"));

    //strcpy( players[client_nr].segvards, buffer);
    players[0].segvards = strdup("name");

}

void send_GAME_IN_PROCESS_MSG(connection_t * connection)
{
    printf("send_GAME_IN_PROCESS_MSG: START\n");
    int len;
    char* msg_buffer = NULL;

    msg_buffer = (char *)malloc(MSG_GAME_IN_PROCESS_SIZE + 1);
    snprintf(msg_buffer, MSG_GAME_IN_PROCESS_SIZE, MSG_GAME_IN_PROCESS); // TODO change
    send_MSG(connection, msg_buffer);

    free(msg_buffer);
    printf("send_GAME_IN_PROCESS_MSG: END\n");
}

void send_MSG(connection_t * connection, char * msg_buffer)
{
    int len;

    printf("send_MSG START\n");
    printf("send_MSG: Message to send: %s on socket: %d\n", msg_buffer, connection->sock_desc);

    len = strlen(msg_buffer);
    msg_buffer[len] = '\0';
    send(connection->sock_desc, &len, sizeof(int), 0);
    send(connection->sock_desc, msg_buffer, sizeof(msg_buffer), 0);
}

void send_MSG_TO_ALL_ACTIVE(char * msg_buffer)
{
    int i = 0;

    for (; i < PLAYERS_COUNT; i++)
    {
        printf("I: %d\n", i);
        printf("IS ACTIVE: %c %s %d\n",players[i].symbol,  players[i].segvards, players[i].active );
        if (players[i].active == 1)
        {
            send_MSG(players[i].connection, msg_buffer);
        }
    }
}

void set_games_countdown()
{
    pthread_t countdown_thread;

    if (client_count >= 2 && client_count < 8)
    {
//        if (COUNTDOWN_STARTED == 0)
//        {
//            COUNTDOWN_STARTED = 1;
//            pthread_create(&countdown_thread, 0, start_countdown, NULL);
//            pthread_join(countdown_thread, NULL);
//            COUNTDOWN_STARTED = 0;
               GAME_IN_PROCESS = 1;
//        }
//        else
//        {
//            pthread_cancel(countdown_thread);
//            pthread_create(&countdown_thread, 0, start_countdown, NULL);
//            pthread_join(countdown_thread, NULL);
//            COUNTDOWN_STARTED = 0;
//            GAME_IN_PROCESS = 1;
//        }
    }
    else if (client_count == 8)
    {
        GAME_IN_PROCESS = 1;
    }
}

/* start a new thread but do not wait for it */
void register_new_player(connection_t * connection)
{
    struct client * new_player;
    pthread_t thread;
    pthread_t countdown_thread;
    int client_nr;

    printf("Connected to the client with socket descriptor\n");
    printf("Client: on socket %d.\n", connection -> sock_desc);

    if (client_count != 0) {
        client_nr = client_count - 1;
    } else {
        client_nr = 0;
    }

    players[client_nr].connection = connection;

    pthread_create(&thread, 0, process_new_player_thread, (void *) client_nr);
    pthread_detach(thread);
}

void  send_updated_LOBBY_INFO()
{
    printf("Sending updated LOBBY_INFO_MSG to each active player...\n");
    int i = 1;
    send_LOBBY_INFO_MSG(i);
//    for (; i <= PLAYERS_COUNT; i++)
//    {
//        if (players[i].active == 1)
//        {
//            send_LOBBY_INFO_MSG(i);
//        }
//    }
    printf("Updates LOBBY_INFO_MSG sent to each active player!\n");
}

void start_game()
{
    printf("Starting game!\n");

    int i = 0;
//    for (; i < PLAYERS_COUNT; i++)
//    {
//        printf("I: %d\n", i);
//        printf("IS ACTIVE: %c %s %d\n",players[i].symbol,  players[i].segvards, players[i].active );
//        if (players[i].active == 1)
//        {
//            printf("PRINT START GAME\n");
//            send_GAME_START_MSG();
//        }
//    }

    send_GAME_START_MSG();

    // Send map to each
    read_send_game_MAP();

    print_game_map();

    set_players_coordinats();

    print_game_map();

    // Send positions to each
    send_game_update();
    // Start new thread for each player.
}

void set_players_coordinats() {
    int i = 0;
    for (; i < PLAYERS_COUNT; i++)
    {
        if (players[i].active == 1)
        {
            game_map[players[i].y][players[i].x] = players[i].symbol;
        }
    }
}

void print_game_map() {
    int i = 0;
    int j = 0;
    printf("************************************************\n");
    for(; i < MAP_H; i++)
    {
        for (; j < MAP_W; j ++)
        {
            printf("%c", game_map[i][j]);
        }
        printf("\n");
    }
    printf("************************************************\n");
}

void send_game_update()
{
    printf("Sending GAME_UPDATE_MSG to each active player...\n");
    int i = 0;
//    for (; i < client_count; i++)
//    {
//        if (players[i].active == 0)
//        {
//            send_first_GAME_UPDATE_MSG(i);
//        }
//    }
    send_first_GAME_UPDATE_MSG();
    printf("Update sent to each active player.\n");
}

void send_first_GAME_UPDATE_MSG() {
    char* msg_buffer = NULL;
    char* list_of_players_updates = NULL;
  // connection_t * conn;

  //  conn = (connection_t *)players[client_nr].connection;

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

    send_MSG_TO_ALL_ACTIVE(msg_buffer);
   // send_MSG(conn, msg_buffer);

    free(msg_buffer);
    free(list_of_players_updates);
}

void read_send_game_MAP()
{
    static char buffer[MAP_W+1];
    int line_count = 0;
    FILE * fp;

    fp = fopen("./map.txt", "r");
    if (fp == NULL)
    {
        exit(EXIT_FAILURE);
    }

    // First line is 0!
    while(!feof (fp))
    {
        if (fgets(buffer, MAP_W+1, fp))// MAP_W+1, so the \n also will be read!
        {
            send_MAP_line(buffer, line_count);
            add_MAP_line_to_array(buffer, line_count);
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
        printf("Put char %d into array cell {%d}{%d}\n", map_line[i], line_nr, i);
        game_map[line_nr][i] = map_line[i];
    }

    printf("Last i:%d\n", i);
    printf("Last char: %d\n", game_map[line_nr][MAP_W-1]); // MAP_W - 1, cuz starting from 0.
}

void send_MAP_line(char *map_line, int line_nr)
{
    int i = 1;
//    for (; i <= PLAYERS_COUNT; i++)
//    {
//        if (players[i].active == 1)
//        {
//            send_MAP_ROW_MSG(i, map_line, line_nr);
//        }
//    }
    send_MAP_ROW_MSG(map_line, line_nr);
}

//6<rindas_nummurs><kartes_rinda>
void send_MAP_ROW_MSG(char *line, int line_nr)
{
    char* msg_buffer = NULL;
   // connection_t * conn;

  //  conn = players[client_nr].connection;
  //  conn = (connection_t *)client -> connection;

    msg_buffer = (char *)malloc(MSG_MAP_ROW_SIZE + 1);
    //6<rindas_nummurs><kartes_rinda>
    snprintf(msg_buffer, MSG_MAP_ROW_SIZE, MSG_MAP_ROW, line_nr, line);
    printf("Message to send: %s\n",msg_buffer);

   // send_MSG(conn, msg_buffer);

    send_MSG_TO_ALL_ACTIVE(msg_buffer);

    free(msg_buffer);
}

void send_GAME_START_MSG()
{
    printf("SEND GAMESTART MSG START\n");
    char* msg_buffer = NULL;
    char* list_of_players = NULL;
  //  connection_t * conn;

  //  conn = players[client_nr].connection;
    //conn = (connection_t *)client -> connection;

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

  //  if (conn == NULL)
 //   {
 //       printf("Connection is null. GAME START MSG");
 //   }

    send_MSG_TO_ALL_ACTIVE(msg_buffer);
   // send_MSG(conn, msg_buffer);

    free(msg_buffer);
    free(list_of_players);
    printf("SEND GAMESTART MSG END");
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

void process_JOIN_GAME_MSG(int client_nr, char *buffer)
{
    printf("process_JOIN_GAME_MSG: START\n");
    char* seg_name = NULL;
  //  seg_name = (char*)malloc(sizeof(process_income_message(buffer)));
    seg_name = process_income_message(buffer);

    if (name_exist(seg_name) == 1) //Segfault
    {
        printf("USERNAME TAKEN: %s\n", seg_name);
        send_USERNAME_TAKEN_MSG(players[client_nr].connection);
        return;
    }

    players[client_nr].active = 1;
    players[client_nr].segvards = (char *)malloc(16);

    //strcpy( players[client_nr].segvards, buffer);
    players[client_nr].segvards = strdup(seg_name);

    send_LOBBY_INFO_MSG(client_nr);
    printf("process_JOIN_GAME_MSG: END\n");
    client_count++;
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
    long addr = 0;

    printf("Starting new thread for client with socket desc: \n");

    int client_nr = (int)ptr;
    //players[client_nr];
   // client = (struct client *) ptr; // Getting client from the main thread
    conn =  players[client_nr].connection; //  Getting connection from thread

    if(conn == NULL)
    {
        printf("Connectionn is NULL\n");
    }

    printf("Starting new thread for client with socket desc: %d\n", conn->sock_desc);

   // while (1) {
        printf("Waiting for a message...\n");
        read(conn->sock_desc, &len, sizeof(int));

        if (len > 0) {
            buffer = (char *) malloc((len + 1) * sizeof(char));
            buffer[len] = 0;
            read(conn->sock_desc, buffer, len);

            if (buffer[0] == '0') {
                printf("JOIN GAME message is gotten from client with socket desc: %d\n", conn->sock_desc);
                process_JOIN_GAME_MSG(client_nr, buffer);//TODO!
                printf("Client name  %s\n",players[client_nr].segvards);
                printf("Client with sock desc: %d. Joined Game. Clients count %d. \n", conn->sock_desc, client_count);
            }
            free(buffer);
            printf("Buffer is empty!\n");
        }
    //}

    printf("Client name2  %s\n",players[client_nr].segvards);

    printf("Client's thread is over.\n");
   // close(conn->sock_desc);
   // free(conn);
    printf("\n");
    pthread_exit(0);
}

void send_LOBBY_INFO_MSG()
{
    printf("send_LOBBY_INFO_MSG: START\n");

    char* msg_buffer = NULL;
    int len = MSG_LOBBY_INFO_SIZE + (client_count * Ns_PLAYER_NAME_SIZE);
    char* list_of_players = NULL;
 //   connection_t * conn;

  //  conn = (connection_t *)players[client_nr].connection;

//    printf("connection read,\n");

    msg_buffer = (char *)malloc(1000);
    list_of_players = (char *)malloc(client_count * Ns_PLAYER_NAME_SIZE);

    printf("malloc done\n");

    int i = 0;
    char temp[Ns_PLAYER_NAME_SIZE];
    for (; i < client_count - 1; i++)
    {
        sprintf(temp, Ns_PLAYER_NAME, players[i].segvards);
        strcat(list_of_players, temp);
    }

    printf("Players list done\n");

    sprintf(temp, "<%s>", players[client_count - 1].segvards);
    strcat(list_of_players, temp);

    printf("players list done done\n");

    //"2<%d>{%s}"
   sprintf(msg_buffer, MSG_LOBBY_INFO, client_count + 1, list_of_players);
   // sprintf(msg_buffer, MSG_LOBBY_INFO, client_count + 1, "smth");

    //printf("Message to send: %s, on socket %d\n", msg_buffer, players[client_nr].connection->sock_desc);

   // printf("Active: %d Segname %s \n", players[client_nr].active, players[client_nr].segvards);

//    if (conn == NULL)
//    {
//        printf("null connection\n");
//    }

    printf("Message to send: %s\n", msg_buffer);

    send_MSG_TO_ALL_ACTIVE(msg_buffer);
    //send_MSG(conn, msg_buffer);

    free(msg_buffer);
    free(list_of_players);

    printf("send_LOBBY_INFO_MSG: END\n");
}

char * process_income_message(char *buffer) {
    int message_type = buffer[0];
    char *to_return = NULL;
    int i = 0;
    int j = 0;

    if (message_type == MSG_JOIN_GAME_C)
    {
        to_return = (char *)malloc(SEGVARDS_SIZE+1);
        i = 2; //0<%s> skipping first two chars; starting after <
        for (; i < SEGVARDS_SIZE; i++)
        {
            if (buffer[i] == '>')
            {
                break;
            }
            to_return[j] = buffer[i];
            j++;
        }
        to_return[SEGVARDS_SIZE] = '\0';
    }

    return to_return;
}
