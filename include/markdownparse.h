#ifndef MARKDOWNPARSE_H
#define MARKDOWNPARSE_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Hyperlink data
typedef struct mdp_link {
	struct mdp_element *label;
	char *url;
	char *title;    
} mdp_link;

// Element contents
union mdp_contents {
	char *str;
	mdp_link *link;
};

// Semantic keys for elements
enum keys {
	MDP_LIST,         // An element used only to contain its children
	MDP_RAW,          // Raw markdown to be further processed
	MDP_SPACE,
	MDP_LINEBREAK,
	MDP_ELLIPSIS,
	MDP_EMDASH,
	MDP_ENDASH,
	MDP_APOSTROPHE,
	MDP_SINGLEQUOTED,
	MDP_DOUBLEQUOTED,
	MDP_STR,          // Text content
	MDP_LINK,
	MDP_IMAGE,
	MDP_CODE,
	MDP_HTML,
	MDP_EMPH,
	MDP_STRONG,
	MDP_STRIKE,
	MDP_PLAIN,
	MDP_PARA,
	MDP_LISTITEM,
	MDP_BULLETLIST,
	MDP_ORDEREDLIST,
	MDP_H1, MDP_H2, MDP_H3, MDP_H4, MDP_H5, MDP_H6,
	MDP_BLOCKQUOTE,
	MDP_VERBATIM,
	MDP_HTMLBLOCK,
	MDP_HRULE,
	MDP_REFERENCE,
	MDP_NOTE
};

// Element of the abstract syntax tree
typedef struct mdp_element {
	int key;
	union mdp_contents contents;
	struct mdp_element *children;
	struct mdp_element *next;
} mdp_element;

enum markdown_extensions {
	MDP_EXT_SMART         = 0x01,
	MDP_EXT_NOTES         = 0x02,
	MDP_EXT_FILTER_HTML   = 0x04,
	MDP_EXT_FILTER_STYLES = 0x08,
	MDP_EXT_STRIKE        = 0x10
};

enum formats {
	MDP_FORMAT_HTML = 0x01,
};

// Convert markdown into the given output format
char *mdp_format_markdown(char const *document, int format);

// Convert extended markdown into the given output format
char *mdp_format_extended_markdown(char const *document, int extensions, int format);

// Parse markdown into an element tree
mdp_element *mdp_parse_markdown(char const *string);

// Parse extended markdown into an element tree
mdp_element *mdp_parse_extended_markdown(char const *string, int extensions);

// Convert an element tree into the given output format
char *mdp_format_tree(mdp_element *root, int format);

// Apply function to each element in an element tree
void mdp_traverse_tree(mdp_element *root, bool (*func)(mdp_element *el, int depth));

// Deallocate all elements in an element tree
void mdp_free_element_tree(mdp_element *root);

#ifdef __cplusplus
}
#endif

#endif
