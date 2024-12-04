#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include "libs/cTable/src/table.h"
#include "ams.h"
#include "access.h"

#define UpdateInterval 3 // Amount of time to wait when refreshing machines in seconds.

char* getCLIArgument(const char* argumentName, int argc, char *argv[]);
char* requireCLIArgument(const char* argumentName, int argc, char *argv[]);

#include <stdio.h>
#include <stdlib.h>

// https://stackoverflow.com/questions/111928/is-there-a-printf-converter-to-print-in-binary-format?page=2&tab=scoredesc#tab-top
char* printb(unsigned int v, unsigned int n) {
    if (n > sizeof(v) * 8) {
        // Clamp
        n = sizeof(v) * 8; 
    }

    char* result = (char*)malloc(n + 1);
    if (!result) {
        return NULL; 
    }

    result[n] = '\0'; // Null-terminate the string

    for (unsigned int i = 0; i < n; ++i) {
        result[i] = (v & 1) ? '1' : '0'; // Check if the LSB is 1 or 0
        v >>= 1; // Right shift v to move to the next bit
    }

    // Reverse the string to show the right-most bits in correct order.
    for (unsigned int i = 0; i < n / 2; ++i) {
        char temp = result[i];
        result[i] = result[n - i - 1];
        result[n - i - 1] = temp;
    }

    return result;
}

void display_machines(const char* accessToken)
{
    int machineQuantity = 0;
    struct Machine* machines = NULL;

    // We need to store the previous result in case something goes wrong fetching the new interval.
    struct Machine* previousMachines = NULL;
    int previousMachineQuantity = 0;

    int isLastFetchSuccessful = 0;

    while (1) {
        machines = fetch_machines(&machineQuantity, accessToken);

        isLastFetchSuccessful = machineQuantity != 0;

        // Use previous results if fetching machines fails
        if (machineQuantity == 0) {
            // fprintf(stderr, "\x1B[38;5;1mError: Unable to contact the server. Displaying previous results.\033[m\n");
            machines = previousMachines;
            machineQuantity = previousMachineQuantity;
        } else {
            // Free previous results and update them
            if (previousMachines != NULL) {
                free(previousMachines);
            }
            previousMachines = machines;
            previousMachineQuantity = machineQuantity;
        }

        // Create the table
        Table* myTable = get_empty_table();

        set_hline(myTable, BORDER_SINGLE);

        add_cell(myTable, " Machine Name ");
        add_cell(myTable, " Status ");
        add_cell(myTable, " Filename ");
        add_cell(myTable, " Progress ");

        override_above_border(myTable, BORDER_NONE);
        add_cell(myTable, "\x1B[38;5;243m Features \033[m");

        next_row(myTable);

        // Style the ctable
        set_hline(myTable, BORDER_SINGLE);
        set_vline(myTable, 0, BORDER_SINGLE);
        set_vline(myTable, 1, BORDER_SINGLE);
        set_vline(myTable, 2, BORDER_SINGLE);
        set_vline(myTable, 4, BORDER_SINGLE);

        // Populate table with machine data
        for (int i = 0; i < machineQuantity; i++) {
            add_cell_fmt(myTable, " %s (%s) ", machines[i].name, machines[i].model);

            int isBusy = strcmp(machines[i].status, "Printing") == 0 || strcmp(machines[i].status, "Preparing") == 0;
            int isDoneOrUnfinished = strcmp(machines[i].status, "Printed") == 0 || strcmp(machines[i].status, "Error") == 0;

            // Use colors for status
            if (strcmp(machines[i].status, "Printed") == 0) {
                add_cell_fmt(myTable, "\x1B[38;5;2m %s \033[m", machines[i].status);
            } 
            else if (isBusy) {
                add_cell_fmt(myTable, "\x1B[38;5;4m %s \033[m", machines[i].status);
            }
            else if (strcmp(machines[i].status, "Error") == 0) {
                add_cell_fmt(myTable, "\x1B[38;5;1m %s \033[m", "Cancelled");
            } 
            else 
            {
                add_cell_fmt(myTable, " %s ", machines[i].status);
            }

            if (machines[i].filename != NULL && (isBusy || isDoneOrUnfinished || strcmp(machines[i].status, "Paused") == 0)) {
                add_cell_fmt(myTable, " %s ", machines[i].filename);

                override_horizontal_alignment(myTable, H_ALIGN_RIGHT);
                add_cell_fmt(myTable, " %d%% ", machines[i].progress);
            } else {
                add_empty_cell(myTable);
                add_empty_cell(myTable);
            }

            char* featuresInBinary = printb((unsigned short)machines[i].features, 4);
            char* featuresFormat = "\x1B[38;5;243m %s \033[m";
            char* wrappedFeaturesInBinary = malloc((strlen(featuresInBinary) + strlen(featuresFormat) + 1) * sizeof(char));
            sprintf(wrappedFeaturesInBinary, featuresFormat, featuresInBinary);
            free(featuresInBinary);

            override_above_border(myTable, BORDER_NONE);
            add_cell_gc(myTable, wrappedFeaturesInBinary);

            // if ((machines[i].features & FEATURE_CONTROLTABBLE) != 0)
            // {
            //     add_cell(myTable, "\x1B[38;5;243m Controllable \033[m");
            // }

            set_hline(myTable, BORDER_SINGLE);
            
            next_row(myTable);
        }
        set_hline(myTable, BORDER_SINGLE);

        add_empty_cell(myTable);
        add_empty_cell(myTable);
        add_empty_cell(myTable);
        add_empty_cell(myTable);

        override_above_border(myTable, BORDER_NONE);
        add_empty_cell(myTable);

        // This clears the terminal using ANSI characters.
        printf("\033[H\033[J");
        print_table(myTable);

        if (!isLastFetchSuccessful)
        {
            fprintf(stderr, "\x1B[38;5;1mUnable to connect to AMS. Retrying...\033[m\n");
        }
        else
        {
            printf("Connected to AMS! Listening for changes...\n");
        }

        // Free the table
        free_table(myTable);

        sleep(UpdateInterval);
    }

    // Free resources before exiting
    if (previousMachines != NULL) {
        free(previousMachines);
    }
    free(machines);
}

int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        if (strcmp(argv[1], "login") == 0)
        {
            login();
        }
        if (strcmp(argv[1], "logout") == 0)
        {
            logout();
        }
        else if ((strcmp(argv[1], "pause")==0) || (strcmp(argv[1], "resume")==0) || (strcmp(argv[1], "stop")==0))
        {
            printf("\n");

            char* machine = requireCLIArgument("machine", argc, argv);

            if (argc < 2)
            {
                goto usage;
            }

            // Read the stored access token.
            char* accessToken = readAccessToken();

            printf("\x1B[38;5;243mContacting PNW3D API...\033[m\n");

            int control_result = control_machine(machine, argv[1], NULL, accessToken);

            if (control_result)
            {
                printf("\x1B[38;5;2m✓ Command has been sent to %s!\033[m\n\n", machine, argv[2]);
            }
            else
            {
                printf("\x1B[38;5;1mAn issue occurred commanding %s!\033[m\n\n", machine);
            }
            free(accessToken);
        }
        else if (strcmp(argv[1], "start") == 0)
        {
            printf("\n");

            char* fileToUse = requireCLIArgument("file", argc, argv);
            char* machine = requireCLIArgument("machine", argc, argv);

            char* accessToken = readAccessToken();

            printf("\x1B[38;5;243mContacting PNW3D API...\033[m\n");

            int control_result = control_machine(machine, argv[1], fileToUse, accessToken);

            if (control_result)
            {
                printf("\x1B[38;5;2m✓ Printing %s on %s!\033[m\n\n", fileToUse, machine);
            }
            else
            {
                printf("\x1B[38;5;1mAn issue occurred commanding %s!\033[m\n\n", machine);
            }
            free(accessToken);
        }
        else
        {
usage: 
            printf("Usage:\tcli.exe\t[starts the monitor]\n");
            printf("OR\tcli.exe login/logout\n");
            printf("OR\tcli.exe start --machine NAME --file PATH\n");
            printf("OR\tcli.exe stop/resume/pause --machine NAME\n");
            exit(1);
        }
    }
    else
    {
        // Read the stored access token.
        char* accessToken = readAccessToken();
        display_machines(accessToken);
    }

    return 0;
}

/**
 * Gets an arguments value from the CLI formatted as `-argumentName value` where value is returned.
 * A NULL-pointer is returned if the argument does not exist or a value is not provided.
 */
char* getCLIArgument(const char* argumentName, int argc, char *argv[])
{
    char* dashArgumentName = malloc(strlen(argumentName) + 3);
    sprintf(dashArgumentName, "--%s", argumentName);

    for (int i = 0; i < argc; i++)
    {
        if (strcmp(dashArgumentName, argv[i]) == 0)
        {
            // We found the option!
            if (i + 1 >= argc)
            {
                // Option does not contain a value.
                fprintf(stdout, "Argument: %s must contain a value!", argumentName);
                exit(1);
            }
            return argv[i + 1];
        }
    }

    free(dashArgumentName);
    return NULL;
}

char* requireCLIArgument(const char* argumentName, int argc, char *argv[])
{
    char* result = getCLIArgument(argumentName, argc, argv);

    if (result == NULL)
    {
        printf("Argument: --%s is required!", argumentName);
        exit(1);
    }
    return result;
}