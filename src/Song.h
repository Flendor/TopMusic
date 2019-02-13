#include <string>
#include <map>
#include <vector>
using namespace std;

#ifndef TOPMUSIC_SONG_H
#define TOPMUSIC_SONG_H

class Song
{
    string name;
    string description;
    string artist;
    vector<string> genres;
    string link;
    string proposedBy;
    int votes;
    map<string,int> myVote;
    multimap<string,string> comments;
public:
    Song();
    Song(string nume, string descriere, string singer, vector<string> genuri, string url, int voturi, multimap<string,string> comentarii, map<string,int> votulMeu, string user);
    void upVote(string user);
    void downVote(string user);
    void addComment(string user,string comm);
    int howManyVotes();
    string getName();
    string getArtist();
    vector<string> &getGenres();
    void showInfo(string &msgrasp);
    string getDescription();
    string getUrl();
    multimap<string,string> &getComments();
    map<string,int> &getVotes();
    void changeVote(string user);
    string getUser();
    ~Song();
};

#endif //TOPMUSIC_SONG_H
