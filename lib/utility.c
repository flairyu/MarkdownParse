/* utility_functions.c - List manipulation functions, element
 * constructors, and macro definitions for leg markdown parser. */

#include "utility.h"

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#include "parse.h"


char *strdup(char const *s) {
	char *d = malloc(strlen(s) + 1);
	if (d == NULL) return NULL;
	strcpy(d, s);
	return d;
}

static bool strCaseCmp(char const *s1, char const *s2) {
	while (tolower(*s1) == tolower(*s2) &&
	       *s1 != '\0' && *s2 != '\0') {
		s1++;
		s2++;
	}

	return tolower(*s1) - tolower(*s2);
}

/**********************************************************************

	List manipulation functions

 ***********************************************************************/

/* cons - cons an element onto a list, returning pointer to new head */
mdp_element *cons(mdp_element *new, mdp_element *list) {
	assert(new != NULL);
	new->next = list;
	return new;
}

/* reverse - reverse a list, returning pointer to new list */
mdp_element *reverse(mdp_element *list) {
	mdp_element *new = NULL;
	while (list != NULL) {
		mdp_element *next = list->next;
		new = cons(list, new);
		list = next;
	}

	return new;
}

/* concat_string_list - concatenates string contents of list of STR elements.
 * Frees STR elements as they are added to the concatenation. */
string *concat_string_list(mdp_element *list) {
	string *result = str_create("");
	while (list != NULL) {
		assert(list->key == MDP_STR);
		assert(list->contents.str != NULL);

		str_append(result, list->contents.str);
		mdp_element *next = list->next;
		free_element(list);
		list = next;
	}

	return result;
}

/**********************************************************************

	Global variables used in parsing

 ***********************************************************************/

char *charbuf = "";     /* Buffer of characters to be parsed. */
mdp_element *references = NULL;    /* List of link references found. */
mdp_element *notes = NULL;         /* List of footnotes found. */
mdp_element *parse_result;  /* Results of parse. */
int syntax_extensions;  /* Syntax extensions selected. */

/**********************************************************************

	Auxiliary functions for parsing actions.
	These make it easier to build up data structures (including lists)
	in the parsing actions.

 ***********************************************************************/

/* mk_element - generic constructor for element */
mdp_element *mk_element(int key) {
	mdp_element *result = malloc(sizeof(mdp_element));
	result->key = key;
	result->children = NULL;
	result->next = NULL;
	result->contents.str = NULL;

	return result;
}

/* mk_str - constructor for STR element */
mdp_element *mk_str(char *string) {
	assert(string != NULL);
	mdp_element *result = mk_element(MDP_STR);
	result->contents.str = strdup(string);

	return result;
}

/* mk_str_from_list - makes STR element by concatenating a
 * reversed list of strings, adding optional extra newline */
mdp_element *mk_str_from_list(mdp_element *list, bool extra_newline) {
	string *c = concat_string_list(reverse(list));

	if (extra_newline) {
		str_append(c, "\n");
	}

	mdp_element *result = mk_element(MDP_STR);
	result->contents.str = c->content;
	str_free_container(c);

	return result;
}

/* mk_list - makes new list with key 'key' and children the reverse of 'lst'.
 * This is designed to be used with cons to build lists in a parser action.
 * The reversing is necessary because cons adds to the head of a list. */
mdp_element *mk_list(int key, mdp_element *lst) {
	mdp_element *result = mk_element(key);
	result->children = reverse(lst);

	return result;
}

static int done = 0;

/* mk_link - constructor for LINK element */
mdp_element *mk_link(mdp_element *label, char *url, char *title) {
	mdp_element *result = mk_element(MDP_LINK);
	result->contents.link = malloc(sizeof(mdp_link));
	result->contents.link->label = label;
	result->contents.link->url = strdup(url);
	result->contents.link->title = strdup(title);

	return result;
}

/* extension = returns true if extension is selected */
bool extension(int ext) {
	return syntax_extensions & ext;
}

/* match_inlines - returns true if inline lists match (case-insensitive...) */
bool match_inlines(mdp_element *l1, mdp_element *l2) {
	while (l1 != NULL && l2 != NULL) {
		if (l1->key != l2->key)
			return false;
		switch (l1->key) {
			case MDP_SPACE:
			case MDP_LINEBREAK:
			case MDP_ELLIPSIS:
			case MDP_EMDASH:
			case MDP_ENDASH:
			case MDP_APOSTROPHE:
				break;
			case MDP_CODE:
			case MDP_STR:
			case MDP_HTML:
				if (strCaseCmp(l1->contents.str, l2->contents.str) != 0)
					return false;
				break;
			case MDP_EMPH:
			case MDP_STRONG:
			case MDP_LIST:
			case MDP_SINGLEQUOTED:
			case MDP_DOUBLEQUOTED:
				if (!match_inlines(l1->children, l2->children))
					return false;
				break;
			case MDP_LINK:
			case MDP_IMAGE:
				return false;  /* No links or images within links */
			default:
				fprintf(stderr, "match_inlines encountered unknown key = %d\n", l1->key);
				exit(EXIT_FAILURE);
				break;
		}
		l1 = l1->next;
		l2 = l2->next;
	}
	return l1 == NULL && l2 == NULL;  /* return true if both lists exhausted */
}

/* find_reference - return true if link found in references matching label.
 * 'link' is modified with the matching url and title. */
bool find_reference(mdp_link *result, mdp_element *label) {
	mdp_element *cur = references;  /* pointer to walk up list of references */

	while (cur != NULL) {
		mdp_link *curitem = cur->contents.link;

		if (match_inlines(label, curitem->label)) {
			*result = *curitem;
			return true;
		} else {
			cur = cur->next;
		}
	}

	return false;
}

/* find_note - return true if note found in notes matching label.
	 if found, 'result' is set to point to matched note. */
bool find_note(mdp_element **result, char *label) {
	mdp_element *cur = notes;

	while (cur != NULL) {
		if (strcmp(label, cur->contents.str) == 0) {
			*result = cur;
			return true;
		} else {
			cur = cur->next;
		}
	}

	return false;
}
