#include <string>
#include <iostream>

using namespace std;

int main( int ac, char **av )
{
    ac--;
    av++;
    for( int i=1; i<ac; i+=2 ) {
        if( ! strcmp(av[0],av[i]) ) {
                cout<<av[i+1];
                return 0;
        }
    }
    cerr<<av[0]<<" does not match anything"<<endl;
    return 1;
}
