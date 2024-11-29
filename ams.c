#include "curl/curl.h"
#include "libs/cJSON/cJSON.h"

#define PNW3DUrl "https://pnw3d.com"

struct Machine
{
    char* name;
    char* status;
    char* filename;
};

struct Machine make_machine(const char* name, const char* status, const char* filename)
{
    struct Machine machine;
    machine.name = name;
    machine.status = status;
    machine.filename = filename;

    return machine;
}

/**
 * Fetches information about 3D printers from the [Additive Manufacturing Service](https://pnw3d.com) FarmAPI endpoint.
 * 
 * If the request is successful, the `quantity` parameter will be updated to reflect the number of printers available, and a pointer to an array of `Machine` structures will be returned.
 *
 * If the request fails, an error message will be printed, and `NULL` will be returned. The `quantity` parameter will be set to `0` in case of failure.
 * 
 * @param quantity A pointer to an integer that will be updated with the number of printers fetched.
 * @param token_value The session-token value required to access the FarmAPI of AMS, Maintainer+ permission required.
 * 
 * @return A pointer to an array of `Machine` structures containing the printer information, or `NULL` if the request fails.
 */
struct Machine* fetch_machines(int* quantity, char* token_value)
{
    CURL* curl = curl_easy_init();
    if (!curl)
    {
        quantity = 0;
        return NULL;
    }

    curl_easy_setopt(curl, CURLOPT_URL, strcat(PNW3DUrl, "/api/farm/printers"));
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl, CURLOPT_COOKIELIST, "ALL");
    curl_easy_setopt(curl, CURLOPT_COOKIE, strcat("session-token=", token_value));

    CURLcode res = curl_easy_perform(curl);
    
    if (res != CURLE_OK){
        printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }
    curl_easy_cleanup(curl);

    // TODO: Write a callback function to actually process the data.
    // curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, )

    quantity = 0;
    return NULL;
}