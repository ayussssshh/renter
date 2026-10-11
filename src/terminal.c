#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <errno.h>

// this file works for displaying part

// prowide the data of the terminal window
// render the charFrameBuffer

struct winsize getTerminalSize(){

    struct winsize ws;

    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1){
        perror("ioctl");
    }

    return ws;

}


int paintTerminal(char *farmebuffer, size_t length){

    size_t offset = 0;

    while(offset < length){

        ssize_t n = write(1, farmebuffer + offset, length - offset);

        if(n>0){
            offset += (size_t)n;
        }
        else if (n < 0 && errno == EINTR) {
            continue;
        }
        else{
            return -1;
        }

    }

    return 0;

}
