#include <iostream>
#include <string>
#include <sys/types.h>
#include <pwd.h>

using namespace std;

int main(int ac, char **av) {
	if( ac != 2 ) {
		cerr<<"usage: " << *av<< " name"<<endl;
		return 1;
	}
		
	struct passwd *pw;
	pw = getpwnam(av[1]);
	if( ! pw ) {
		cerr<<"can't find user: "<<av[1]<<endl;
		return 2;
	}
	cout<< pw->pw_uid << endl;  
	return 0;
}
