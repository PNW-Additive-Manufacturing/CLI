#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "libs/cJSON/cJSON.h"

#define PNW3DUrl "https://pnw3d.com"

struct CurlUserData {
    char* content;
    size_t size;
};

size_t curl_user_write_callback(char* buffer, size_t size, size_t nitems, void* userdata) {
    size_t total_bytes = size * nitems;
    struct CurlUserData* current_data = (struct CurlUserData*)userdata;

    char* new_content = realloc(current_data->content, current_data->size + total_bytes + 1);
    if (new_content == NULL) {
        fprintf(stderr, "Failed to reallocate memory during CURL Write!\n");
        return 0; 
    }

    current_data->content = new_content;
    memcpy(new_content + current_data->size, buffer, total_bytes);
    current_data->size = current_data->size + total_bytes;
    current_data->content[current_data->size] = '\0'; // Null-terminate

    return total_bytes;
}

cJSON* parse_user_data_as_json(struct CurlUserData* userData)
{
    cJSON* resDataJSON = cJSON_ParseWithLength(userData->content, userData->size);    
    // if (resDataJSON == NULL) {
    //     fprintf(stderr, "Failed to parse CURL JSON data: %s\n", userData->content);
    //     exit(1);
    // }
    return resDataJSON;
}

char* curl_format_cookie(const char* name, const char* value) {
    char* cookie = malloc((strlen(name) + strlen(value) + 2) * sizeof(char)); // +2 for '=' and '\0'
    if (cookie == NULL) 
    {
        fprintf(stderr, "Failed to allocate memory for CURL Cookie!\n");
        return NULL;
    }
    sprintf(cookie, "%s=%s", name, value);
    return cookie;
}

/**
 * Represents a limited version of a 3D Printer / machines state from the AMS API.  
 */
struct Machine {
    char* name;
    char* status;
    char* filename;
    char* model;
    unsigned int progress;
};

struct Machine make_machine(const char *identifier, const char* model, const char *status, const char *filename, unsigned int progress) {
    struct Machine machine;

    // We want Machine to own its references.
    machine.name = strdup(identifier);
    machine.status = strdup(status);
    machine.filename = filename ? strdup(filename) : NULL;
    machine.progress = progress;
    machine.model = strdup(model);

    return machine;
}

/**
 * Parses a Machine using a cJSON object.
 */
struct Machine cJSON_Parse_Machine(cJSON* node) {
    return make_machine(
        cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(node, "identifier")),
        cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(node, "model")),
        cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(node, "status")),
        cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(node, "filename")),
        (unsigned int)cJSON_GetNumberValue(cJSON_GetObjectItemCaseSensitive(node, "progress"))
    );
}

void free_machine(struct Machine* machine) {
    if (machine)
    {
        free(machine->name);
        free(machine->model);
        free(machine->status);
        free(machine->filename);
    }
}

// No-op callback to discard response body
static size_t noop_write_callback(void* ptr, size_t size, size_t nmemb, void* userdata) {
    return size * nmemb;
}

#define MACHINE_ACTION_STOP "stop"
#define MACHINE_ACTION_RESUME "resume"
#define MACHINE_ACTION_PAUSE "pause"

// I will add the success check later and make more functions.... later...... not today.... It is very late.
int control_machine(char* identifier, char* action, const char* token, char* fileToUse)
{
    char* url;

    if (strcmp(action, "start") == 0)
    {
        if (fileToUse == NULL) return NULL;

        char* format = "https://pnw3d.com/api/farm/control?identifier=%s&action=%s&fileToPrint=%s";
        url = malloc((strlen(format) + strlen(identifier) + strlen(action) + strlen(fileToUse) + 1) * sizeof(char));
        sprintf(url, format, identifier, action, fileToUse);
    }
    else 
    {
        char* format = "https://pnw3d.com/api/farm/control?identifier=%s&action=%s";
        url = malloc((strlen(format) + strlen(identifier) + strlen(action) + 1) * sizeof(char));
        sprintf(url, format, identifier, action);
    }


    CURL* curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Failed to initialize CURL!\n");
        free(url);
        return NULL;
    }

    // char* encoded_curl = curl_easy_escape(curl, url, 0);

    char* cookies = curl_format_cookie("session-token", token);
    if (cookies == NULL) {
        curl_easy_cleanup(curl);
        free(url);
        return NULL;
    }

    printf("URL: %s\n", url);
    
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_COOKIELIST, "ALL");
    curl_easy_setopt(curl, CURLOPT_COOKIE, cookies);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    // curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, noop_write_callback);

    CURLcode res = curl_easy_perform(curl);

    curl_easy_cleanup(curl);

    return res == CURLE_OK;
}

/**
 * Counts the number of children a cJSON object contains.
 */
int cJSON_Count_Children(cJSON* node)
{
    int count = 0;
    cJSON *current_element = NULL;
    // This is a tricky for-loop macro to reduce the amount of boilerplate. 
    cJSON_ArrayForEach(current_element, node) {
        count++;
    }
    return count;
}

char* ams_login(char* email, char* password)
{
    // We could use cJSON to construct JSON correctly for sure but this will work for the moment.
    char* format = "{\"email\": \"%s\", \"password\": \"%s\"}";
    // `{"email": "", "password": ""}`.length
    char *buffer = malloc(strlen(email) + strlen(password) + strlen(format) + 1);    
    sprintf(buffer, format, email, password);

    CURL* curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Failed to initialize CURL!\n");
        free(buffer);
        return NULL;
    }

    struct CurlUserData resData = { .content = malloc(1), .size = 0 };
    if (resData.content == NULL) {
        fprintf(stderr, "Failed to allocate initial memory for response data!\n");
        return NULL;
    }

    curl_easy_setopt(curl, CURLOPT_URL, "https://pnw3d.com/api/login");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buffer);
    // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);    
    curl_easy_setopt(curl, CURLOPT_COOKIELIST, "ALL");
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resData);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_user_write_callback);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    cJSON* resDataJSON = parse_user_data_as_json(&resData);
    if (resDataJSON == NULL)
    {
        fprintf(stderr, "Failed to parse response JSON: %s\n", resData.content);
        return NULL;
    }
    free(resData.content);

    // printf("Data: %s\n", cJSON_Print(resDataJSON));
    if (cJSON_IsTrue(cJSON_GetObjectItemCaseSensitive(resDataJSON, "success")) == 0)
    {
        fprintf(stderr, "API response from AMS was marked as unsuccessful!\n");
        return NULL;
    }

    free(buffer);
    return cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(resDataJSON, "token"));
    cJSON_free(resDataJSON);
    
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
struct Machine* fetch_machines(int* quantity, const char* token_value) {

    *quantity = 0;

    struct CurlUserData resData = { .content = malloc(1), .size = 0 };
    if (resData.content == NULL) {
        fprintf(stderr, "Failed to allocate initial memory for response data!\n");
        return NULL;
    }

    CURL* curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Failed to initialize CURL!\n");
        free(resData.content);
        return NULL;
    }

    char* cookies = curl_format_cookie("session-token", token_value);
    if (cookies == NULL) {
        curl_easy_cleanup(curl);
        free(resData.content);
        return NULL;
    }

    curl_easy_setopt(curl, CURLOPT_URL, "https://pnw3d.com/api/farm/printers");
    curl_easy_setopt(curl, CURLOPT_COOKIELIST, "ALL");
    curl_easy_setopt(curl, CURLOPT_COOKIE, cookies);
    // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);    
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resData);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_user_write_callback);

    free(cookies);
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        curl_easy_cleanup(curl);
        return NULL;
    }
    curl_easy_cleanup(curl);


    cJSON* resDataJSON = parse_user_data_as_json(&resData);
    free(resData.content);

    // printf("Data: %s\n", cJSON_Print(resDataJSON));
    if (cJSON_IsTrue(cJSON_GetObjectItemCaseSensitive(resDataJSON, "success")) == 0)
    {
        fprintf(stderr, "API response from AMS was marked as unsuccessful!\n");
        return NULL;
    }

    // The response includes a "success" entry so we don't count that.
    *quantity = cJSON_Count_Children(resDataJSON) - 1;
    // printf("Allocating %d Machine(s)\n", *quantity);
    struct Machine* machines = malloc(*quantity * sizeof(struct Machine));
    int insertedMachines = 0;

    cJSON *current_element = NULL;
    // This is a tricky C macro hiding a for-loop statement to reduce boilerplate.
    cJSON_ArrayForEach(current_element, resDataJSON) {
        if (strcmp(current_element->string, "success") == 0) continue;

        // printf("Parsing JSON %s\n", current_element->string);
        machines[insertedMachines] = cJSON_Parse_Machine(current_element);

        insertedMachines += 1;
    }

    cJSON_free(resDataJSON);

    return machines; // Update this to return actual parsed data if needed
}