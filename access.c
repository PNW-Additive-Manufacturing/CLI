#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <termios.h>
#include "ams.h"

// https://www.gnu.org/savannah-checkouts/gnu/libc/manual/html_node/getpass.htmls
ssize_t my_getpass (char** lineptr, size_t *n, FILE *stream)
{
  struct termios old, new;
  int nread;

  /* Turn echoing off and fail if we can’t. */
  if (tcgetattr (fileno (stream), &old) != 0)
    return -1;
  new = old;
  new.c_lflag &= ~ECHO;
  if (tcsetattr (fileno (stream), TCSAFLUSH, &new) != 0)
    return -1;

  /* Read the passphrase */
  nread = getline (lineptr, n, stream);

  /* Restore terminal. */
  (void) tcsetattr (fileno (stream), TCSAFLUSH, &old);

  return nread;
}

char* readAccessToken()
{
    const char *name = "PNW3dAccessToken";
    const char *env_p = getenv(name);
    char* accessToken;
    FILE* accessTokenFile = fopen(".accessToken", "r");

    if(env_p)
    {
        accessToken = env_p;
    }
    else if (accessTokenFile == NULL)
    {
        fprintf(stderr, "You are not logged in!\n");
        exit(1);
    }
    else
    {
        fseek(accessTokenFile, 0, SEEK_END);
        long fileSize = ftell(accessTokenFile); // Cursor position
        fseek(accessTokenFile, 0, SEEK_SET); 

        accessToken = (char *)malloc((fileSize + 1) * sizeof(char)); 

        size_t bytesRead = fread(accessToken, sizeof(char), fileSize, accessTokenFile);
        accessToken[bytesRead] = '\0';

        fclose(accessTokenFile);
    }

    return accessToken;
}


void storeAccessToken(char* accessToken)
{
    FILE* accessTokenFile = fopen(".accessToken", "w+");
    if (accessTokenFile == NULL)
    {
        fprintf(stderr, "Failed to interact with the .accessToken file. Is anything else handling it?\n");
        exit(1);
    }
    
    fwrite(accessToken, sizeof(char), strlen(accessToken), accessTokenFile);

    fclose(accessTokenFile);
}


void login()
{
    while (1)
    {
        char email[100];
        // char password[100];
        char* password = NULL;
        
        printf("\nEnter your PNW3D Email: ");
        fgets(email, sizeof(email), stdin);
        
        email[strcspn(email, "\r\n")] = 0;
        
        printf("Enter your PNW3D Password: ");
        size_t len = 0;
        my_getpass(&password, &len, stdin);
        // fgets(password, sizeof(password), stdin);

        // New line after password-input:
        printf("\n");

        password[strcspn(password, "\r\n")] = 0;
        
        // The user has given us their email and password, send it to the AMS to get a session/access token so we can pull machine data!
        char* token = ams_login(email, password);
        if (token == NULL)
        {
            fprintf(stderr, "\nPassword or email is incorrect! Try again...\n");
            free(password);
        }
        else
        {
            free(password);

            // We got a session-token. Store it an close the program!
            storeAccessToken(token);

            printf("\nYou have been successfully logged in!\n");
            exit(1);
        }
    }
}


void logout()
{
    remove(".accessToken");
    printf("\nYou have been successfully logged out!\n");
}