#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "libs/cTable/src/table.h"
#include "ams.c"

int login();
int upload(char* stlPath, struct Machine* machines, int machineCount);

char* getCLIArgument(const char* argumentName, int argc, char *argv[]);
char* requireCLIArgument(const char* argumentName, int argc, char *argv[]);

char* formatDetails(const char* email, const char* password);

int main(int argc, char *argv[])
{
    // Place-holder values. Will need to use malloc when fetching.
    struct Machine machines[3];
    machines[0] = make_machine("Sam (A1)", "Printing", "Spotify Car Thing Mount");
    machines[1] = make_machine("Cinder Block (X1C)", "Printing", "IEEE Tags");
    machines[2] = make_machine("Ripley (X1E)", "Idle", NULL);

    if (argc > 1)
    {
        if (strcmp(argv[1], "login") == 0)
        {
            login();
        }
        else if (strcmp(argv[1], "upload") == 0)
        {
            char* stlPath = requireCLIArgument("file", argc, argv);

            upload(stlPath, machines, 3);
        }
        else
        {
            printf("Usage:\tci.exe\nOR\tci.exe login\nOR\tci.exe upload --file PATH\n");
            exit(1);
        }
    }
    else
    {
        // Printer Monitor
        printf("Logged in as Aaron Jung\n\n");

        // Place holder values
        Table* myTable = get_empty_table();

        set_hline(myTable, BORDER_SINGLE);

        add_cell(myTable, " Name ");
        add_cell(myTable, " Status ");
        add_cell(myTable, " Filename ");

        next_row(myTable);

        set_hline(myTable, BORDER_SINGLE);

        set_vline(myTable, 0, BORDER_SINGLE);
        set_vline(myTable, 1, BORDER_SINGLE);
        set_vline(myTable, 2, BORDER_SINGLE);

        // Values
        for (int i = 0; i < 3; i++)
        {
            add_cell_fmt(myTable, " %s ", machines[i].name);
            add_cell_fmt(myTable, " %s ", machines[i].status);

            if (machines[i].filename != NULL)
            {
                add_cell_fmt(myTable, " %s ", machines[i].filename);
            }
            else add_empty_cell(myTable);

            set_hline(myTable, BORDER_SINGLE);
            next_row(myTable);
        }

        make_boxed(myTable, BORDER_SINGLE);

        print_table(myTable);
    }

    return 0;
}

int login()
{
    char email[100];
    char password[100];
    
    printf("Enter your email associated with pnw3d: ");
    fgets(email, sizeof(email), stdin);
    
    // TODO: Fix this, we used another method to remove the newline charater.
    email[strlen(email) - 1] = "\0";
    
    printf("Enter your password associated with pnw3d: ");
    fgets(password, sizeof(password), stdin);
    
    // TODO: Fix this, we used another method to remove the newline charater.
    password[strlen(password) - 1] = "\0";
    
    char* logindetails = formatDetails(email, password);
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

char* formatDetails(const char* email, const char* password)
{
    char* format = "{\"email\": \"%s\", \"password\": \"%s\"}";
    
    // `{"email": "", "password": ""}`.length
    char *buffer = malloc(strlen(email) + strlen(password) + strlen(format) + 1);
    
    sprintf(buffer, format, email, password);
    
    return buffer;
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
