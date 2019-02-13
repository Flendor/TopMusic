#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <exception>
#include <map>
#include "thisUser.h"
#include "Song.h"
#include "Songs.h"
#include <string>
#include <stdlib.h>

#define PORT 2981

extern int errno;

using namespace std;
using json=nlohmann::json;

thisUser utilizatori;
Songs songs;
ifstream user_file("../users.json");
json users = json::parse(user_file);

char * conv_addr (struct sockaddr_in address)
{
    static char str[25];
    char port[7];
    strcpy (str, inet_ntoa (address.sin_addr));
    bzero (port, 7);
    sprintf (port, ":%d", ntohs (address.sin_port));
    strcat (str, port);
    return (str);
}

void makeTop()
{
    ifstream song_file("../songs.json");
    json mel=json::parse(song_file);
    song_file.close();
    songs.top.clear();
    for(auto s : mel)
    {
        multimap<string,string> comm;
        map<string,int> myVote;
        vector<string> genuri;
        for(auto c : s["comments"])
        {
            string user=c["user"],content=c["content"];
            comm.insert(pair<string, string>(user,content));
        }
        for(auto v : s["whoVoted"]) {
            string user = v["user"];
            if (v["review"] == "up")
                myVote.insert(pair<string, int>(user, 1));
            else myVote.insert(pair<string, int>(user, -1));
        }
        for(auto g:s["genres"])
        {
            string gen=g;
            genuri.push_back(gen);
        }
        auto voteString=s["nrVotes"].get<string>();
        int voturi=stoi(voteString);
        string nume=s["name"];
        string descriere=s["description"];
        string singer=s["artist"];
        string url=s["link"];
        string user=s["proposedBy"];
        Song *temp= new Song(nume,descriere,singer,genuri,url,voturi,comm,myVote,user);
        songs.addSong(temp);
    }
    songs.sortVoturi();
}

void updateJson();

void addSong(char msg[], string& msgrasp)
{
    string mesaj=msg;
    char *secv;
    string nume;
    string artist;
    string descriere;
    string link;
    int nr=count(mesaj.begin(),mesaj.end(),'#');
    if(nr>=6)
    {
        msgrasp+="Caracterul '#' nu este permis!\n";
        return ;
    }
    char* genres=new char[100];
    secv=strtok(msg,"#");
    short i=0;
    while(secv!=NULL)
    {
        if(i==1)
            nume=secv;
        else if(i==2)
            artist=secv;
        else if(i==3)
            descriere=secv;
        else if(i==4)
            link=secv;
        else if(i==5)
            genres=secv;
        i++;
        secv = strtok (NULL, "#");
    }
    vector<string> genuri;
    secv=strtok(genres,", ");
    while(secv!=NULL)
    {
        genuri.push_back(secv);
        secv=strtok(NULL,", ");
    }
    multimap<string,string> comms;
    map<string,int> myVote;
    string user=utilizatori.getName();
    int nrVoturi=0;
    Song* newSong=new Song(nume,descriere,artist,genuri,link,nrVoturi,comms,myVote,user);
    songs.top.push_back(newSong);
    updateJson();
}

void updateJson()
{
    json newmel;
    newmel.clear();
    for(unsigned int i=1;i<=songs.getSize();i++)
    {
        string nr=to_string(i);
        newmel[nr]["name"]=songs.top[i-1]->getName();
        newmel[nr]["description"]=songs.top[i-1]->getDescription();
        newmel[nr]["artist"]=songs.top[i-1]->getArtist();
        for(unsigned int j=1;j<=songs.top[i-1]->getGenres().size();j++) {
            string altnr=to_string(j);
            newmel[nr]["genres"][altnr] = songs.top[i-1]->getGenres()[j - 1];
        }
        newmel[nr]["link"]=songs.top[i-1]->getUrl();
        newmel[nr]["nrVotes"]=to_string(songs.top[i-1]->howManyVotes());
        unsigned int j;
        j=1;
        for(auto iterator=songs.top[i-1]->getComments().begin();iterator!=songs.top[i-1]->getComments().end();iterator++)
        {
            string altnr=to_string(j);
            newmel[nr]["comments"][altnr]["user"]=iterator->first;
            newmel[nr]["comments"][altnr]["content"]=iterator->second;
            j++;
        }
        j=1;
        for(auto it=songs.top[i-1]->getVotes().begin();it!=songs.top[i-1]->getVotes().end();it++)
        {
            string altnr=to_string(j);
            newmel[nr]["whoVoted"][altnr]["user"]=it->first;
            if(it->second==1)
                newmel[nr]["whoVoted"][altnr]["review"]="up";
            else newmel[nr]["whoVoted"][altnr]["review"]="down";
            j++;
        }
        newmel[nr]["proposedBy"]=songs.top[i-1]->getUser();
    }
    ofstream song_filewr("../songs.json");
    song_filewr<<newmel.dump(2);
    song_filewr.close();
}

void genres(string &msgrasp)
{
    ifstream song_file2("../songs.json");
    json forGen = json::parse(song_file2);
    vector<string>genuri;
    msgrasp+='\n';
    for (auto s : forGen)
        for (auto &g : s["genres"]) {
            auto it=find(genuri.begin(),genuri.end(),g.get<string>());
            if(it==genuri.end()) {
                msgrasp += g.get<string>();
                msgrasp += '\n';
                genuri.push_back(g.get<string>());
            }
        }
    song_file2.close();
}

void addComment(char msg[],string &msgrasp)
{
    char *secv;
    string gen;
    string comm;
    char *arg=new char[50];
    secv=strtok(msg,":");
    short i=0;
    while(secv!=NULL)
    {
        if(i==0)
            arg=secv;
        else comm=secv;
        i++;
        secv = strtok (NULL, ":");
    }
    i=0;
    int s=0;
    bool ok=true;
    secv = strtok (arg," ");
    while (secv != NULL)
    {
        if(i==1) {
            for(unsigned int j=0;j<strlen(secv);j++)
                if(!isdigit(secv[j]))
                    ok=false;
            if(ok)
                s = atoi(secv);
        }
        else if(i==2)
            gen=secv;
        secv = strtok (NULL, " ");
        i++;
    }
    if(i>3)
    {
        msgrasp+="Specificati un singur gen!\n";
        return ;
    }
    if(s==0 || !ok)
    {    msgrasp+="Specificati o pozitie valida din top!"; msgrasp+='\n';}
    else{
        if(gen.empty()||gen=="all")
        {
            songs.addComment(s,utilizatori.getName(),comm,msgrasp);
            updateJson();
        }
        else
        {
            songs.selectGenre(gen);
            songs.addGenreComment(s,utilizatori.getName(),comm,msgrasp);
            updateJson();
        }
    }
}

void upvote(char msg[], string &msgrasp)
{
    char *secv;
    string gen;
    short i=0;
    int s=0;
    bool ok=true;
    secv = strtok (msg," ");
    while (secv != NULL)
    {
        if(i==1) {
            for(unsigned int j=0;j<strlen(secv);j++)
                if(!isdigit(secv[j]))
                    ok=false;
            if(ok)
                s = atoi(secv);
        }
        else if(i==2)
            gen=secv;
        secv = strtok (NULL, " ");
        i++;
    }
    if(i>3)
    {
        msgrasp+="Specificati un singur gen!";
        return ;
    }
    if(s==0 || !ok)
    {    msgrasp+="Specificati o pozitie valida din top!"; msgrasp+='\n';}
    else{
        if(gen.empty()||gen=="all")
            songs.upVote(s, utilizatori.getName(), msgrasp);
        else
        {
            songs.selectGenre(gen);
            songs.upVoteGenre(s, utilizatori.getName(), msgrasp);
        }
        updateJson();
    }
}

void downvote(char msg[], string &msgrasp)
{
    char *secv;
    string gen;
    short i=0;
    int s=0;
    bool ok=true;
    secv = strtok (msg," ");
    while (secv != NULL)
    {
        if(i==1) {
            for(unsigned int j=0;j<strlen(secv);j++)
                if(!isdigit(secv[j]))
                    ok=false;
            if(ok)
                s = atoi(secv);
        }
        else if(i==2)
            gen=secv;
        secv = strtok (NULL, " ");
        i++;
    }
    if(i>3)
    {
        msgrasp+="Specificati un singur gen!";
        return ;
    }
    if(s==0 || !ok)
    {    msgrasp+="Specificati o pozitie valida din top!"; msgrasp+='\n';}
    else{
        if(gen.empty()||gen=="all")
            songs.downVote(s, utilizatori.getName(), msgrasp);
        else
        {
            songs.selectGenre(gen);
            songs.downVoteGenre(s, utilizatori.getName(), msgrasp);
        }
        updateJson();
    }
}

void changevote(char msg[], string &msgrasp)
{
    char *secv;
    string gen;
    short i=0;
    int s=0;
    bool ok=true;
    secv = strtok (msg," ");
    while (secv != NULL)
    {
        if(i==1) {
            for(unsigned int j=0;j<strlen(secv);j++)
                if(!isdigit(secv[j]))
                    ok=false;
            if(ok)
                s = atoi(secv);
        }
        else if(i==2)
            gen=secv;
        secv = strtok (NULL, " ");
        i++;
    }
    if(i>3)
    {
        msgrasp+="Specificati un singur gen!";
        return ;
    }
    if(s==0 || !ok)
    {    msgrasp+="Specificati o pozitie valida din top!"; msgrasp+='\n';}
    else{
        if(gen.empty()||gen=="all")
            songs.changeVote(s, utilizatori.getName(), msgrasp);
        else
        {
            songs.selectGenre(gen);
            songs.changeVoteGenre(s, utilizatori.getName(), msgrasp);
        }
        updateJson();
    }
}

void whichTop(char msg[], string &msgrasp)
{
    msgrasp+='\n';
    char *secv;
    string gen;
    short i=0;
    secv = strtok (msg," ");
    while (secv != NULL)
    {
        if(i==1)
            gen=secv;
        secv = strtok (NULL, " ");
        i++;
    }
    if(i>2)
        msgrasp+="Specificati un singur gen!";
    else {
        if (gen.empty() || gen == "all")
            songs.showTop(msgrasp);
        else {
            songs.selectGenre(gen);
            songs.showGenreTop(msgrasp);
        }
    }
}

void whichInfo(char msg[],string &msgrasp)
{
    msgrasp+='\n';
    char *secv;
    string gen;
    short i=0;
    int s=0;
    bool ok=true;
    secv = strtok (msg," ");
    while (secv != NULL)
    {
        if(i==1) {
            for(unsigned int j=0;j<strlen(secv);j++)
                if(!isdigit(secv[j]))
                    ok=false;
            if(ok)
                s = atoi(secv);
        }
        else if(i==2)
            gen=secv;
        secv = strtok (NULL, " ");
        i++;
    }
    if(i>3)
    {
        msgrasp+="Specificati un singur gen!";
        return ;
    }
    if(s==0 || !ok)
    {    msgrasp+="Specificati o pozitie valida din top!"; msgrasp+='\n';}
    else{
        if(gen.empty()||gen=="all")
            songs.showInfo(s,msgrasp);
        else
        {
            songs.selectGenre(gen);
            songs.showGenreInfo(s,msgrasp);
        }
    }
}

void deletesong(char msg[],string &msgrasp)
{
    if(!utilizatori.isAdmin())
    {
        msgrasp+="Nu aveti dreptul la aceasta comanda!\n";
        return;
    }
    char *secv;
    string gen;
    short i=0;
    int s=0;
    bool ok=true;
    secv = strtok (msg," ");
    while (secv != NULL)
    {
        if(i==1) {
            for(unsigned int j=0;j<strlen(secv);j++)
                if(!isdigit(secv[j]))
                    ok=false;
            if(ok)
                s = atoi(secv);
        }
        else if(i==2)
            gen=secv;
        secv = strtok (NULL, " ");
        i++;
    }
    if(i>3)
    {
        msgrasp+="Specificati un singur gen!\n";
        return ;
    }
    if(s==0 || !ok)
    {    msgrasp+="Specificati o pozitie valida din top!"; msgrasp+='\n';}
    else{
        if(gen.empty()||gen=="all")
            songs.top.erase(songs.top.begin()+s-1);
        else
        {
            songs.selectGenre(gen);
            songs.deleteGenreSong(s, msgrasp);
        }
        updateJson();
    }
    msgrasp+="Operatiune incheiata!\n";
}

void makeadmin(char msg[], string &msgrasp)
{
    if(!utilizatori.isAdmin())
    {
        msgrasp+="Doar administratorii pot da drepturi de administrator altor utilizatori!";
        return;
    }
    char *secv;
    string user;
    short i=0;
    secv = strtok (msg," ");
    while (secv != NULL)
    {
        if(i==1)
            user=secv;
        secv = strtok (NULL, " ");
        i++;
    }
    if(i!=2)
        msgrasp+="Specificati un user, un singur user!";
    else {
        if (users.find(user)==users.end())
            msgrasp+="Acest user nu exista!";
        else {
            if(users[user]["tipUser"]=="admin")
                msgrasp+="Este deja administrator!";
            else {
                users[user]["tipUser"] = "admin";
                ofstream user_filewr("../users.json");
                user_filewr<<users.dump(2);
                user_filewr.close();
                msgrasp += "Operatiune incheiata!\n";
            }
        }
    }
}

void restrict(char msg[], string &msgrasp)
{
    if(!utilizatori.isAdmin())
    {
        msgrasp+="Doar administratorii pot restrictiona dreptul altor utilizatori!";
        return;
    }
    char *secv;
    string user;
    short i=0;
    secv = strtok (msg," ");
    while (secv != NULL)
    {
        if(i==1)
            user=secv;
        secv = strtok (NULL, " ");
        i++;
    }
    if(i!=2)
        msgrasp+="Specificati un user, un singur user!";
    else {
        if (users.find(user)==users.end())
            msgrasp+="Acest user nu exista!";
        else {
            if(users[user]["tipUser"]=="admin")
                msgrasp+="Nu puteti restrictiona drepturile altui administrator!";
            else {
                users[user]["restrictionat"] = "da";
                ofstream user_filewr("../users.json");
                user_filewr<<users.dump(2);
                user_filewr.close();
                msgrasp += "Operatiune incheiata!\n";
            }
        }
    }
}

void unrestrict(char msg[], string &msgrasp)
{
    if(!utilizatori.isAdmin())
    {
        msgrasp+="Doar administratorii pot reda drepturile altor utilizatori!";
        return;
    }
    char *secv;
    string user;
    short i=0;
    secv = strtok (msg," ");
    while (secv != NULL)
    {
        if(i==1)
            user=secv;
        secv = strtok (NULL, " ");
        i++;
    }
    if(i!=2)
        msgrasp+="Specificati un user, un singur user!";
    else {
        if (users.find(user)==users.end())
            msgrasp+="Acest user nu exista!";
        else {
            if(users[user]["restrictionat"]=="nu")
                msgrasp+="Acest user are deja toate drepturile!";
            else {
                users[user]["restrictionat"] = "nu";
                ofstream user_filewr("../users.json");
                user_filewr<<users.dump(2);
                user_filewr.close();
                msgrasp += "Operatiune incheiata!\n";
            }
        }
    }
}

void registerFunc(char msg[], string &msgrasp)
{
    char *secv;
    char user[50];
    char pass[50];
    char i=0;
    string mesaj=msg;
    int nr=count(mesaj.begin(),mesaj.end(),'#');
    if(nr>=3)
    {
        msgrasp+="Caracterul '#' nu este permis in user sau parola!";
        return ;
    }
    secv = strtok (msg,"#");
    while (secv != NULL)
    {
        if(i==1)
            strcpy(user,secv);
        else if(i==2)
            strcpy(pass,secv);
        secv = strtok (NULL, "#");
        i++;
    }
    if (users.find(user) != users.end())
    {
        msgrasp="User deja existent!";
    }
    else{
        users[user]["password"]=pass;
        users[user]["tipUser"]="regular";
        users[user]["restrictionat"]="nu";
        msgrasp="Utilizatorul a fost inregistrat cu succes!";
        ofstream user_filewr("../users.json");
        user_filewr<<users.dump(2);
        user_filewr.close();
    }
}

void loginFunc(char msg[], string &msgrasp)
{
    char *secv;
    char user[50];
    char pass[50];
    char i=0;
    secv = strtok (msg,"#");
    while (secv != NULL)
    {
        if(i==1)
            strcpy(user,secv);
        else if(i==2)
            strcpy(pass,secv);
        secv = strtok (NULL, "#");
        i++;
    }
    if (users.find(user) != users.end())
    {
        auto userString=users[user]["password"].get<string>();
        if(pass==userString)
        {
            if(users[user]["tipUser"]=="regular")
            {
                if(users[user]["restrictionat"]=="da") {
                    msgrasp = "V-ati autentificat cu succes, dar aveti drepturi restrictionate!";
                    utilizatori.setProp(user, false, true);
                } else
                {
                    msgrasp = "V-ati autentificat cu succes!";
                    utilizatori.setProp(user, false, false);
                }
            }
            else
            {
                msgrasp="V-ati autentificat cu succes ca administrator!";
                utilizatori.setProp(user,true,false);
            }
        }
        else msgrasp= "Parola incorecta! Incercati din nou!";
    }
    else msgrasp= "Username-ul nu exista! Va puteti inregistra cu comanda 'register'!";
}

void help(string &msgrasp)
{
    msgrasp+="\nBun venit pe aplicatia TopMusic! Aici, puteti afla informatii despre melodii de care nu ati mai auzit, ";
    msgrasp+="si de asemenea va puteti da cu parerea asupra muzicii pe care o gasiti, prin voturi si comentarii! ";
    msgrasp+="Daca aveti o melodie preferata si nu exista in top, puteti oricand sa o adaugati si va putea fi vazuta de ceilalti utilizatori. ";
    msgrasp+="Orice comentarii neadecvate sau melodii adaugate cu informatii incorecte sau care reprezinta o bataie de joc, ";
    msgrasp+="vor fi pedepsite cu restrictionarea drepturilor de vot, de a adauga comentarii sau melodii la top pentru userul respectiv, ";
    msgrasp+="atat timp cat considera administratorii. Veti putea vedea in continuare topul si informatiile despre melodii. ";
    msgrasp+="De asemenea, administratorii pot sterge melodii din top. ";
    msgrasp+="In mod evident, trebuie sa fiti logat pentru a avea acces la orice comanda legata de melodii.\n\n";
    msgrasp+="Mai jos, aveti o lista cu comenzile pe care le puteti da:\n";
    msgrasp+="1. 'login' pentru autentificare: comanda va cere username-ul si parola dumneavoastra.\n";
    msgrasp+="2. 'register' pentru inregistrare: de asemenea, va vor fi cerute un username si o parola pentru cont. Username-ul nu trebuie sa existe deja.\n";
    msgrasp+="3. 'help' va afisa aceleasi informatii pe care le vedeti acum, in caz ca nu va amintiti o comanda.\n";
    msgrasp+="4. 'quit' va deconecteaza de tot de la server.\n";
    msgrasp+="Urmatoarele comenzi necesita neaparat sa fiti autentificat:\n";
    msgrasp+="5. 'logout' va permite sa va delogati din contul curent.\n";
    msgrasp+="6. 'whoami' va afisa username-ul contului pe care sunteti logat in momentul respectiv.\n";
    msgrasp+="7. 'genres' va afisa toate genurile existente in aplicatie.\n";
    msgrasp+="8. 'top <gen>' afiseaza in ordine, de la cele mai multe voturi la cele mai putine, toate melodiile din genul specificat. ";
    msgrasp+="Campul <gen> trebuie sa contina un singur gen, dar poate contine si cuvantul 'all', sau poate fi chiar gol in cazul in care doriti ";
    msgrasp+="topul tuturor melodiilor, indiferent de gen.\n";
    msgrasp+="9. 'showinfo <numar_ordine> <gen>' afiseaza toate informatiile despre o melodie specificata astfel: ";
    msgrasp+="melodia cu numarul de ordine specificat din topul celor de genul <gen>. De asemenea, campul <gen> poate contine cuvantul 'all' sau poate fi gol\n";
    msgrasp+="10. 'addcomment <numar_ordine> <gen>:<continut>' adauga un comentariu la melodia specificata, cu continutul dat.\n";
    msgrasp+="11. 'upvote <numar_ordine> <gen>' creste cu 1 numarul voturilor pentru melodia specificata, daca nu a fost deja votata de dumneavoastra.\n";
    msgrasp+="12. 'downvote <numar_ordine> <gen>' scade cu 1 numarul voturilor pentru melodia specificata, daca nu a fost deja votata de dumneavoastra.\n";
    msgrasp+="13. 'changevote <numar_ordine> <gen>' schimba votul pentru melodia specificata din up in down sau invers.\n";
    msgrasp+="14. 'addsong' adauga o noua melodie la top. Va vor fi cerute toate informatiile despre melodia pe care doriti sa o adaugati. ";
    msgrasp+="Fiti responsabil, un administrator va putea restrictiona drepturile dumneavoastra si va putea sterge melodia oricand.\n";
    msgrasp+="Urmatoarele comenzi pot fi date doar de administratori:\n";
    msgrasp+="15. 'delete <numar_ordine> <gen>' va sterge melodie specificata din top.\n";
    msgrasp+="16. 'restrict <username>' va restrictiona drepturile de a adauga comentarii, de a vota si de a adauga melodii in top pentru utilizatorul specificat.\n";
    msgrasp+="17. 'unrestrict <username>' va reda drepturile utilizatorului specificat.\n";
    msgrasp+="18. 'makeadmin <username>' va da drepturi de administrator utilizatorului specificat.\n\n";
    msgrasp+="Spatiile inutile de la inceputul sau sfarsitul comenzilor, numelor de utilizatori si a parolelor vor duce la nerecunoasterea acestora.\n";
    msgrasp+="Acestea sunt toate comenzile. Va rugam sa le folositi in mod responsabil, respectati sintaxa si va uram distractie placuta!\n";
}

int getFunc(int fd)
{
    char buffer[1000];
    int bytes;
    char msg[1000];
    string msgrasp;
    bytes = read(fd, msg, sizeof(buffer));
    if (bytes < 0) {
        perror("Eroare la read() de la client.\n");
        return 0;
    }
    if(bytes==0)
        return -1;
    printf("[server]Mesajul a fost receptionat...%s\n", msg);
    int descriptor=open("../songs.json",O_RDWR);
    struct flock lacat;
    lacat.l_type=F_RDLCK;
    lacat.l_whence=SEEK_SET;
    lacat.l_start=0;
    lacat.l_len=0;
    struct flock deblocare;
    deblocare.l_type=F_UNLCK;
    deblocare.l_whence=SEEK_SET;
    deblocare.l_start=0;
    deblocare.l_len=0;
    if(-1==(fcntl(descriptor,F_SETLKW,&lacat)))
    {
        perror("A aparut o eroare la punerea lacatului.\n");
        return errno;
    }
    makeTop();
    ifstream user_file2("../users.json");
    users = json::parse(user_file2);
    user_file2.close();
    if((users[utilizatori.getName()]["restrictionat"]=="nu" && utilizatori.isRestricted()) || (users[utilizatori.getName()]["restrictionat"]=="da" && !utilizatori.isRestricted()))
        utilizatori.setRestrict(!utilizatori.isRestricted());
    if(users[utilizatori.getName()]["tipUser"]=="admin" && !utilizatori.isAdmin())
        utilizatori.setAdmin(true);
    if(strcasecmp(msg,"help")==0)
        help(msgrasp);
    else if(strncasecmp(msg,"login#",6)==0)
    {
        if(utilizatori.loggedIn())
            msgrasp="Sunteti deja autentificat! Folositi comanda 'logout' pentru a va deloga.\n";
        else
            loginFunc(msg,msgrasp);
    }
    else if(strcasecmp(msg,"logout")==0)
    {
        if(!utilizatori.loggedIn())
            msgrasp="Nu sunteti logat!\n";
        else
        {
            msgrasp= "V-ati delogat cu succes!\n";
            utilizatori.logIn=false;
        }
    }
    else if(strncasecmp(msg,"register#",9)==0)
    {
        if(utilizatori.loggedIn())
            msgrasp="Sunteti deja autentificat! Folositi comanda 'logout' pentru a va deloga.\n";
        else registerFunc(msg,msgrasp);
    }
    else if(strcasecmp(msg,"quit")==0)
        return -1;
    else {
        if (!utilizatori.loggedIn())
            msgrasp=
                   "Trebuie sa va autentificati cu comanda 'login' sau sa creati un cont cu comanda 'register'!\n";
        else if (strncasecmp(msg, "top", 3) == 0 && (msg[3]==' ' || msg[3]=='\0'))
            whichTop(msg,msgrasp);
        else if (strncasecmp(msg,"showinfo ",9)==0)
            whichInfo(msg,msgrasp);
        else if(strncasecmp(msg,"addsong#",8)==0)
        {
            if(utilizatori.isRestricted())
                msgrasp="Nu aveti dreptul de a comenta, de a vota, sau de a adauga continut de orice fel in aplicatie!\n";
            else{
                addSong(msg, msgrasp);
                msgrasp += "Operatiune incheiata!\n";}
        }
        else if(strcasecmp(msg,"genres")==0)
            genres(msgrasp);
        else if(strncasecmp(msg, "restrict ", 9)==0)
            restrict(msg,msgrasp);
        else if(strncasecmp(msg, "unrestrict ", 11)==0)
            unrestrict(msg,msgrasp);
        else if(strcasecmp(msg,"whoami")==0) {
            msgrasp = utilizatori.getName();
            msgrasp += '\n';
        }
        else if(strncasecmp(msg, "delete ", 7)==0)
            deletesong(msg,msgrasp);
        else if (strncasecmp(msg, "makeadmin ", 10)==0)
            makeadmin(msg,msgrasp);
        else if (strncasecmp(msg, "addcomment ", 11) == 0) {
            if(utilizatori.isRestricted())
                msgrasp="Nu aveti dreptul de a comenta, de a vota, sau de a adauga continut de orice fel in aplicatie!\n";
            else{
            addComment(msg, msgrasp);
            msgrasp += "Operatiune incheiata!\n";}
        } else if (strncasecmp(msg, "upvote ", 7) == 0) {
            if(utilizatori.isRestricted())
                msgrasp="Nu aveti dreptul de a comenta, de a vota, sau de a adauga continut de orice fel in aplicatie!\n";
            else{
            upvote(msg, msgrasp);
            msgrasp += "Operatiune incheiata!\n";}
        } else if (strncasecmp(msg, "downvote ", 9) == 0) {
            if(utilizatori.isRestricted())
                msgrasp="Nu aveti dreptul de a comenta, de a vota, sau de a adauga continut de orice fel in aplicatie!\n";
            else{
            downvote(msg, msgrasp);
            msgrasp += "Operatiune incheiata!\n";}
        } else if (strncasecmp(msg, "changevote ", 11) == 0) {
            if(utilizatori.isRestricted())
                msgrasp="Nu aveti dreptul de a comenta, de a vota, sau de a adauga continut de orice fel in aplicatie!\n";
            else{
            changevote(msg, msgrasp);
            msgrasp += "Operatiune incheiata!\n";}
        }
        else msgrasp = "Comanda necunoscuta! Incercati din nou sau folositi comanda 'help' pentru a va reaminti sintaxa comenzilor!\n";
        printf("[server]Trimitem mesajul inapoi...\n");
    }
    if(-1==(fcntl(descriptor,F_SETLKW,&deblocare)))
    {
        perror("A aparut o eroare la punerea lacatului.\n");
        return errno;
    }
    unsigned long bytesWrite=msgrasp.size();
    if (bytes && write(fd, &bytesWrite, 4) < 0) {
        perror("[server] Eroare la write() catre client.\n");
        return 0;
    }
    if (bytes && write(fd, msgrasp.c_str(), bytesWrite) < 0) {
        perror("[server] Eroare la write() catre client.\n");
        return 0;
    }
    return bytes;
}

int main ()
{
    struct sockaddr_in server;
    struct sockaddr_in from;
    fd_set readfds;
    fd_set actfds;
    struct timeval tv;
    int sd, client;
    int optval=1;
    int nfds;
    unsigned int len;
    if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror ("[server] Eroare la socket().\n");
        return errno;
    }
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR,&optval,sizeof(optval));
    bzero (&server, sizeof (server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl (INADDR_ANY);
    server.sin_port = htons (PORT);
    if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
    {
        perror ("[server] Eroare la bind().\n");
        return errno;
    }
    if (listen (sd, 5) == -1)
    {
        perror ("[server] Eroare la listen().\n");
        return errno;
    }
    FD_ZERO (&actfds);
    FD_SET (sd, &actfds);
    FD_SET (0,&actfds);
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    nfds = sd;
    printf ("[server] Asteptam la portul %d...\n", PORT);
    fflush (stdout);
    while (1)
    {
        bcopy ((char *) &actfds, (char *) &readfds, sizeof (readfds));
        if (select (nfds+1, &readfds, NULL, NULL, &tv) < 0)
        {
            perror ("[server] Eroare la select().\n");
            return errno;
        }
        if (FD_ISSET(0,&readfds))
        {
           break;
        }
        if (FD_ISSET (sd, &readfds))
        {
            len = sizeof (from);
            bzero (&from, sizeof (from));
            client = accept (sd, (struct sockaddr *) &from, &len);
            if (client < 0)
            {
                perror ("[server] Eroare la accept().\n");
                continue;
            }
            if (nfds < client)
                nfds = client;
            FD_SET (client, &actfds);
            printf("[server] S-a conectat un client de la adresa %s.\n", conv_addr (from));
            fflush (stdout);
            pid_t pid_fiu;
            if((pid_fiu=fork())==-1)
            {
                perror("[server] Eroare la fork");
                return errno;
            }
            else if(pid_fiu==0)
            {
                close(sd);
                FD_CLR(sd,&actfds);
                while(1)
                    if (getFunc(client)==-1) {
                        if(utilizatori.loggedIn())
                            printf("[server] S-a deconectat clientul cu userul %s.\n", utilizatori.getName().c_str());
                        else printf("[server] S-a deconectat un client nelogat!\n");
                        fflush(stdout);
                        utilizatori.logIn=false;
                        close(client);
                        FD_CLR (client, &actfds);
                        break;
                    }
                exit(0);
            } else
            {
                close(client);
                FD_CLR(client,&actfds);
                while(waitpid(-1,NULL,WNOHANG));
                continue;
            }
        }
    }
}
