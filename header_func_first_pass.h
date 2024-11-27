#ifndef HEADER_FUNC_FIRST_PASS_H
#define HEADER_FUNC_FIRST_PASS_H

#include "header_macro.h"
#include "header_help_func.h"
#include "header_first_pass.h"


bool symbol_exists_in_table(Symbols_table *symbols_table, int symbols_table_size, char *symbol_name);
bool check_symbol(char *word, int line_number);
void fill_symbol_table(Symbols_table **symbols_table, int *symbols_table_size, char *name, int value, bool is_code, bool is_entry,bool is_extern);
int binary_opcode(Binary_table **binary_table,int * binary_table_size, char *word);
bool first_operand(Binary_table **binary_table, int *binary_table_size, Symbol_address_table **symbol_address_table, int *symbol_address_table_size, char *word, int line_number,int *first_operand_value,int opcode);
bool second_operand(Binary_table **binary_table, int *binary_table_size, Symbol_address_table **symbol_address_table, int *symbol_address_table_size, char *word, int line_number, int *first_operand_value, int opcode);
int check_operand(Symbol_address_table **symbol_address_table, int *symbol_address_table_size, char *word, int line_number, int *first_operand_value);
void fill_binary_table(int *IC, Binary_table **binary_table, int *binary_table_size, int first_operand_value, int second_operand_value, Symbol_address_table **symbol_address_table, int symbol_address_table_size);
void help_fill_binary_table(Binary_table **binary_table, int *binary_table_size,Symbol_address_table **symbol_address_table, int symbol_address_table_size, int operand_value, bool is_symbol, int index);
bool symbol_is_ok(char *word, Symbols_table *symbols_table, int symbols_table_size, Macro_table *macro_table, int macro_table_size, int line_number);
bool check_string(char *line, int line_number, Data_table **data_table, int *data_table_size);
bool check_data(char **line, int line_number, Data_table **data_table ,int * data_table_size);

#endif