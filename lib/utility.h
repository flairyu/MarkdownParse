#ifndef UTILITY_FUNCTIONS_H
#define UTILITY_FUNCTIONS_H

#include <stdbool.h>
#include "str.h"
#include "markdownparse.h"

/* utility_functions.h - List manipulation functions, element
 * constructors, and macro definitions for leg markdown parser. */

char *strdup(char const *s);

/* cons - cons an element onto a list, returning pointer to new head */
mdp_element *cons(mdp_element *new, mdp_element *list);

/* reverse - reverse a list, returning pointer to new list */
mdp_element *reverse(mdp_element *list);
/* concat_string_list - concatenates string contents of list of STR elements.
 * Frees STR elements as they are added to the concatenation. */
string *concat_string_list(mdp_element *list);
/**********************************************************************

  Global variables used in parsing

 ***********************************************************************/

extern char *charbuf;          /* Buffer of characters to be parsed. */
extern mdp_element *references;    /* List of link references found. */
extern mdp_element *notes;         /* List of footnotes found. */
extern mdp_element *parse_result;  /* Results of parse. */
extern int syntax_extensions;  /* Syntax extensions selected. */

/**********************************************************************

  Auxiliary functions for parsing actions.
  These make it easier to build up data structures (including lists)
  in the parsing actions.

 ***********************************************************************/

/* mk_element - generic constructor for element */
mdp_element *mk_element(int key);

/* mk_str - constructor for STR element */
mdp_element *mk_str(char *string);

/* mk_str_from_list - makes STR element by concatenating a
 * reversed list of strings, adding optional extra newline */
mdp_element *mk_str_from_list(mdp_element *list, bool extra_newline);

/* mk_list - makes new list with key 'key' and children the reverse of 'lst'.
 * This is designed to be used with cons to build lists in a parser action.
 * The reversing is necessary because cons adds to the head of a list. */
mdp_element *mk_list(int key, mdp_element *lst);

/* mk_link - constructor for LINK element */
mdp_element *mk_link(mdp_element *label, char *url, char *title);
/* extension = returns true if extension is selected */
bool extension(int ext);

/* match_inlines - returns true if inline lists match (case-insensitive...) */
bool match_inlines(mdp_element *l1, mdp_element *l2);

/* find_reference - return true if link found in references matching label.
 * 'link' is modified with the matching url and title. */
bool find_reference(mdp_link *result, mdp_element *label);

/* find_note - return true if note found in notes matching label.
if found, 'result' is set to point to matched note. */

bool find_note(mdp_element **result, char *label);

#endif
