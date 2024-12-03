#ifndef PNW3D_H
#define PNW3D_H

#include <stddef.h>
#include "curl/curl.h"
#include "libs/cJSON/cJSON.h"

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


// Machine-related functions
struct Machine make_machine(const char *identifier, const char* model, const char *status, const char *filename, unsigned int progress);

void free_machine(struct Machine* machine);

// AMS API functions
char* ams_login(char* email, char* password);

struct Machine* fetch_machines(int* quantity, const char* token_value);

#define MACHINE_ACTION_STOP "stop"
#define MACHINE_ACTION_RESUME "resume"
#define MACHINE_ACTION_PAUSE "pause"

int control_machine(char* identifier, char* action, char* token);

#endif