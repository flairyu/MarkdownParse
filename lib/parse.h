#ifndef PARSING_FUNCTIONS_H
#define PARSING_FUNCTIONS_H
/* parsing_functions.c - Functions for parsing markdown and
 * freeing element lists. */

#include "markdownparse.h"

mdp_element *parse_references(char *string, int extensions);
mdp_element *parse_notes(char *string, int extensions, mdp_element *reference_list);
mdp_element *parse_content(char *string, int extensions, mdp_element *reference_list, mdp_element *note_list);

void free_element(mdp_element *elt);
void free_element_contents(mdp_element elt);

#endif
