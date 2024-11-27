#include "header_help_func.h"

void *my_malloc(int size)
{
    void *ptr = malloc(size);
    if (!ptr)
    {
        fprintf(stderr, "Allocation failed\n");
        exit(1);
    }
    return ptr;
}

void skipSpace(char **line) {
    while (**line == ' ' || **line == ',') {
        (*line)++;
    }
}

/* Skips leading spaces and commas in the line */
/*void skipSpace(char **line)
{
    while (**line == ' ' && **line != '\0' && **line != '\n')
    {
        (*line)++;
    }
}*/

/* Returns the next word from the line */
char *get_word(char **line)
{
    int i = 0;
    char *word;

    word = (char *)calloc(MAX_LINE_LENGTH, sizeof(char));
    if (word)
    {
        skipSpace(line); /* Skip leading spaces and commas*/

        /*  Read the word*/
        while (**line != '\0' && !isspace(**line) && **line != ',' && i < MAX_LINE_LENGTH)
        {
            word[i] = **line;
            i++;
            (*line)++;
        }

        /* Null-terminate the word*/
        word[i] = '\0';

        /* If we didn't read any characters, return NULL*/
        if (i == 0)
        {
            free(word);
            return NULL;
        }
    }
    return word;
}

/*A function that checks if a comma is present*/
bool is_comma(char **line)
{
    while (**line == ' ' && **line != '\0' && **line != '\n')
    {
        (*line)++;
    }
    if (**line == ',')
    {
        (*line)++;
        return true;
    }
    return false;
}

bool check_comma(char **line, int line_number)
{
    if(!is_comma(line)){
        fprintf(stderr,"Error: Missing comma on line %d\n",line_number);
        return false;
    }
    if(is_comma(line)){
        fprintf(stderr,"Error: Duplicate comma on line %d\n",line_number);
        return false;
    }

    return true;
}

bool extra_text(char **line, int line_number){
    while (**line == ' ' && **line != '\0' && **line != '\n')
    {
        (*line)++;
    }
    if (**line == '\n' || **line == '\0')
    {
        return false;
    }
    fprintf(stderr, "Error: extra text in line %d\n", line_number);
    return true;
}

char *add_end(char *file_name, char *end)
{
    char *c, *new_file_name;
    new_file_name = (char *)my_malloc(sizeof(char) * MAX_LINE_LENGTH);
    strcpy(new_file_name, file_name);
    if ((c = strchr(new_file_name, '.')) != NULL)
    {
        *c = '\0';
    }

    strcat(new_file_name, end);
    return new_file_name;
}

/*בדיקת אם זה מילת פונקציה*/
bool check_opcode(char *word)
{
    char *opcode[] = {"mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};
    int i;
    for (i = 0; i < 16; i++)
    {
        if (strcmp(word, opcode[i]) == 0)
        {
            return true;
        }
    }
    return false;
}

int index_opcode(char *word){
    int i;
    char *opcode[] = {"mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};
    

    /* Find the index of the command */
    for (i = 0; i < 16; i++)
    {
        if (strcmp(word, opcode[i]) == 0)
        {
            return i;
        }
    }
    return -1;
}

bool check_Register(char *word)
{
    char *Register[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};
    int i;
    for (i = 0; i < 8; i++)
    {
        if (strcmp(word, Register[i]) == 0)
        {
            return true;
        }
    }
    return false;
}

bool check_directive(char *word)
{
    char *directive[] = {".data", ".string", ".entry", ".extern"};
    int i;
    for (i = 0; i < 4; i++)
    {
        if (strcmp(word, directive[i]) == 0)
        {
            return true;
        }
    }
    return false;
}
