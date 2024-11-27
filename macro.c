#include "header_macro.h"
#include "header_help_func.h"

bool read_file(Macro_table **table, int *table_size, char *filename_source, char *filename_am)
 {
    FILE *file_source, *file_am;
    char line[MAX_LINE_LENGTH];
    char *word;
    bool macro_flag = false;
    Macro_table *current_macro = NULL;
    int line_number = 0;

    /* flag that indicates if there are errors */
    bool error_flag = false;

    file_source = fopen(filename_source, "r");
    if (!file_source) {
        fprintf(stderr, "Cannot open file %s\n", filename_source);
        exit(1);
    }

    /* open the output file */
    file_am = fopen(filename_am, "w");
    if (!file_am) {
        fprintf(stderr, "Cannot open file %s\n", filename_am);
        fclose(file_source);
        exit(1);
    }

    /* read each line from the source file */
    while (fgets(line, MAX_LINE_LENGTH, file_source) != NULL) {
        char *line_ptr = line;
        line_number++; 
        
        /* Read each line from the source file */
        if (strlen(line) >= MAX_LINE_LENGTH -1 ) 
        {
            fprintf(stderr, "Error: Line %d exceeds maximum length of %d characters.\n", line_number, MAX_LINE_LENGTH - 1);
            error_flag = true;
        }
        
        

        word = get_word(&line_ptr);

        if (word != NULL) {
            Macro_table *macro = find_macro(*table, *table_size, word);

            /* Skip comment lines */
            if(word[0] == ';'){
                continue;
            }
            if (macro != NULL) {
                /* If the first word matches a macro name, write the macro's value to the output file */
                fputs(macro->value, file_am);
            } else if (strcmp(word, "macr") == 0) {
                /* Start of a new macro definition */
                macro_flag = true;
               
                word = get_word(&line_ptr); /* Get the macro name */
                if (word != NULL) {
                    /* Check if the name of macro is ok - not name of opcode/ Register/directive */
                    if(check_opcode(word) || check_Register(word) || check_directive(word))
                    {
                        error_flag = true;
                    }

                    if(strcmp(word,"macr")==0){
                        error_flag = true;
                    }

                    /*if have errors- print error mesege*/
                    if (error_flag) {
                        fprintf(stderr, "Error in name macro in line %d\n", line_number);
                    }

                    /* Allocate memory for macro table */
                    *table = realloc(*table, (*table_size + 1) * sizeof(Macro_table));
                   
                    /* Allocate memory for macro name */
                    (*table)[*table_size].name = (char *)my_malloc(MAX_LINE_LENGTH);
                    strcpy((*table)[*table_size].name, word);

                    /* Initialize value */
                    (*table)[*table_size].value = calloc(1, sizeof(char));
                    current_macro = &(*table)[*table_size];
                    (*table_size)++;

                    /* Check if there is extra char after the macro name */
                    if (extra_text(&line_ptr,line_number)) {
                        error_flag = true;
                    }
                }

             
            } 

            /* End of the current macro definition */
            else if (macro_flag && strcmp(word, "endmacr") == 0) {

                /* End of the current macro definition */
                macro_flag = false;
                current_macro = NULL;

                /* Check if there is extra char after the macro end */
                if (extra_text(&line_ptr,line_number)) {
                    error_flag = true;
                }
            
            } 

            /* If inside a macro definition, append the line to the macro's value */
            else if (macro_flag && current_macro != NULL) {

                /* Append the line to the current macro's value */
                current_macro->value = realloc(current_macro->value, strlen(current_macro->value) + strlen(line) + 1);
                strcat(current_macro->value, line);
            } 

            /* If not inside a macro, write the original line to the output file */
            else {
                /* Write the original line to the output file */
                fputs(line, file_am);
            }

            free(word);
            word = NULL;
        } 

    }
    /* close the files */
    fclose(file_source);
    fclose(file_am);

    /* If have errors return to main and continue to the next file and remove the file_am*/
    if (error_flag) {

        remove(filename_am);
        return false;
    }

    return true;
}

Macro_table* find_macro(Macro_table *table, int size, char *name) {
    int i;
    for (i = 0; i < size; i++) {
        if (strcmp(table[i].name, name) == 0) {
            return &table[i];
        }
    }
    return NULL;
}