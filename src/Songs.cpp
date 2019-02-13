#include "Songs.h"
#include <exception>
#include <iostream>

Songs::Songs()
{

}

bool caseEquals(const string& a, const string& b)
{
    unsigned int size = a.size();
    if (b.size() != size)
        return false;
    for (unsigned int i = 0; i < size; ++i)
        if (toupper(a[i]) != toupper(b[i]))
            return false;
    return true;
}

void Songs::addSong(Song* s)
{
    top.push_back(s);
}

void Songs::sortVoturi()
{
    for(unsigned int i=0;i<top.size()-1;i++)
        for(unsigned int j=i+1;j<top.size();j++)
            if(top[i]->howManyVotes()<top[j]->howManyVotes())
            {
                Song* temp;
                temp=top[i];
                top[i]=top[j];
                top[j]=temp;
            }

}

void Songs::afiseaza(string &msgrasp)
{
    for(auto &s : top)
        s->showInfo(msgrasp);
}

void Songs::selectGenre(string genre)
{
    topGenre.clear();
    for(auto &s : top)
        for(auto &g : s->getGenres())
            if(caseEquals(g,genre))
                topGenre.push_back(s);
}

void Songs::showTop(string &msgrasp)
{
    for (unsigned int i = 0; i < top.size(); i++)
    {
        char temp[500];
        sprintf(temp,"%d. %s by %s (%d)\n",i+1,top[i]->getName().c_str(),top[i]->getArtist().c_str(),top[i]->howManyVotes());
        msgrasp+=temp;
    }
}

void Songs::showGenreTop(string &msgrasp)
{
    if(topGenre.empty())
        msgrasp="Nu exista melodii cu acest gen in top!";
    else
        for(unsigned int i=0;i<topGenre.size();i++)
        {
            char temp[200];
            sprintf(temp,"%d. %s by %s (%d)\n",i+1,topGenre[i]->getName().c_str(),topGenre[i]->getArtist().c_str(),topGenre[i]->howManyVotes());
            msgrasp+=temp;
        }
}

void Songs::showInfo(int s, string &msgrasp)
{
    if(s>top.size())
    {   msgrasp+="Nu exista aceasta melodie in top!"; msgrasp+='\n';}
    else top[s-1]->showInfo(msgrasp);
}

void Songs::showGenreInfo(int s, string &msgrasp)
{
    if(topGenre.empty())
    {    msgrasp+="Nu exista melodii de acest gen in top!"; msgrasp+='\n';}
    else{
        if(s>topGenre.size())
        {
            msgrasp+="Nu exista aceasta melodie in topul celor de acest gen!"; msgrasp+='\n';
        }
        else topGenre[s-1]->showInfo(msgrasp);
    }
}

unsigned int Songs::getSize()
{
    return top.size();
}

void Songs::addComment(int s, string user, string comm, string &msgrasp)
{
    if(s>top.size())
    {   msgrasp+="Nu exista aceasta melodie in top!"; msgrasp+='\n';}
    else top[s-1]->addComment(user,comm);
}

void Songs::addGenreComment(int s,string user,string comm, string &msgrasp)
{
    if(topGenre.empty())
    {    msgrasp+="Nu exista melodii de acest gen in top!"; msgrasp+='\n';}
    else{
        if(s>topGenre.size())
        {
            msgrasp+="Nu exista aceasta melodie in topul celor de acest gen!"; msgrasp+='\n';
        }
        else topGenre[s-1]->addComment(user,comm);
    }
}

void Songs::upVote(int s, string user, string &msgrasp)
{
    if(s>top.size())
    {   msgrasp+="Nu exista aceasta melodie in top!"; msgrasp+='\n';}
    else {
        auto it = top[s - 1]->getVotes().find(user);
        if (it != top[s - 1]->getVotes().end())
            msgrasp+="Ati votat deja aceasta melodie!\n";
        else top[s-1]->upVote(user);
    }
}

void Songs::upVoteGenre(int s, string user, string &msgrasp)
{
    if(topGenre.empty())
    {    msgrasp+="Nu exista melodii de acest gen in top!"; msgrasp+='\n';}
    else{
        if(s>topGenre.size())
        {
            msgrasp+="Nu exista aceasta melodie in topul celor de acest gen!"; msgrasp+='\n';
        }
        else
        {
            auto it = topGenre[s - 1]->getVotes().find(user);
            if (it != topGenre[s - 1]->getVotes().end())
                msgrasp+="Ati votat deja aceasta melodie!\n";
            else topGenre[s-1]->upVote(user);
        }
    }
}

void Songs::downVote(int s, string user, string &msgrasp)
{
    if(s>top.size())
    {   msgrasp+="Nu exista aceasta melodie in top!"; msgrasp+='\n';}
    else {
        auto it = top[s - 1]->getVotes().find(user);
        if (it != top[s - 1]->getVotes().end())
            msgrasp+="Ati votat deja aceasta melodie!\n";
        else top[s-1]->downVote(user);
    }
}

void Songs::downVoteGenre(int s, string user, string &msgrasp)
{
    if(topGenre.empty())
    {    msgrasp+="Nu exista melodii de acest gen in top!"; msgrasp+='\n';}
    else{
        if(s>topGenre.size())
        {
            msgrasp+="Nu exista aceasta melodie in topul celor de acest gen!"; msgrasp+='\n';
        }
        else
        {
            auto it = topGenre[s - 1]->getVotes().find(user);
            if (it != topGenre[s - 1]->getVotes().end())
                msgrasp+="Ati votat deja aceasta melodie!\n";
            else topGenre[s-1]->downVote(user);
        }
    }
}

void Songs::changeVote(int s, string user, string &msgrasp)
{
    if(s>top.size())
    {   msgrasp+="Nu exista aceasta melodie in top!"; msgrasp+='\n';}
    else {
        auto it = top[s - 1]->getVotes().find(user);
        if (it == top[s - 1]->getVotes().end())
            msgrasp+="Nu ati votat aceasta melodie!\n";
        else top[s-1]->changeVote(user);
    }
}

void Songs::changeVoteGenre(int s, string user, string &msgrasp)
{
    if(topGenre.empty())
    {    msgrasp+="Nu exista melodii de acest gen in top!"; msgrasp+='\n';}
    else{
        if(s>topGenre.size())
        {
            msgrasp+="Nu exista aceasta melodie in topul celor de acest gen!"; msgrasp+='\n';
        }
        else
        {
            auto it = topGenre[s - 1]->getVotes().find(user);
            if (it == topGenre[s - 1]->getVotes().end())
                msgrasp+="Nu ati votat deja aceasta melodie!\n";
            else topGenre[s-1]->changeVote(user);
        }
    }
}

void Songs::deleteGenreSong(int s, string &msgrasp)
{
    if(topGenre.empty())
    {    msgrasp+="Nu exista melodii de acest gen in top!"; msgrasp+='\n';}
    else{
        if(s>topGenre.size())
        {
            msgrasp+="Nu exista aceasta melodie in topul celor de acest gen!"; msgrasp+='\n';
        }
        else {
            for (unsigned int i = 0; i < top.size(); i++)
                if (top[i] == topGenre[s - 1]) {
                    top.erase(top.begin() + i);
                    topGenre.erase(topGenre.begin() + s - 1);
                    break;
                }
            msgrasp += "Operatiune efectuata cu succes!";
        }
    }
}