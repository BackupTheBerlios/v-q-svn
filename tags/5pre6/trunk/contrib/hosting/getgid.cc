#include <iostream>
#include <string>
#include <sys/types.h>
#include <grp.h>

using namespace std;

int main(int ac, char **av) {
	if( ac != 2 ) {
		cerr<<"usage: " << *av<< " name"<<endl;
		return 1;
	}
		
	struct group *gr;
	gr = getgrnam(av[1]);
	if( ! gr ) {
		cerr<<"can't find group: "<<av[1]<<endl;
		return 2;
	}
	cout << gr->gr_gid << endl;  
	return 0;
}
