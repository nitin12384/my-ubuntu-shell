#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    char* val = "A" ;
    if (argc == 2) {
        val = argv[1] ;
    }
    while (1) {
        sleep(1);
        printf("%s\n", val);
    }
    return 0;
}
