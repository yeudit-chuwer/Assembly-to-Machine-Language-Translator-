#ifndef HEADER_SECOND_PASS_H
#define HEADER_SECOND_PASS_H

#include "header_first_pass.h"
#include "header_help_func.h"

/*The function completes the assembly process by terminating the binary code and creating output files (.ob, .ext, .ent) based on the data and if no errors are found in the process*/
bool second_pass(char *filename_am, bool *error_flag, Symbols_table *symbols_table, int *symbols_table_size, Binary_table *binary_table, int *binary_table_size, Macro_table *macro_table, int macro_table_size, Symbol_address_table **symbol_address_table, int *symbol_address_table_size, Data_table *data_table, int *data_table_size, int IC, int DC);

/* Function to free memory allocated for the entry table */
void free_entry_table(Entry_table *table, int count);

/* Function to free memory allocated for the external table */
void free_extern_table(Extern_table *table, int count);

#endif