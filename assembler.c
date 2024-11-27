#include <stdio.h>
#include "header_macro.h"
#include "header_first_pass.h"



int main(int argc, char *argv[])
{
    /*initialize variables IC DC*/
    int IC ;
    int DC ;
    int i = 1;/*A variable passed to argv*/
    bool check_macro; /* Argument to check if the pre-process is good to know if to continue */
    bool check_first_pass; /* Argument to check if the first pass is good to know if to continue */
    char *file_as, *file_am;
    /*Initialize tables and counters*/
    Macro_table *macro_table = NULL;
    int macro_table_size;
    
    Data_table *data_table = NULL;
    int data_table_size;

    Symbols_table *symbols_table = NULL;
    int symbols_table_size;

    Binary_table *binary_table = NULL;
    int binary_table_size;

    Symbol_address_table *symbol_address_table = NULL;
    int symbol_address_table_size;


    /* Check if the program received any arguments */
    if (argc == 1)
    {
        fprintf(stderr, "You need to enter arguments\n");
        exit(0);
    }

    /* Iterate through each file provided as an argument and process it */
    while (argv[i] != NULL) {
        /* Reset counters and tables for each file */
        IC = 0;
        DC = 0;
        macro_table = NULL;
        macro_table_size = 0;
        symbols_table = NULL;
        binary_table = NULL;
        symbols_table_size = 0;
        binary_table_size = 0;
        symbol_address_table = NULL;
        symbol_address_table_size = 0;
        data_table = NULL;
        data_table_size = 0;

        /* Append file extensions to the current file name */
        file_as = add_end(argv[i], ".as");
        file_am = add_end(argv[i], ".am");
        printf("FILE %s:\n",file_as);
        /* check the macro in the file*/
        check_macro = read_file(&macro_table, &macro_table_size, file_as, file_am);
        /* If the check macro succeeded, proceed to the first pass */
        if (check_macro) {
            check_first_pass = first_pass(&IC,&DC,file_am, symbols_table, &symbols_table_size, binary_table,&binary_table_size, macro_table, macro_table_size, &symbol_address_table, &symbol_address_table_size, data_table, &data_table_size  );
            
        }
        /* If the first pass and second pass succeeded, print a success message */
        if (check_first_pass && check_macro) {
            printf("No errors found\n");
        }
        
        
        free(file_am);
        free(file_as);
        
        i++;/* Increment to process the next file */
    }

    return 0;
}



