#ifndef MARKDOWNPARSE_H
#define MARKDOWNPARSE_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Hyperlink data
typedef struct link {
	struct element *label;
	char *url;
	char *title;    
} link;

// Element contents
union contents {
	char *str;
	link *link;
};

// Semantic keys for elements
enum keys {
	DOCUMENT,     // The root of a markdown document
	LIST,         // A generic list
	RAW,          // Raw markdown to be further processed
	SPACE,
	LINEBREAK,
	ELLIPSIS,
	EMDASH,
	ENDASH,
	APOSTROPHE,
	SINGLEQUOTED,
	DOUBLEQUOTED,
	STR,          // Text content
	LINK,
	IMAGE,
	CODE,
	HTML,
	EMPH,
	STRONG,
	STRIKE,
	PLAIN,
	PARA,
	LISTITEM,
	BULLETLIST,
	ORDEREDLIST,
	H1, H2, H3, H4, H5, H6,
	BLOCKQUOTE,
	VERBATIM,
	HTMLBLOCK,
	HRULE,
	REFERENCE,
	NOTE
};

// Element of the abstract syntax tree
typedef struct element {
	int key;
	union contents contents;
	struct element *children;
	struct element *next;
} element;

enum markdown_extensions {
	EXT_SMART         = 0x01,
	EXT_NOTES         = 0x02,
	EXT_FILTER_HTML   = 0x04,
	EXT_FILTER_STYLES = 0x08,
	EXT_STRIKE        = 0x10
};

enum formats {
	FORMAT_HTML = 0x01,
};

element *parse_markdown(char const *string);
element *parse_extended_markdown(char const *string, int extensions);
char *format_tree(element *tree, int format);
char *format_markdown(char const *document, int format);
void traverse_tree(element *tree, bool (*func)(element *, int));
void free_element_tree(element *tree);

#ifdef __cplusplus
}
#endif

#endif
