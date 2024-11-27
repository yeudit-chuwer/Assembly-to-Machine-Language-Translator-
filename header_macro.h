#ifndef HEADER_MACRO_H
#define HEADER_MACRO_H

#include "header_help_func.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Macro_table 
{
        char *name;
        char *value;
}Macro_table;



bool read_file(Macro_table **table, int *table_size, char *filename_source, char *filename_am);
Macro_table* find_macro(Macro_table *table, int size, char *name);

 

#endif