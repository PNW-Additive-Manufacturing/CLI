#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include "libs/cTable/src/table.h"
#include "ams.h"
#include "access.h"

#define UpdateInterval 5 // Amount of time to wait when refreshing machines in seconds.

char* getCLIArgument(const char* argumentName, int argc, char *argv[]);
char* requireCLIArgument(const char* argumentName, int argc, char *argv[]);

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
            fprintf(stderr, "\x1B[38;5;1mError: Unable to contact the server. Displaying previous results.\033[m\n");
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

            set_hline(myTable, BORDER_SINGLE);
            
            next_row(myTable);
        }
        set_hline(myTable, BORDER_SINGLE);

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
            char* machine = requireCLIArgument("machine", argc, argv);

            if (argc < 2)
            {
                goto usage;
            }

            // Read the stored access token.
            char* accessToken = readAccessToken();

            int control_result = control_machine(machine, argv[1], accessToken, NULL);

            if (control_result)
            {
                printf("\n\x1B[38;5;2m✓ Command has been sent to %s!\033[m\n\n", machine, argv[2]);
            }
            else
            {
                printf("\n\x1B[38;5;1mAn issue occurred commanding %s!\033[m\n\n", machine);
            }
            free(accessToken);
        }
        else if (strcmp(argv[1], "start") == 0)
        {
            char* fileToUse = requireCLIArgument("file", argc, argv);
            char* machine = requireCLIArgument("machine", argc, argv);
            char* accessToken = readAccessToken();

            int control_result = control_machine(machine, argv[1], accessToken, fileToUse);

            if (control_result)
            {
                printf("\n\x1B[38;5;2m✓ Printing %s on %s!\033[m\n\n", fileToUse, machine);
            }
            else
            {
                printf("\n\x1B[38;5;1mAn issue occurred commanding %s!\033[m\n\n", machine);
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


int upload(char* stlPath, struct Machine* machines, int machineCount)
{
    FILE* file = fopen(stlPath, "r");
    if (file == NULL)
    {
        printf("\nUnable to upload, %s does not exist!\n", stlPath);
    }

    printf("Uploading %s\n", stlPath);

    Table* myTable = get_empty_table();
    
    add_cell(myTable, " Select idle machine to use: ");

    next_row(myTable);

    add_cell(myTable, "");

    next_row(myTable);

    for (int i = 0; i < machineCount; i++)
    {
        add_cell_fmt(myTable, " %s ", (machines + i)->name);
        next_row(myTable);
    }

    make_boxed(myTable, BORDER_SINGLE);

    print_table(myTable);

    // TODO Read and upload to FarmAPI
    printf("Enter machine name: ", stlPath);
    char* machineName = scanf("%s");

    printf("Sending part to %s\n", machineName);

    fclose(file);
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