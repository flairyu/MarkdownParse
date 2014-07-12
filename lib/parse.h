#ifndef PARSING_FUNCTIONS_H
#define PARSING_FUNCTIONS_H
/* parsing_functions.c - Functions for parsing markdown and
 * freeing element lists. */

#include "markdownparse.h"

element * parse_references(char *string, int extensions);
element * parse_notes(char *string, int extensions, element *reference_list);
element * parse_content(char *string, int extensions, element *reference_list, element *note_list);

void free_element(element *elt);

#endif
