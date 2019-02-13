#ifndef TOPMUSIC_SONGS_H
#define TOPMUSIC_SONGS_H
#include "Song.h"
#include <vector>
using namespace std;
class Songs {
    vector<Song*> topGenre;
public:
    vector<Song*> top;
    Songs();
    void addSong(Song* s);
    void sortVoturi();
    void afiseaza(string &msgrasp);
    void selectGenre(string genres);
    void showTop(string &msgrasp);
    void showGenreTop(string &msgrasp);
    void showInfo(int s, string &msgrasp);
    void showGenreInfo(int s, string &msgrasp);
    unsigned int getSize();
    void addComment(int s,string user,string comm, string &msgrasp);
    void addGenreComment(int s,string user,string comm, string &msgrasp);
    void upVote(int s, string user, string &msgrasp);
    void upVoteGenre(int s, string user, string &msgrasp);
    void downVote(int s, string user, string &msgrasp);
    void downVoteGenre(int s, string user, string &msgrasp);
    void changeVote(int s, string user, string &msgrasp);
    void changeVoteGenre(int s, string user, string &msgrasp);
    void deleteGenreSong(int s, string &msgrasp);
};


#endif //TOPMUSIC_SONGS_H
