#include "thisUser.h"
#include <string>
using namespace std;

thisUser::thisUser()
{
    logIn=false;
}

void thisUser::setProp(char user[], bool adm, bool res)
{
    name=user;
    admin=adm;
    logIn=true;
    restricted=res;
}

string thisUser::getName()
{
    return name;
}

bool thisUser::isAdmin()
{
    return admin;
}

bool thisUser::loggedIn()
{
    return logIn;
}

bool thisUser::isRestricted()
{
    return restricted;
}

void thisUser::setRestrict(bool res)
{
    restricted=res;
}

void thisUser::setAdmin(bool adm)
{
    admin=adm;
}

thisUser::~thisUser()
{

}