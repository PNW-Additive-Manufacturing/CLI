#include <stdio.h>
#include "string.h"
#include "libs/cTable/src/table.h"
#include "libs/cJSON/cJSON.h"

char* formatDetails(const char* email, const char* password)
{
    char* format = "{\"email\": \"%s\", \"password\": \"%s\"}";
    
    // `{"email": "", "password": ""}`.length
    char *buffer = malloc(strlen(email) + strlen(password) + strlen(format) + 1);
    
    sprintf(buffer, format, email, password);
    
    return buffer;
}

int main()
{
    // Test cJSON
    cJSON* myJson = cJSON_Parse("{\"name\": \"Aaron Jung\"}");
    printf("cJSON Test:\n%s\n", cJSON_Print(myJson));

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

    Table* myTable = get_empty_table();

    add_cell(myTable, "Email");
    add_cell(myTable, "Password");

    next_row(myTable);

    set_hline(myTable, BORDER_DOUBLE);

    add_cell(myTable, email);
    add_cell(myTable, password);

    print_table(myTable);
    
    printf("JSON Payload: %s\n", logindetails);
    
    return 0;
}

// int main() {
//     printf("Hello World\n");

//     Table* myTable = get_empty_table();

//     set_hline(&myTable, BORDER_DOUBLE);

//     add_empty_cell(myTable);

//     add_cell(myTable, " This is the second cell! ");

//     set_vline(myTable, 1, BORDER_SINGLE);

//     add_cell(myTable, " This is the third cell! ");

//     next_row(myTable);

//     set_hline(myTable, BORDER_DOUBLE);

//     // add_empty_cell(myTable);

//     add_cell(myTable, " This is the first cell! ");

//     print_table(myTable);

//     free_table(myTable);

//     next_row(myTable);


//     return 0;
// }