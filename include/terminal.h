#if !defined(TERMINAL_H)
#define TERMINAL_H

#include <stdio.h>


struct winsize getTerminalSize();
int paintTerminal(char *farmebuffer, size_t length);


#endif // TERMINAL_H

