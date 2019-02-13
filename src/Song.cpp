#include "Song.h"
#include <string>
#include <iostream>
using namespace std;

Song::Song()
{
    votes=0;
}

Song::Song(string nume, string descriere, string singer, vector<string> genuri, string url, int voturi, multimap<string,string> comentarii,map<string,int>votulMeu, string user)
{
    name=nume;
    description=descriere;
    artist=singer;
    genres=genuri;
    link=url;
    comments=comentarii;
    votes=voturi;
    myVote=votulMeu;
    proposedBy=user;
}

void Song::upVote(string user)
{
    votes++;
    myVote.insert(pair<string,int>(user,1));
}

void Song::downVote(string user)
{
    votes--;
    myVote.insert(pair<string,int>(user,-1));
}

void Song::addComment(string user,string comm)
{
    comments.insert(pair<string,string>(user,comm));
}

int Song::howManyVotes()
{
    return votes;
}

string Song::getName()
{
    return name;
}

string Song::getArtist()
{
    return artist;
}

vector<string> &Song::getGenres()
{
    return genres;
}

void Song::showInfo(string &msgrasp)
{
    msgrasp+="Nume: "; msgrasp+=name; msgrasp+='\n';
    msgrasp+="Descriere: "; msgrasp+=description; msgrasp+='\n';
    msgrasp+="Artist: "; msgrasp+=artist; msgrasp+='\n';
    msgrasp+="Genuri: ";
    for(unsigned int i=0;i<genres.size();i++) {
        msgrasp += genres[i]; msgrasp+="; ";
    }
    msgrasp+='\n';
    msgrasp+="Link catre melodie: "; msgrasp+=link; msgrasp+='\n';
    msgrasp+=to_string(howManyVotes()); msgrasp+=" voturi"; msgrasp+='\n';
    msgrasp+=to_string(comments.size()); msgrasp+=" Comentarii:\n";
    for(auto it=comments.begin();it!=comments.end();it++)
    {
        msgrasp+=it->first; msgrasp+=" : "; msgrasp+=it->second; msgrasp+='\n';
    }
    msgrasp+="Melodie propusa de: ";
    msgrasp+=proposedBy;
    msgrasp+='\n';
}

string Song::getDescription()
{
    return description;
}

string Song::getUrl()
{
    return link;
}

multimap<string,string> &Song::getComments()
{
    return comments;
}

map<string,int> &Song::getVotes()
{
    return myVote;
}

string Song::getUser()
{
    return proposedBy;
}

void Song::changeVote(string user)
{
    if(myVote[user]==1)
    {
        votes-=2;
        myVote[user]=-1;
    }
    else
    {
        votes+=2;
        myVote[user]=1;
    }
}

Song::~Song()
{

}

