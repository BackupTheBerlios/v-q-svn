#include <iostream>
#include <string>
#include <sys/types.h>
#include <grp.h>

using namespace std;

int main(int ac, char **av) {
    if( ac <= 2 ) {
        cerr<<"usage: " << *av<< " value name"<<endl;
        return 1;
    }
        
    struct group *gr;
    gr = getgrnam(av[1]);
    if( ! gr ) {
        cerr<<"can't find group: "<<av[1]<<endl;
        return 2;
    }
    cout<<"#include \"conf_"<<av[2]<<".h\""<<endl;
    cout<<"const std::string conf_"<<av[2]<<"_s = \"";
    cout<< av[1] << "\";\nconst int conf_"<<av[2]<<"_i = "
        << gr->gr_gid << ";" << endl;  
    return 0;
}
