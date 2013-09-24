#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int n = 0;
char *p = 0; /* I know it is not necesary to assign here, just for easier understanding */

void
signal_callback_handler(int signum)
{
    /*printf("Caught signal %d\n",signum);*/
    // Cleanup and close up stuff here
    /*printf("I am the real handler\n");*/
    if ( n ) {
        p += 4096;
        n--;
        return;
    } else {
        exit(signum);
    }

}

void dummy_handler(int signum)
{
    /*printf("I am dummy\n");*/
    if ( n ) {
        p += 4096;
        n--;
        return;
    } else {
        exit(signum);
    }

}

int main(int argc, char **argv)
{
    if ( argc != 3 ) {
        printf("Usage: %s do_seg_fault[0|1] times\n", argv[0]);
        exit(1);
    }
    
    int do_seg = atoi(argv[1]);
    n = atoi(argv[2]);

    signal(SIGSEGV, signal_callback_handler);

    while(n)
    {
        /*printf("Program processing stuff here.\n");*/
        if ( do_seg ) {
            *p = 0;
        } else {
            dummy_handler(SIGSEGV);
        }
    }
    return EXIT_SUCCESS;
}


