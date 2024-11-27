#include "header_first_pass.h"
#include "header_func_first_pass.h"
#include "header_second_pass.h"

bool first_pass(int *IC, int *DC, char *filename_am, Symbols_table *symbols_table, int *symbols_table_size, Binary_table *binary_table, int *binary_table_size, Macro_table *macro_table, int macro_table_size, Symbol_address_table **symbol_address_table, int *symbol_address_table_size, Data_table *data_table, int *data_table_size)
{
    FILE *file_am;
    char line[MAX_LINE_LENGTH];
    char *first_word;
    char *second_word;
    bool symbols_flag = false;
    int line_number = 0;
    bool error_flag = false;
    int op_code;
    bool check_second_pass;
    /* Open the .am file for reading */
    file_am = fopen(filename_am, "r");
    if (!file_am)
    {
        fprintf(stderr, "Cannot open file %s\n", filename_am);
        exit(1);
    }
    /* Process each line of the file */
    while (fgets(line, MAX_LINE_LENGTH, file_am) != NULL)
    {
        char *line_ptr = line;
        line_number++;/*add a number to the line to know which line you are in the file*/

        /* Check for an unnecessary comma at the beginning of the line */
        if (is_comma(&line_ptr))
        {
            fprintf(stderr, "Error: unnecessary comma on line %d\n", line_number);
            error_flag = true;
        }
         /* Get the first word from the line */
        first_word = get_word(&line_ptr);
    
        if (first_word != NULL)
        {
            /* Check for an unnecessary comma after the first word */
            if (is_comma(&line_ptr))
            {
                fprintf(stderr, "Error: unnecessary comma on line %d\n", line_number);
                error_flag = true;
            }

            /* Check if the first word is a label (ends with ':') */
            if (first_word[strlen(first_word) - 1] == ':')
            {
                /*Remove the colons and take only the name of the label*/
                first_word[strlen(first_word) - 1] = '\0';

                /* check if symbol valid */
                if (symbol_is_ok(first_word, symbols_table, *symbols_table_size, macro_table, macro_table_size, line_number) && check_symbol(first_word, line_number))
                {
                    symbols_flag = true;
                }
                /* if the symbol invalid - continue to the next word */
                else
                {
                    first_word = get_word(&line_ptr);
                    error_flag = true;
                }
            }
            /* Process the line based on whether it starts with a symbol */
            if (symbols_flag)
            {
                /* Get the next word after the label */
                second_word = get_word(&line_ptr);
                symbols_flag = false;
                if (second_word == NULL)
                {
                    fprintf(stderr, "Error in line %d: missing value\n", line_number);
                    error_flag = true;
                    continue;
                }
                /* Handle the different types of directives */
                if (strcmp(second_word, ".data") == 0 || strcmp(second_word, ".string") == 0)
                {
                    fill_symbol_table(&symbols_table, symbols_table_size, first_word, (*DC), false, false, false);
                    first_word = second_word;
                }
                else if (strcmp(second_word, ".extern") == 0 || strcmp(second_word, ".entry") == 0)
                {
                    first_word = second_word;
                    
                }
                else
                {
                    fill_symbol_table(&symbols_table, symbols_table_size, first_word, ((*IC) + 100), true, false, false);
                    first_word = second_word;
                    
                }
            }
            /* Check for an unnecessary comma after processing the first word */
            if (is_comma(&line_ptr))
            {
                fprintf(stderr, "Error: unnecessary comma on line %d\n", line_number);
                error_flag = true;
            }
            /* Process data and string directives */
            if (strcmp(first_word, ".data") == 0 || strcmp(first_word, ".string") == 0)
            {
                bool good_string = true;
                bool good_data = true;

                if (strcmp(first_word, ".string") == 0)
                {
                    /* Validate the string directive */
                    good_string = check_string(line_ptr, line_number, &data_table, data_table_size);
                }
                else
                {
                    /* Validate the data directive */
                    good_data = check_data(&line_ptr, line_number, &data_table, data_table_size);
                }
                if (!good_string || !good_data)
                {
                    error_flag = true;
                }
                else
                {
                    /*Update the DC to be the size of the data and the string table*/
                    (*DC) = *data_table_size;
                }
                continue;
            }
            /* Handle the .extern and .entry directives */
            else if (strcmp(first_word, ".extern") == 0 || strcmp(first_word, ".entry") == 0)
            {

                if (strcmp(first_word, ".extern") == 0)
                {
                    char *temp_line;
                    first_word = get_word(&line_ptr);
                    while (first_word != NULL)
                    {
                        temp_line = (line_ptr);
                        second_word = get_word(&line_ptr);
                        if ((second_word == NULL))
                        {
                            /* Check for an unnecessary comma or extra text after the extern directive */
                            if (is_comma(&temp_line))
                            {
                                fprintf(stderr, "Error: unnecessary comma on line %d\n", line_number);
                                error_flag = true;
                            }
                            if (extra_text(&line_ptr, line_number))
                            {
                                error_flag = true;
                            }
                        }
                        else
                        {
                            /* Check for a comma between operands */
                            if (!check_comma(&temp_line, line_number))
                            {
                                error_flag = true;
                            }
                        }

                        if (!symbol_exists_in_table(symbols_table, *symbols_table_size, first_word))
                        {
                            fill_symbol_table(&symbols_table, symbols_table_size, first_word, -1, false, false, true);
                        }
                        else
                        {
                            error_flag = true;
                            fprintf(stderr, "Error: The symbol %s in line %d has already been defined as something else.\n", first_word, line_number);
                        }
                        first_word = second_word;
                        
                    }
                }
            }
            else
            {
                if (!check_opcode(first_word))
                {
                    error_flag = true;
                    fprintf(stderr, "Error on line %d: Invalid opcode: %s\n", line_number, first_word);
                }
                else
                {
                    int first_operand_value = 0;
                    int second_operand_value = 0;
                    bool first_operand_good;
                    bool second_operand_good;
                    op_code = binary_opcode(&binary_table, binary_table_size, first_word);
                    (*IC)++;
                    if (op_code >= 0 && op_code <= 4)
                    {
                        first_word = get_word(&line_ptr);
                        first_operand_good = first_operand(&binary_table, binary_table_size, symbol_address_table, symbol_address_table_size, first_word, line_number, &first_operand_value, op_code);
                        if (!check_comma(&line_ptr, line_number))
                        {
                            error_flag = true;
                        }
                    }
                    if (op_code >= 0 && op_code <= 13)
                    {
                        second_word = get_word(&line_ptr);
                        second_operand_good = second_operand(&binary_table, binary_table_size, symbol_address_table, symbol_address_table_size, second_word, line_number, &second_operand_value, op_code);
                    }
                    /*?????????????????????????*/
                    if (extra_text(&line_ptr, line_number))
                    {
                        error_flag = true;
                    }
                    if (!second_operand_good || !first_operand_good)
                    {
                        error_flag = true;
                        continue;
                    }

                    fill_binary_table(IC, &binary_table, binary_table_size, first_operand_value, second_operand_value, symbol_address_table, *symbol_address_table_size);
                }
            }
        }
        else
        {
            fprintf(stderr,"First word is NULL, skipping line %d\n", line_number);
        }
    }

    fclose(file_am);
    free(first_word);
    first_word = NULL;

    if (!error_flag)
    {
        int i;
        for (i = 0; i < *symbols_table_size; i++)
        {
            if (symbols_table[i].is_code == false && symbols_table[i].is_entry == false)
            {
                symbols_table[i].value = symbols_table[i].value + (*IC) + 100;
            }
        }
    }

    check_second_pass = second_pass(filename_am, &error_flag, symbols_table, symbols_table_size, binary_table, binary_table_size, macro_table, macro_table_size, symbol_address_table, symbol_address_table_size, data_table, data_table_size, *IC, *DC);

    free_macro_table(macro_table,macro_table_size);

    free_symbols_table(symbols_table,*symbols_table_size);
                   

    free_symbol_address_table(*symbol_address_table,*symbol_address_table_size);


    free(data_table);
    data_table = NULL;
    

    return check_second_pass;
}


void free_macro_table(Macro_table *table, int count) {
    int i;
    for (i = 0; i < count; i++) {
        free(table[i].name);
        free(table[i].value);
        table[i].name = NULL;
        table[i].value = NULL;

    }
    free(table);
    table = NULL;

}

void free_symbols_table(Symbols_table *table, int count) {
    int i;
    

    for (i = 0; i < count; i++) {
        free(table[i].name);
        table[i].name = NULL;

    }
    free(table);
    table = NULL;
}

void free_symbol_address_table(Symbol_address_table *table, int count) {
    int i;
    for (i = 0; i < count; i++) {
       
        free(table[i].name);
        table[i].name = NULL;
    }
    free(table);
    table = NULL;
}



