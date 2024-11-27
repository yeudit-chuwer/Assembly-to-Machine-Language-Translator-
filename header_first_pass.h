#ifndef HEADER_FIRST_PASS_H
#define HEADER_FIRST_PASS_H

#include "header_macro.h"
#include "header_help_func.h"



#define MOV 0
#define ADD 2
#define LEA 4
#define DEC 8
#define JMP 9
#define BNE 10
#define RED 11
#define JSR 13

typedef struct Symbols_table 
{
    char *name;
    int value;
    bool is_code; /* true if it is code, false if it is .entry or .extern or .string or .data */
    bool is_entry; /* true if it is entry, false if it is extern */
    bool is_extern;
} Symbols_table;

typedef struct Binary_table
{
    int address;
    int binary_code[15];
} Binary_table;

typedef struct Symbol_address_table {
    char *name;
    int address;
} Symbol_address_table;

typedef struct Data_table{
    int value;
}Data_table;

typedef struct Entry_table{
    char *name;
    int address;
}Entry_table;

typedef struct Extern_table
{
    char *name;
    int address;
}Extern_table;



bool first_pass(int *IC, int *DC,char *filename_am, Symbols_table *symbols_table, int *symbols_table_size, Binary_table *binary_table, int *binary_table_size, Macro_table *macro_table, int macro_table_size, Symbol_address_table **symbol_address_table, int *symbol_address_table_size, Data_table *data_table, int *data_table_size);



void free_macro_table(Macro_table *table, int count);
void free_symbols_table(Symbols_table *table, int count);
void free_symbol_address_table(Symbol_address_table *table, int count);
#endif
