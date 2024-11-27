#include "header_func_first_pass.h"
bool check_symbol(char *word, int line_number)
{
    int i;
    /* Check if the symbol name length exceeds the maximum allowed length of 31 characters */
    if (strlen(word) > 31)
    {
        fprintf(stderr, "Error in line %d: The symbol name %s is too long.\n", line_number, word);
        return false;
    }

    /* Check if each character in the symbol name is alphanumeric */
    for (i = 0; word[i] != '\0'; i++)
    {
        if (!isalnum(word[i]))
        {
            fprintf(stderr, "Error in line %d: The symbol name %s is invalid.\n", line_number, word);
            return false;
        }
    }

    return true;
}

bool symbol_is_ok(char *word, Symbols_table *symbols_table, int symbols_table_size, Macro_table *macro_table, int macro_table_size, int line_number)
{

    int i;
    /* Check if the symbol name already exists in the symbols table */
    if (symbol_exists_in_table(symbols_table, symbols_table_size, word))
    {

        fprintf(stderr, "Error: The symbol %s in line %d has already been defined as something else.\n", word, line_number);
        return false;
    }
    /* Check if the symbol name is valid (starts with a letter and is not an opcode, register, or directive) */
    if (!isalpha(word[0]) || check_opcode(word) || check_Register(word) || check_directive(word))
    {
        fprintf(stderr, "Error in line %d: the symbol name %s invalid.\n", line_number, word);
        return false;
    }
    /* Check if the symbol name conflicts with any macro names */
    for (i = 0; i < macro_table_size; i++)
    {
        if (strcmp(macro_table[i].name, word) == 0)
        {
            fprintf(stderr, "Error in line %d: the symbol name %s is like macro name.\n", line_number, word);
            return false;
        }
    }

    return true;
}

bool symbol_exists_in_table(Symbols_table *symbols_table, int symbols_table_size, char *symbol_name)
{
    int i;
    /* Iterate through the symbols table to find the symbol */
    for (i = 0; i < symbols_table_size; i++)
    {

        if (strcmp(symbols_table[i].name, symbol_name) == 0)
        {
            return true;
        }
    }

    return false;
}

void fill_symbol_table(Symbols_table **symbols_table, int *symbols_table_size, char *name, int value, bool is_code, bool is_entry, bool is_extern)
{
/* Reallocate memory for the symbols table to accommodate the new entry */
    *symbols_table = realloc(*symbols_table, (*symbols_table_size + 1) * sizeof(Symbols_table));
    if (*symbols_table == NULL)
    {
        perror("Failed to allocate memory");
        exit(1);
    }
    /* Allocate memory and copy the symbol name */
    (*symbols_table)[*symbols_table_size].name = (char *)my_malloc(MAX_LINE_LENGTH);
    strcpy((*symbols_table)[*symbols_table_size].name, name);
    /* Initialize the new symbol's attributes */
    (*symbols_table)[*symbols_table_size].value = value;
    (*symbols_table)[*symbols_table_size].is_code = is_code;
    (*symbols_table)[*symbols_table_size].is_entry = is_entry;
    (*symbols_table)[*symbols_table_size].is_extern = is_extern;
    /* Increment the size of the symbols table */
    (*symbols_table_size)++;
}

bool check_string(char *line, int line_number, Data_table **data_table, int *data_table_size)
{
    int len = strlen(line) - 1; /*Get the length of the input line*/
    int end = -1;
    int new_size;
    int i;
    int index_extra_char = 0;
    skipSpace(&line);
    /* Check if the string starts with a quotation mark */
    if(line[0] != '"'){
        fprintf(stderr,"Line %d: String must start with a quotation mark.\n", line_number);
        return false;
    }
    /*Find the first and last quotation marks in the line*/
    for (i = 1; i < len; i++)
    {
        if (line[i] == '"')
        {
            end = i;
        
        }
        else if(line[i] != ' '){
            index_extra_char = i;
        }
    }

    /*Checks if the line ends in quotes*/
    if (end == -1 || index_extra_char > end)
    {
        fprintf(stderr,"Line %d: String must end with a quotation mark.\n", line_number);
        return false;
    }
    
    /*Calculate the new size of the array after adding the string, including the null terminator*/
    new_size = *data_table_size + end ; /*len - 2 characters in the string + 1 for the null terminator*/
    /*Reallocate memory for the array*/
    *data_table = realloc(*data_table, (*data_table_size + (len - 1)) * sizeof(Data_table));
    if (*data_table == NULL)
    {
        perror("Failed to allocate memory");
        return false;
    }
    
    /* Copy the string content (excluding the quotes) to the data table */
    for (i = 1; i < end  ; i++)
    {
        (*data_table)[*data_table_size + i - 1].value = (int)line[i];
    }

    /* Add a null terminator to the data table */
    (*data_table)[new_size - 1].value = 0;

    /* Update the size of the data table */
    *data_table_size = new_size;

    return true;
}

bool check_data(char **line, int line_number, Data_table **data_table, int *data_table_size)
{
    char *first_word, *second_word;
    int i;
    char *temp_line;
    bool return_flag = true;
    first_word = get_word(line);
    /* Process each word in the line */
    while (first_word != NULL)
    {
        temp_line = (*line);
        second_word = get_word(line);
        if ((second_word == NULL))
        {
            if (is_comma(&temp_line))
            {
                fprintf(stderr, "Error: unnecessary comma on line %d\n", line_number);
                return_flag = false;
            }
            if (extra_text(line, line_number))
            {
                return_flag = false;
            }
        }
        else
        {
            if (!check_comma(&temp_line, line_number))
            {
                return_flag = false;
            }
        }

        /* Reallocate memory for the array */
        *data_table = realloc(*data_table, (*data_table_size + 1) * sizeof(Data_table));
        if (*data_table == NULL)
        {
            perror("Failed to allocate memory");
            return_flag = false;
        }
        /* Check if the operand is valid */
        for (i = 0; first_word[i] != '\0'; i++)
        {
            if (!isdigit(first_word[i]) && first_word[i] != '-' && first_word[i] != '+')
            {
                fprintf(stderr, "Error: Invalid operand on line %d\n", line_number);
                return_flag = false;
            }
        }

        /* Add the valid operand to the data table */
        (*data_table)[*data_table_size].value = atoi(first_word);

        /* Update the size of the data table */
        (*data_table_size)++;

        first_word = second_word;
    }
    free(first_word);
    first_word = NULL;
    return return_flag;
}
int binary_opcode(Binary_table **binary_table, int *binary_table_size, char *word)
{
    int i;
    int index;
    Binary_table *table_entry;
    /* Get the index of the opcode for the given word */
    index = index_opcode(word);

    /* If the command is found, update the binary table */
    if (index != -1)
    {

        /* Allocate or reallocate memory for Binary_table */
        *binary_table = realloc(*binary_table, (*binary_table_size + 1) * sizeof(Binary_table));
        if (*binary_table == NULL)
        {
            perror("Failed to allocate memory");
            exit(1);
        }


        /* Update the new Binary_table entry */
        table_entry = &(*binary_table)[*binary_table_size];
        table_entry->address = 100 + *binary_table_size; /* Set the address to 100 */

        /* Clear the binary_code array */
        for (i = 0; i < 15; i++)
        {
            table_entry->binary_code[i] = 0;
        }

        /* Set the first 4 bits to represent the index */
        for (i = 0; i < 15; i++)
        {
            if (index & (1 << i))
            {
                table_entry->binary_code[3 - i] = 1;
            }
        }
        table_entry->binary_code[12] = 1; /*update the ARE*/

        /* Increment the size of the table */
        (*binary_table_size)++;

        return index; /* Return the index of the command */
    }

    return -1; /* Return -1 if command is not found */
}

bool second_operand(Binary_table **binary_table, int *binary_table_size, Symbol_address_table **symbol_address_table, int *symbol_address_table_size, char *word, int line_number, int *first_operand_value, int opcode)
{
    int num_addressing_method;
    Binary_table *table_entry;
    /* Check if the operand is missing and print an error message if so */
    if (word == NULL)
    {
        fprintf(stderr, "Error in line %d: missing operand \n", line_number);
        return false;
    }
    /* Get the last entry in the binary table */
    table_entry = &(*binary_table)[*binary_table_size - 1];

    /* Determine the addressing method for the operand */
    num_addressing_method = check_operand(symbol_address_table, symbol_address_table_size, word, line_number, first_operand_value);
    /* Validate the operand based on the opcode and addressing method */
    if ((opcode == MOV || (opcode >= ADD && opcode <= DEC) || opcode == RED) && num_addressing_method == 0)
    {
        fprintf(stderr, "Error in line %d: The destination operand is invalid", line_number);
        return false;
    }
    if ((opcode == JMP || opcode == BNE || opcode == JSR) && (num_addressing_method == 0 || num_addressing_method == 3))
    {
        fprintf(stderr, "Error in line %d: The destination operand is invalid", line_number);
        return false;
    }
    if (num_addressing_method == -1)
    {
        return false;
    }
    /* Update the binary code in the table entry according to the addressing method */
    table_entry->binary_code[11 - num_addressing_method] = 1;
    return true;
}

bool first_operand(Binary_table **binary_table, int *binary_table_size, Symbol_address_table **symbol_address_table, int *symbol_address_table_size, char *word, int line_number, int *first_operand_value, int opcode)
{
    int num_addressing_method;
    Binary_table *table_entry;
    /* Check if the operand is missing and print an error message if so */
    if (word == NULL)
    {
        fprintf(stderr, "Error in line %d: missing operand \n", line_number);
    }
    /* Get the last entry in the binary table */
    table_entry = &(*binary_table)[*binary_table_size - 1];

    /* Determine the addressing method for the operand */
    num_addressing_method = check_operand(symbol_address_table, symbol_address_table_size, word, line_number, first_operand_value);
    /* Validate the operand based on the opcode and addressing method */
    if (opcode == LEA && num_addressing_method != 1)
    {
        fprintf(stderr, "Error in line %d: The source operand is invalid", line_number);
        return false;
    }
    if (num_addressing_method == -1)
    {
        return false;
    }
    /* Update the binary code in the table entry according to the addressing method */
    table_entry->binary_code[7 - num_addressing_method] = 1;
    return true;
}

int check_operand(Symbol_address_table **symbol_address_table, int *symbol_address_table_size, char *word, int line_number, int *first_operand_value)
{
    int i;
    /* Handle immediate values (e.g., #123) */
    if (word[0] == '#')
    {
        /* Check if the rest of the string is a number*/
        for (i = 1; word[i] != '\0'; i++)
        {
            if (!isdigit(word[i]) && word[i] != '-' && word[i] != '+')
            {
                fprintf(stderr, "Error: Invalid operand on line %d\n", line_number);
                return -1;
            }
        }
        /* Convert the string to an integer and store it in number*/
        *first_operand_value = atoi(word + 1);
        if ((*first_operand_value) > 2047 || (*first_operand_value) < -2048)
        {
            fprintf(stderr, "Error: The operand on line %d out of range\n", line_number);
            return -1;
        }
        return 0;
    }
    /* Handle indirect addressing with registers (e.g., *r1) */
    if (word[0] == '*')
    {
        if (check_Register(word + 1))
        {
            word = (word + 2);
            /* find the register number */
            for (i = 0; i < 8; i++)
            {
                if ((atoi(word) == i))
                {
                    *first_operand_value = i;
                    return 2;
                }
            }
        }
        return -1;
    }
    /* Handle direct addressing with registers (e.g., r1) */
    if (check_Register(word))
    {
        word = (word + 1);
        /* find the register number */
        for (i = 0; i < 8; i++)
        {
            if ((atoi(word) == i))
            {
                *first_operand_value = i;

                return 3;
            }
        }
        return -1;
    }
    /* Handle symbols */
    if (check_symbol(word, line_number))
    {
        /* Allocate or reallocate memory for symbol_address_table */
        *symbol_address_table = realloc(*symbol_address_table, (*symbol_address_table_size + 1) * sizeof(Symbol_address_table));
        if (*symbol_address_table == NULL)
        {
            perror("Failed to allocate memory");
            exit(1);
        }        
        (*symbol_address_table)[*symbol_address_table_size].name = (char *)my_malloc(MAX_LINE_LENGTH);
        (*symbol_address_table)[*symbol_address_table_size].name = word;
        (*symbol_address_table_size)++;

        return 1;
    }
    return -1;
}

void fill_binary_table(int *IC, Binary_table **binary_table, int *binary_table_size, int first_operand_value, int second_operand_value, Symbol_address_table **symbol_address_table, int symbol_address_table_size)
{
    Binary_table *table_entry;
    Binary_table *table_previous;
    int i;
    /* Get the previous entry in the binary table */
    table_previous = &(*binary_table)[*binary_table_size - 1];
    /* if both the first operand is a register and the second one, then one line is made for both */
    if ((table_previous->binary_code[4] || table_previous->binary_code[5]) && (table_previous->binary_code[8] || table_previous->binary_code[9]))
    {

        /* Allocate or reallocate memory for Binary_table */
        *binary_table = realloc(*binary_table, (*binary_table_size + 1) * sizeof(Binary_table));
        if (*binary_table == NULL)
        {
            perror("Failed to allocate memory");
            exit(1);
        }

        /* Update the new Binary_table entry */
        table_entry = &(*binary_table)[*binary_table_size];
        table_entry->address = 100 + *binary_table_size; /* Set the address to 100 + */

        /* Clear the binary_code array */
        for (i = 0; i < 15; i++)
        {
            table_entry->binary_code[i] = 0;
        }

        /* Set the 3-5 bits to represent the first_operand_value */
        for (i = 0; i < 15; i++)
        {
            if (first_operand_value & (1 << i))
            {
                table_entry->binary_code[8 - i] = 1;
            }
        }

        /* Set the6-8 bits to represent the second_operand_value */
        for (i = 0; i < 15; i++)
        {
            if (second_operand_value & (1 << i))
            {
                table_entry->binary_code[11 - i] = 1;
            }
        }
        table_entry->binary_code[12] = 1; /*update the ARE*/

        /* Increment the size of the table */
        (*binary_table_size)++;
        (*IC)++;
        return;
    }
    /* Handle cases where both operands are a label */
    if (table_previous->binary_code[6] && table_previous->binary_code[10])
    {
        (*symbol_address_table)[symbol_address_table_size - 2].address = (100 + *binary_table_size);
    }
    /* Handling cases where both operands are immediate numbers*/
    if (table_previous->binary_code[7])
    {
        help_fill_binary_table(binary_table, binary_table_size, symbol_address_table, symbol_address_table_size, first_operand_value, false, 11);
        (*IC)++;
    }
    /* Handling cases where operands are labeled*/
    else if (table_previous->binary_code[6])
    {
        help_fill_binary_table(binary_table, binary_table_size, symbol_address_table, symbol_address_table_size, first_operand_value, true, 0);
        (*IC)++;
    }
    /* Handling cases where the operand is a register*/
    else if (table_previous->binary_code[4] == 1 || table_previous->binary_code[5] == 1)
    {
        help_fill_binary_table(binary_table, binary_table_size, symbol_address_table, symbol_address_table_size, first_operand_value, false, 8);
        (*IC)++;
    }
    /* Handling cases where both operands are immediate numbers*/
    if (table_previous->binary_code[11])
    {
        help_fill_binary_table(binary_table, binary_table_size, symbol_address_table, symbol_address_table_size, second_operand_value, false, 11);
        (*IC)++;
    }
    /* Handling cases where operands are labeled*/
    else if (table_previous->binary_code[10])
    {
        help_fill_binary_table(binary_table, binary_table_size, symbol_address_table, symbol_address_table_size, second_operand_value, true, 0);

        (*IC)++;
    }
    /* Handling cases where the operand is a register*/
    else if (table_previous->binary_code[8] || table_previous->binary_code[9])
    {
        help_fill_binary_table(binary_table, binary_table_size, symbol_address_table, symbol_address_table_size, second_operand_value, false, 11);
        (*IC)++;
    }
}

void help_fill_binary_table(Binary_table **binary_table, int *binary_table_size, Symbol_address_table **symbol_address_table, int symbol_address_table_size, int operand_value, bool is_symbol, int index)
{
    Binary_table *table_entry;
    int i;

    /* Allocate or reallocate memory for Binary_table */
    *binary_table = realloc(*binary_table, (*binary_table_size + 1) * sizeof(Binary_table));
    if (*binary_table == NULL)
    {
        perror("Failed to allocate memory");
        exit(1);
    }

    /* Update the new Binary_table entry */
    table_entry = &(*binary_table)[*binary_table_size];
    table_entry->address = 100 + *binary_table_size; /* Set the address to 100 + */

    /* Clear the binary_code array */
    for (i = 0; i < 15; i++)
    {
        table_entry->binary_code[i] = 0;
    }
    (*binary_table_size)++;
    /* If the operand is a symbol, update the address in symbol_address_table and return */
    if (is_symbol)
    {
        (*symbol_address_table)[symbol_address_table_size - 1].address = (100 + *binary_table_size - 1);
        return;
    }

    /* Set the binary_code bits based on the operand_value */
    for (i = 0; i < 12; i++)
    {
        if (operand_value & (1 << i))
        {
            table_entry->binary_code[index - i] = 1;
        }
    }
    table_entry->binary_code[12] = 1; /*update the ARE*/
}