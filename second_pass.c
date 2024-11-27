#include "header_second_pass.h"
#include "header_func_first_pass.h"
bool second_pass(char *filename_am, bool *error_flag, Symbols_table *symbols_table, int *symbols_table_size, Binary_table *binary_table, int *binary_table_size, Macro_table *macro_table, int macro_table_size, Symbol_address_table **symbol_address_table, int *symbol_address_table_size, Data_table *data_table, int *data_table_size, int IC, int DC)
{

    FILE *file_am;
    char line[MAX_LINE_LENGTH];
    char *first_word;
    int line_number = 0;
    Entry_table *entry_table = NULL;
    int entry_table_size = 0;
    Extern_table *extern_table = NULL;
    int extern_tabel_size = 0;
    int i, j;
    bool return_flag = false;
    /* open the file_am */
    file_am = fopen(filename_am, "r");
    if (!file_am)
    {
        fprintf(stderr, "Cannot open file %s\n", filename_am);
        exit(1);
    }

    for (i = 0; i < *symbol_address_table_size; i++)
    {
        char *word;
        word = (*symbol_address_table)[i].name;
        for (j = 0; j < *symbols_table_size; j++)
        {
            if ((strcmp(word, symbols_table[j].name) == 0) && (symbols_table[j].is_extern == true))
            {
                extern_table = realloc(extern_table, (extern_tabel_size + 1) * sizeof(Extern_table));
                (extern_table)[extern_tabel_size].name = (char *)my_malloc(MAX_LINE_LENGTH);
                strcpy((extern_table)[extern_tabel_size].name, word);
                (extern_table)[extern_tabel_size].address = (*symbol_address_table)[i].address;
                extern_tabel_size++;
            }
        }
    }

    while (fgets(line, MAX_LINE_LENGTH, file_am) != NULL)
    {
        int index_op;
        int count_operand = 0;
        char *line_ptr = line;
        line_number++;
        first_word = get_word(&line_ptr);

        if (first_word != NULL)
        {
            if (first_word[strlen(first_word) - 1] == ':')
            {
                first_word = get_word(&line_ptr);
                if (first_word == NULL)
                {
                    continue;
                }
            }
            if (strcmp(first_word, ".data") == 0 || strcmp(first_word, ".string") == 0 || strcmp(first_word, ".extern") == 0)
            {
                continue;
            }
            if (strcmp(first_word, ".entry") == 0)
            {

                entry_table = realloc(entry_table, (entry_table_size + 1) * sizeof(Entry_table));
                if (entry_table == NULL)
                {
                    perror("Failed to allocate memory");
                    exit(1);
                }
                first_word = get_word(&line_ptr);

                for (i = 0; i < *symbols_table_size; i++)
                {
                    if (strcmp(first_word, symbols_table[i].name) == 0)
                    {

                        symbols_table[i].is_entry = true;
                        (entry_table)[entry_table_size].name = (char *)my_malloc(MAX_LINE_LENGTH);
                        strcpy((entry_table)[entry_table_size].name, first_word);
                        (entry_table)[entry_table_size].address = (symbols_table)[i].value;
                        entry_table_size++;

                        break;
                    }
                }
                if (extra_text(&line_ptr, line_number))
                {
                    *error_flag = true;
                }
            }
            else
            {
                index_op = index_opcode(first_word);
                if (index_op >= 0 && index_op <= 4)
                {
                    count_operand = 2;
                }
                else if (index_op > 4 && index_op <= 13)
                {
                    count_operand = 1;
                }

                while ((first_word = get_word(&line_ptr)) != NULL && count_operand != 0)
                {
                    int index_to_symbols_table = -1;
                    count_operand--;
                    if (first_word[0] == 'r' || first_word[0] == '*' || first_word[0] == '#')
                    {
                        continue;
                    }
                    for (i = 0; i < *symbols_table_size; i++)
                    {
                        if (strcmp(first_word, symbols_table[i].name) == 0)
                        {
                            index_to_symbols_table = i;
                        }
                    }
                    if (index_to_symbols_table == -1)
                    {
                        fprintf(stderr, "Error in line %d: The symbol name %s don't found.\n", line_number, first_word);
                        *error_flag = true;
                        continue;
                    }
        
                    for (i = 0; i < *symbol_address_table_size; i++)
                    {

                        if (strcmp(first_word, (*symbol_address_table)[i].name) == 0)
                        {
                            for (j = 0; j < strlen((*symbol_address_table)[i].name); j++)
                            {
                                ((*symbol_address_table)[i].name)[j] = '!';
                            }
                            if (symbols_table[index_to_symbols_table].is_extern)
                            {
                                binary_table[((*symbol_address_table)[i].address) - 100].binary_code[14] = 1;
                                break;
                            }
                            else
                            {
                                for (j = 0; j < 12; j++)
                                {
                                    if (symbols_table[index_to_symbols_table].value & (1 << j))
                                    {
                                        binary_table[((*symbol_address_table)[i].address) - 100].binary_code[11 - j] = 1;
                                    }
                                }

                                binary_table[((*symbol_address_table)[i].address) - 100].binary_code[13] = 1;
                                continue;
                            }
                        }
                    }
                }
            }
        }
    }
        free(first_word);
        first_word = NULL;

    /* Iterate over the data table to update the binary table */
    for (i = 0; i < *data_table_size; i++)
    {
        /* Reallocate memory for binary_table to accommodate new entries */
        binary_table = realloc(binary_table, (*binary_table_size + 1) * sizeof(Binary_table));
        if (binary_table == NULL)
        {
            perror("Failed to allocate memory");
            exit(1);
        }
        /* Initialize binary_code array to 0 */
        for (j = 0; j < 15; j++)
        {
            binary_table[*binary_table_size].binary_code[j] = 0;
        }
        /* Convert data_table value to binary representation */
        for (j = 0; j < 15; j++)
        {
            if (data_table[i].value & (1 << j))
            {
                binary_table[*binary_table_size].binary_code[14 - j] = 1;
            }
        }
        /* Set address for the new entry */
        binary_table[*binary_table_size].address = 100 + (*binary_table_size);
        (*binary_table_size)++;
    }

    /*for (i = 0; i < *binary_table_size; i++)
    {
        int j;
        printf("Address: %d\n", binary_table[i].address);
        printf("Binary code: ");
        for (j = 0; j < 15; j++)
        {
            printf("%d ", binary_table[i].binary_code[j]);
        }
        printf("\n");
    }*/
    /* Check if the instruction count (IC) and data count (DC) exceeds memory limit */
    if (IC + DC + 100 > 4095)
    {
        fprintf(stderr, "Error in line %d: Too many lines - browsing from memory \n", line_number);
        *error_flag = true;
    }
    /* If no errors were found, proceed to write output files */
    if (*error_flag == false)
    {
        FILE *file_ob;
        char *filename_ob;
        /* Create and open the object file with ".ob" extension */
        filename_ob = add_end(filename_am, ".ob");
        file_ob = fopen(filename_ob, "w");
        if (!file_ob)
        {
            printf("Error: Could not create output file %s\n", filename_ob);
            free(filename_ob);
            filename_ob = NULL;
            exit(1);
        }
        /* Write IC and DC values to the object file */
        fprintf(file_ob, "%5d %d\n", IC, DC);
        for (i = 0; i < (*binary_table_size); i++)
        {
            int j, num;
            /* write address and convert binary code to octal */
            fprintf(file_ob, "%04d  ", binary_table[i].address);
            for (j = 0; j < 15; j = j + 3)
            {
                num = (binary_table[i].binary_code[j] * 4) + (binary_table[i].binary_code[j + 1] * 2) + (binary_table[i].binary_code[j + 2]);
                fprintf(file_ob, "%d", num);
            }
            fputs("\n", file_ob);
        }
        fclose(file_ob);
        free(filename_ob);
        filename_ob = NULL;
        /* If external symbols exist, create and write to the external symbols file with ".ext" extension */
        if (extern_tabel_size != 0)
        {
            FILE *file_ext;
            char *filename_ext;
            filename_ext = add_end(filename_am, ".ext");
            file_ext = fopen(filename_ext, "w");
            if (!file_ext)
            {
                printf("Error: Could not create output file %s\n", filename_ext);
                free(filename_ext);
                filename_ext = NULL;
                exit(1);
            }
            /* Write external symbols and their addresses */
            for (i = 0; i < extern_tabel_size; i++)
            {
                fprintf(file_ext, "%-3s ", extern_table[i].name);
                fprintf(file_ext, "%04d", extern_table[i].address);
                fputs("\n", file_ext);
            }
            fclose(file_ext);
            free(filename_ext);
            filename_ext = NULL;
        }
        /* If entry symbols exist, create and write to the entry symbols file with ".ent" extension */
        if (entry_table_size != 0)
        {
            FILE *file_ent;
            char *filename_ent;
            filename_ent = add_end(filename_am, ".ent");
            file_ent = fopen(filename_ent, "w");
            if (!file_ent)
            {
                printf("Error: Could not create output file %s\n", filename_ent);
                free(filename_ent);
                filename_ent = NULL;
                exit(1);
            }
            for (i = 0; i < entry_table_size; i++)
            {
                /* Write entry symbols and their addresses */
                fprintf(file_ent, "%-4s ", entry_table[i].name);
                fprintf(file_ent, "%d", entry_table[i].address);
                fputs("\n", file_ent);
            }
            fclose(file_ent);
            free(filename_ent);
            filename_ent = NULL;
        }
        return_flag = true;
    }
    /* Free dynamically allocated memory for entry and external tables */
    free_entry_table(entry_table, entry_table_size);
    free_extern_table(extern_table, extern_tabel_size);

    return return_flag;
}

void free_entry_table(Entry_table *table, int count)
{
    int i;
    for (i = 0; i < count; i++)
    {
        free(table[i].name);
        table[i].name = NULL;
    }
    free(table);
    table = NULL;
}

void free_extern_table(Extern_table *table, int count)
{
    int i;
    for (i = 0; i < count; i++)
    {
        free(table[i].name);
        table[i].name = NULL;
    }
    free(table);
    table = NULL;
}