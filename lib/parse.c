/* parsing_functions.c - Functions for parsing markdown and
 * freeing element lists. */

#include <stdlib.h>

#include "utility.h"
#include "parse.h"

/* These yy_* functions come from markdown_parser.c which is
 * generated from markdown_parser.leg
 * */
typedef int (*yyrule)();

extern int yyparse();
extern int yyparsefrom(yyrule);
extern int yy_References();
extern int yy_Notes();
extern int yy_Doc();

/* free_element_contents - free element contents depending on type */
void free_element_contents(mdp_element element) {
	switch (element.key) {
		case MDP_STR:
		case MDP_SPACE:
		case MDP_RAW:
		case MDP_HTMLBLOCK:
		case MDP_HTML:
		case MDP_VERBATIM:
		case MDP_CODE:
		case MDP_NOTE:
			free(element.contents.str);
			element.contents.str = NULL;
			break;
		case MDP_LINK:
		case MDP_IMAGE:
		case MDP_REFERENCE:
			free(element.contents.link->url);
			element.contents.link->url = NULL;
			free(element.contents.link->title);
			element.contents.link->title = NULL;
			mdp_free_element_tree(element.contents.link->label);
			free(element.contents.link);
			element.contents.link = NULL;
			break;
	}
}

/* free_element - free element and contents */
void free_element(mdp_element *element) {
	free_element_contents(*element);
	free(element);
}

mdp_element *parse_references(char *string, int extensions) {
	syntax_extensions = extensions;

	char *oldcharbuf = charbuf;
	charbuf = string;
	// First pass to collect references
	yyparsefrom(yy_References);
	charbuf = oldcharbuf;

	return references;
}

mdp_element *parse_notes(char *string, int extensions, mdp_element *reference_list) {
	notes = NULL;
	syntax_extensions = extensions;

	if (extension(MDP_EXT_NOTES)) {
		char *oldcharbuf = charbuf;
		references = reference_list;
		charbuf = string;
		// Second pass to collect notes
		yyparsefrom(yy_Notes);
		charbuf = oldcharbuf;
	}

	return notes;
}

mdp_element *parse_content(char *string, int extensions, mdp_element *reference_list, mdp_element *note_list) {
	syntax_extensions = extensions;
	references = reference_list;
	notes = note_list;

	char *oldcharbuf = charbuf;
	charbuf = string;

	yyparsefrom(yy_Doc);

	charbuf = oldcharbuf;          /* restore charbuf to original value */
	return parse_result;
}
