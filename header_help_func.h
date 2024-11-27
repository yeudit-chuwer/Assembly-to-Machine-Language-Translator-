#ifndef HEADER_HELP_H
#define HEADER_HELP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>



#define MAX_LINE_LENGTH 81



typedef enum {false, true} bool;

char *add_end(char *file_name, char *end);
char *get_line(FILE **file);
char *get_word(char **line);
void skipSpace(char **line);
bool is_comma(char **line);
bool check_comma(char **line, int line_number);
bool extra_text(char **line, int line_number);
void* my_malloc(int size);

bool check_opcode(char * word);
int index_opcode(char *word);
bool check_Register(char * word);
bool check_directive(char * word);




#endif