#include <sys/stat.h>
#include <string>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>

#ifdef DIRSTEST
#include <iostream>
#endif

#include "dirs.h"

using namespace std;

/**
create directories hierarchy
@return true on success
*/
bool mkdirhier(const char *path, mode_t m)
{
    if( mkdir(path,m) ) {
            if( errno != ENOENT ) return false;
            string pre, sub, p(path);
            string::size_type pos, beg;
            for( beg=0; (pos=p.find('/',beg)) != string::npos; beg = pos+1 ) {
                    sub = p.substr(beg, pos-beg);
                    if( sub == ".." ) return false;
                    if( sub == "." ) continue;
                    pre += sub + '/';
                    if( sub == "" ) continue;
                    if( mkdir( pre.c_str(), m ) && errno != EEXIST ) {
                            return false;
                    }
            }
            pre += p.substr(beg);
            if( mkdir(pre.c_str(), m) ) return false;
            return true;
    }
    return true;
}

/**
@exception runtime_error error occured, application possibly can't continue
@return false on error which isn't critical
*/
bool rmdirrec( const string & n ) throw(runtime_error)
{
        if( n.empty() ) return true;
        DIR *cd = opendir(".");
        if( ! cd ) return false;

        if( chdir(n.c_str()) ) {
                if( fchdir(dirfd(cd)) ) 
                        throw runtime_error((string)"rmdirrec: can't go back to: " 
                                + n + ": " + strerror(errno));
                closedir(cd);
                return false;
        }
        DIR *d = opendir(".");
        if( ! d ) {
                if( fchdir(dirfd(cd)) )
                        throw runtime_error((string)"rmdirrec: can't go back to: "
                                + n + ": " + strerror(errno) );
                closedir(cd);
                return false;
        }
        struct dirent *de;
        struct stat st;
        while( (de=readdir(d)) ) {
                if( de->d_name[0] == '.' && ( de->d_name[1] == '\0' 
                    || (de->d_name[1] == '.' && de->d_name[2] == '\0' )) )
                        continue;
                        
                if( ::unlink(de->d_name) ) {
                        if( stat(de->d_name, & st) ) {
                                closedir(d);
                                if( fchdir(dirfd(cd)) )
                                        throw runtime_error((string)"rmdirrec: can't go back to: "
                                                + n + ": " + strerror(errno) );
                                closedir(cd);
                                return false;
                        }
                        if( S_ISDIR(st.st_mode) ) {
                                if( ! rmdirrec(de->d_name) ) {
                                        closedir(d);
                                        if( fchdir(dirfd(cd)) )
                                                throw runtime_error((string)"rmdirrec: can't go back to: "
                                                        + n + ": " + strerror(errno) );
                                        closedir(cd);
                                        return false;
                                }
                        } else {
                                closedir(d);
                                if( fchdir(dirfd(cd)) )
                                        throw runtime_error((string)"rmdirrec: can't go back to: "
                                                + n + ": " + strerror(errno) );
                                closedir(cd);
                                return false;
                        }
                }
        }
        closedir(d);
        if( fchdir(dirfd(cd)) ) 
                throw runtime_error((string)"rmdirrec: can't go back to: "
                        + n + ": " + strerror(errno) );
        closedir(cd);
        return rmdir(n.c_str()) ? false : true;
}

/*!
 * \brief removes directories
 * \param ie ignore errors
 */
bool rmdirsrec( const deque<string> & dirs , bool ie ) throw(runtime_error) {
    deque<string>::size_type cnt=dirs.size();
    if(cnt--) {
            do {
                    if( ! rmdirrec(dirs[cnt]) && ! ie ) return false;
            } while(cnt--);
    }
    return true;
}
#ifdef DIRSTEST
int main()
{
    char *d[] = { "tets/xcg/fdgv/./cvb//bcv", "./sdf/srt" };
    cout << d[0] << ' ' << (int)mkdirhier(d[0],0700) << endl;
    cout << d[1] << ' ' << (int)mkdirhier(d[1],0700) << endl;
    return 0;
}
#endif
