#include <iostream>
#include <string>
#include <sys/types.h>
#include <grp.h>

using namespace std;

int main(int ac, char **av) {
    string name;
    if(!getline(cin, name)) {
        cerr<<"no name on stdin"<<endl;
        return 1;
    }

    struct group *gr;
    gr = getgrnam(name.c_str());
    if( ! gr ) {
        cerr<<"can't find group: "<<name<<endl;
        return 2;
    }
    cout<<gr->gr_gid;
    return 3;
}
