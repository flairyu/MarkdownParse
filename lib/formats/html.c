#include "formats.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static int padded = 2;
static mdp_element **endnotes = NULL;
static int num_endnotes = 0;

static void pad(string *out, int num) {
	while (num-- > padded) {
		str_append_format(out, "\n");
	}
	padded = num;
}

/* add_endnote - add an endnote to global endnotes list. */
static void add_endnote(mdp_element *endnote) {
	num_endnotes++;

	mdp_element **new_endnotes = malloc(num_endnotes * sizeof(mdp_element*));
	for (int i = 0; i < num_endnotes-1; i++) {
		new_endnotes[i] = endnotes[i];
	}
	free(endnotes);
	endnotes = new_endnotes;

	endnotes[num_endnotes-1] = endnote;
}

static void format_endnotes_html(string *output) {
	int counter = 0;

	if (endnotes == NULL) {
		return;
	}

	str_append_format(output, "<hr/>\n<ol id=\"notes\">");

	for (int i = num_endnotes - 1; i >= 0; i--) {
		mdp_element *note = endnotes[i];
		counter++;
		pad(output, 1);
		str_append_format(output, "<li id=\"fn%d\">\n", counter);
		padded = 2;
		format_tree_html(output, note->children);
		str_append_format(output, " <a href=\"#fnref%d\" title=\"Jump back to reference\">[back]</a>", counter);
		pad(output, 1);
		str_append_format(output, "</li>");
	}

	pad(output, 1);
	str_append_format(output, "</ol>");

	free(endnotes);
	num_endnotes = 0;
}

static void format_string_html(string *output, char *str) {
	while (*str != '\0') {
		switch (*str) {
			case '&':
				str_append_format(output, "&amp;");
				break;
			case '<':
				str_append_format(output, "&lt;");
				break;
			case '>':
				str_append_format(output, "&gt;");
				break;
			case '"':
				str_append_format(output, "&quot;");
				break;
			default:
				str_append_char(output, *str);
		}
		str++;
	}
}

static void format_element_html(string *output, mdp_element *toor);

static void format_children_html(string *output, mdp_element *parent) {
	mdp_element *child = parent->children;
	while (child != NULL) {
		format_element_html(output, child);

		child = child->next;
	}
}

static void format_element_html(string *output, mdp_element *root) {
	int lev;
	switch (root->key) {
		case MDP_SPACE:
			str_append_format(output, "%s", root->contents.str);
			break;
		case MDP_LINEBREAK:
			str_append_format(output, "<br/>\n");
			break;
		case MDP_STR:
			format_string_html(output, root->contents.str);
			break;
		case MDP_ELLIPSIS:
			str_append_format(output, "&hellip;");
			break;
		case MDP_EMDASH:
			str_append_format(output, "&mdash;");
			break;
		case MDP_ENDASH:
			str_append_format(output, "&ndash;");
			break;
		case MDP_APOSTROPHE:
			str_append_format(output, "&rsquo;");
			break;
		case MDP_SINGLEQUOTED:
			str_append_format(output, "&lsquo;");
			format_children_html(output, root);
			str_append_format(output, "&rsquo;");
			break;
		case MDP_DOUBLEQUOTED:
			str_append_format(output, "&ldquo;");
			format_children_html(output, root);
			str_append_format(output, "&rdquo;");
			break;
		case MDP_CODE:
			str_append_format(output, "<code>");
			format_string_html(output, root->contents.str);
			str_append_format(output, "</code>");
			break;
		case MDP_HTML:
			str_append_format(output, "%s", root->contents.str);
			break;
		case MDP_LINK:
			str_append_format(output, "<a href=\"");
			format_string_html(output, root->contents.link->url);
			str_append_format(output, "\"");
			if (strlen(root->contents.link->title) > 0) {
				str_append_format(output, " title=\"");
				format_string_html(output, root->contents.link->title);
				str_append_format(output, "\"");
			}
			str_append_format(output, ">");
			format_tree_html(output, root->contents.link->label);
			str_append_format(output, "</a>");
			break;
		case MDP_IMAGE:
			str_append_format(output, "<img src=\"");
			format_string_html(output, root->contents.link->url);
			str_append_format(output, "\" alt=\"");
			format_children_html(output, root->contents.link->label);
			str_append_format(output, "\"");
			if (strlen(root->contents.link->title) > 0) {
				str_append_format(output, " title=\"");
				format_string_html(output, root->contents.link->title);
				str_append_format(output, "\"");
			}
			str_append_format(output, " />");
			break;
		case MDP_EMPH:
			str_append_format(output, "<em>");
			format_children_html(output, root);
			str_append_format(output, "</em>");
			break;
		case MDP_STRONG:
			str_append_format(output, "<strong>");
			format_children_html(output, root);
			str_append_format(output, "</strong>");
			break;
		case MDP_STRIKE:
			str_append_format(output, "<del>");
			format_children_html(output, root);
			str_append_format(output, "</del>");
			break;
		case MDP_LIST:
			format_children_html(output, root);
			break;
		case MDP_RAW:
			/* Shouldn't occur - these are handled by process_raw_blocks() */
			assert(root->key != MDP_RAW);
			break;
		case MDP_H1: case MDP_H2: case MDP_H3: case MDP_H4: case MDP_H5: case MDP_H6:
			lev = root->key - MDP_H1 + 1;  /* assumes H1 ... H6 are in order */
			pad(output, 2);
			str_append_format(output, "<h%1d>", lev);
			format_children_html(output, root);
			str_append_format(output, "</h%1d>", lev);
			padded = 0;
			break;
		case MDP_PLAIN:
			pad(output, 1);
			format_children_html(output, root);
			padded = 0;
			break;
		case MDP_PARA:
			pad(output, 2);
			str_append_format(output, "<p>");
			format_children_html(output, root);
			str_append_format(output, "</p>");
			padded = 0;
			break;
		case MDP_HRULE:
			pad(output, 2);
			str_append_format(output, "<hr />");
			padded = 0;
			break;
		case MDP_HTMLBLOCK:
			pad(output, 2);
			str_append_format(output, "%s", root->contents.str);
			padded = 0;
			break;
		case MDP_VERBATIM:
			pad(output, 2);
			str_append_format(output, "%s", "<pre><code>");
			format_string_html(output, root->contents.str);
			str_append_format(output, "%s", "</code></pre>");
			padded = 0;
			break;
		case MDP_BULLETLIST:
			pad(output, 2);
			str_append_format(output, "%s", "<ul>");
			padded = 0;
			format_children_html(output, root);
			pad(output, 1);
			str_append_format(output, "%s", "</ul>");
			padded = 0;
			break;
		case MDP_ORDEREDLIST:
			pad(output, 2);
			str_append_format(output, "%s", "<ol>");
			padded = 0;
			format_children_html(output, root);
			pad(output, 1);
			str_append_format(output, "</ol>");
			padded = 0;
			break;
		case MDP_LISTITEM:
			pad(output, 1);
			str_append_format(output, "<li>");
			padded = 2;
			format_children_html(output, root);
			str_append_format(output, "</li>");
			padded = 0;
			break;
		case MDP_BLOCKQUOTE:
			pad(output, 2);
			str_append_format(output, "<blockquote>\n");
			padded = 2;
			format_children_html(output, root);
			pad(output, 1);
			str_append_format(output, "</blockquote>");
			padded = 0;
			break;
		case MDP_REFERENCE:
			/* Nonprinting */
			break;
		case MDP_NOTE:
			/* if contents.str == 0, then print note; else ignore, since this
			 * is a note block that has been incorporated into the notes list */
			if (root->contents.str == 0) {
				add_endnote(root);
				str_append_format(output, "<a class=\"noteref\" id=\"fnref%d\" href=\"#fn%d\" title=\"Jump to note %d\">[%d]</a>",
				                  num_endnotes, num_endnotes, num_endnotes, num_endnotes);
			}
			break;
		default: 
			fprintf(stderr, "print_html_element encountered unknown element key = %d\n", root->key); 
			exit(EXIT_FAILURE);
	}
}

void format_tree_html(string *output, mdp_element *root) {
	format_element_html(output, root);

	if (endnotes != NULL) {
		pad(output, 2);
		format_endnotes_html(output);
	}
}
