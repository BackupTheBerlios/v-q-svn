#include <iostream>
#include <string>
#include <sys/types.h>
#include <pwd.h>

using namespace std;

int main(int ac, char **av) {
    string name; 
    if(! getline(cin, name)) {
        cerr<<"no name on stdin"<<endl;
        return 1;
    }
        
    struct passwd *pw;
    pw = getpwnam(name.c_str());
    if( ! pw ) {
        cerr<<"can't find user: "<<name<<endl;
        return 2;
    }
    cout<<pw->pw_uid;
    return 3;
}
