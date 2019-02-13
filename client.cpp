#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <strings.h>
#include <arpa/inet.h>
#include <iostream>
#include "thisUser.h"

extern int errno;
using namespace std;
int port;

int main (int argc, char *argv[])
{
    char username[50];
    char* password=new char[50];
    int sd;
    struct sockaddr_in server;
    char msg[1000];
    unsigned int bytesRead;
    string msgrecv;
    if (argc != 3)
    {
        printf ("[client] Sintaxa: %s <adresa_server> <port>\n", argv[0]);
        return -1;
    }
    port = atoi (argv[2]);
    if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror ("[client] Eroare la socket().\n");
        return errno;
    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons (port);
    if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
    {
        perror ("[client]Eroare la connect().\n");
        return errno;
    }
    printf("Sfat: folositi comanda 'help' pentru o introducere in aplicatie!\n");
    while(1) {
        msgrecv.clear();
        bzero(msg, 1000);
        printf("[client]Introduceti o comanda: ");
        fflush(stdout);
        read(0, msg, 1000);
        msg[strlen(msg)-1]='\0';
        if(strcasecmp(msg,"login")==0 || strcasecmp(msg,"register")==0)
        {
            cout<<"Username: ";
            fflush(stdout);
            fgets(username,50,stdin);
            username[strlen(username)-1]='\0';
            password=getpass("Parola: ");
            strcat(msg,"#");
            strcat(msg,username);
            strcat(msg,"#");
            strcat(msg,password);
        }
        else if(strcasecmp(msg,"addsong")==0)
        {
            cout<<"Va rugam sa oferiti urmatoarele informatii despre melodie:\nNume:";
            fflush(stdout);
            char nume[50];
            fgets(nume,50,stdin);
            nume[strlen(nume)-1]='\0';
            cout<<"Artist:";
            fflush(stdout);
            char artist[150];
            fgets(artist,150,stdin);
            artist[strlen(artist)-1]='\0';
            cout<<"Descriere:";
            fflush(stdout);
            char descriere[600];
            fgets(descriere,600,stdin);
            descriere[strlen(descriere)-1]='\0';
            cout<<"Link:";
            fflush(stdout);
            char link[50];
            fgets(link,50,stdin);
            link[strlen(link)-1]='\0';
            cout<<"Genuri, despartite prin virgula, iar spatiile dintre cuvintele aceluiasi gen vor fi marcate cu '-':";
            fflush(stdout);
            char genuri[150];
            fgets(genuri,150,stdin);
            genuri[strlen(genuri)-1]='\0';
            cout<<"Sunteti sigur pe informatiile de mai sus? Daca da, raspundeti cu y, iar daca nu, cu orice altceva.\n";
            fflush(stdout);
            char raspuns[2];
            read(0,raspuns,2);
            if(raspuns[0]!='y')
                continue;
            strcat(msg,"#");
            strcat(msg,nume);
            strcat(msg,"#");
            strcat(msg,artist);
            strcat(msg,"#");
            strcat(msg,descriere);
            strcat(msg,"#");
            strcat(msg,link);
            strcat(msg,"#");
            strcat(msg,genuri);
        }
        if (write(sd, msg, 1000) <= 0) {
            perror("[client]Eroare la write() spre server.\n");
            return errno;
        }
        if(strcmp(msg,"quit")==0)
            break;
        if(read(sd,&bytesRead,4)<0) {
            perror("[client]Eroare la read() de la server.\n");
            return errno;
        }
        msgrecv.resize(bytesRead);
        if (read(sd, msgrecv.data(), msgrecv.size()) < 0) {
            perror("[client]Eroare la read() de la server.\n");
            return errno;
        }
        printf("%s\n", msgrecv.c_str());
    }
    close (sd);
}