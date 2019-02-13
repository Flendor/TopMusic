#ifndef TOPMUSIC_THISUSER_H
#define TOPMUSIC_THISUSER_H
#include <string>
#include <map>
using namespace std;

class thisUser {
    string name;
    bool admin;
    bool restricted;
public:
    bool logIn;
    thisUser();
    string getName();
    bool isAdmin();
    void setProp(char user[], bool adm, bool res);
    bool loggedIn();
    bool isRestricted();
    void setRestrict(bool res);
    void setAdmin(bool adm);
    ~thisUser();
};


#endif //TOPMUSIC_THISUSER_H
