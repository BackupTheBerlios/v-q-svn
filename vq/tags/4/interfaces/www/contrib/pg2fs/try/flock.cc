#include <fcntl.h>
#include <sys/types.h>
#include <sys/file.h>

int main() {
    flock(0, LOCK_NB | LOCK_EX | LOCK_UN);
    return 0;
}
