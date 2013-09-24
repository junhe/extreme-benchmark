#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int main(int argc, char **argv)
{
    if (argc != 3) {
        printf("Usage: %s address do_access\n", argv[0]);
        exit(1);
    }
    char *addr = (char *)atol(argv[1]);
    int do_access = atoi(argv[2]);

    if ( do_access ) {
        /*printf("gonna fault\n");*/
        *addr = 0xff; /* writes a random thing */
    }
    /*printf("NO fault\n");*/

    return EXIT_SUCCESS;
}
