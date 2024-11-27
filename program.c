#include <stdio.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>

char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}


int main()
{
    CURL *curl;
    CURLcode res;
    char email[100];
    char password[100];
    
    printf("Enter your email associated with pnw3d: ");
    fgets(email, sizeof(email), stdin);
    
    printf("Enter your password associated with pnw3d: ");
    fgets(password, sizeof(password), stdin);
    
    char* logindetails = concat(email, password);
    
    curl_global_init(CURL_GLOBAL_ALL);
    
    curl = curl_easy_init();
    
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, "https://pnw3d.com/api/user/login");
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "{\"email\" : \"password\"}"); //INSTERT DATA INTO EMAIL/PASSWORD FIELD
        
        res = curl_easy_perform(curl);
        
        curl_easy_setopt(curl, CURLOPT_COOKIELIST, "ALL");
        
        if (res != CURLE_OK){
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            
        }
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

    return 0;
}