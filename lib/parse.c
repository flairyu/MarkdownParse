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
void free_element_contents(element element) {
	switch (element.key) {
		case STR:
		case SPACE:
		case RAW:
		case HTMLBLOCK:
		case HTML:
		case VERBATIM:
		case CODE:
		case NOTE:
			free(element.contents.str);
			element.contents.str = NULL;
			break;
		case LINK:
		case IMAGE:
		case REFERENCE:
			free(element.contents.link->url);
			element.contents.link->url = NULL;
			free(element.contents.link->title);
			element.contents.link->title = NULL;
			free_element_tree(element.contents.link->label);
			free(element.contents.link);
			element.contents.link = NULL;
			break;
	}
}

/* free_element - free element and contents */
void free_element(element *element) {
	free_element_contents(*element);
	free(element);
}

element *parse_references(char *string, int extensions) {
	syntax_extensions = extensions;

	char *oldcharbuf = charbuf;
	charbuf = string;
	// First pass to collect references
	yyparsefrom(yy_References);
	charbuf = oldcharbuf;

	return references;
}

element *parse_notes(char *string, int extensions, element *reference_list) {
	notes = NULL;
	syntax_extensions = extensions;

	if (extension(EXT_NOTES)) {
		char *oldcharbuf = charbuf;
		references = reference_list;
		charbuf = string;
		// Second pass to collect notes
		yyparsefrom(yy_Notes);
		charbuf = oldcharbuf;
	}

	return notes;
}

element *parse_content(char *string, int extensions, element *reference_list, element *note_list) {
	syntax_extensions = extensions;
	references = reference_list;
	notes = note_list;

	char *oldcharbuf = charbuf;
	charbuf = string;

	yyparsefrom(yy_Doc);

	charbuf = oldcharbuf;          /* restore charbuf to original value */
	return parse_result;
}
