#include "formats.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static int padded = 2;
static element **endnotes = NULL;
static int num_endnotes = 0;

static void pad(string *out, int num) {
	while (num-- > padded) {
		str_append_format(out, "\n");
	}
	padded = num;
}

/* add_endnote - add an endnote to global endnotes list. */
static void add_endnote(element *endnote) {
	num_endnotes++;

	element **new_endnotes = malloc(num_endnotes * sizeof(element*));
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
		element *note = endnotes[i];
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

static void format_children_html(string *output, element *parent) {
	element *child = parent->children;
	while (child != NULL) {
		format_tree_html(output, child);

		child = child->next;
	}
}

void format_tree_html(string *output, element *root) {
	int lev;
	switch (root->key) {
		case DOCUMENT:
			format_children_html(output, root);
			break;
		case SPACE:
			str_append_format(output, "%s", root->contents.str);
			break;
		case LINEBREAK:
			str_append_format(output, "<br/>\n");
			break;
		case STR:
			format_string_html(output, root->contents.str);
			break;
		case ELLIPSIS:
			str_append_format(output, "&hellip;");
			break;
		case EMDASH:
			str_append_format(output, "&mdash;");
			break;
		case ENDASH:
			str_append_format(output, "&ndash;");
			break;
		case APOSTROPHE:
			str_append_format(output, "&rsquo;");
			break;
		case SINGLEQUOTED:
			str_append_format(output, "&lsquo;");
			format_children_html(output, root);
			str_append_format(output, "&rsquo;");
			break;
		case DOUBLEQUOTED:
			str_append_format(output, "&ldquo;");
			format_children_html(output, root);
			str_append_format(output, "&rdquo;");
			break;
		case CODE:
			str_append_format(output, "<code>");
			format_string_html(output, root->contents.str);
			str_append_format(output, "</code>");
			break;
		case HTML:
			str_append_format(output, "%s", root->contents.str);
			break;
		case LINK:
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
		case IMAGE:
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
		case EMPH:
			str_append_format(output, "<em>");
			format_children_html(output, root);
			str_append_format(output, "</em>");
			break;
		case STRONG:
			str_append_format(output, "<strong>");
			format_children_html(output, root);
			str_append_format(output, "</strong>");
			break;
		case STRIKE:
			str_append_format(output, "<del>");
			format_children_html(output, root);
			str_append_format(output, "</del>");
			break;
		case LIST:
			format_children_html(output, root);
			break;
		case RAW:
			/* Shouldn't occur - these are handled by process_raw_blocks() */
			assert(root->key != RAW);
			break;
		case H1: case H2: case H3: case H4: case H5: case H6:
			lev = root->key - H1 + 1;  /* assumes H1 ... H6 are in order */
			pad(output, 2);
			str_append_format(output, "<h%1d>", lev);
			format_children_html(output, root);
			str_append_format(output, "</h%1d>", lev);
			padded = 0;
			break;
		case PLAIN:
			pad(output, 1);
			format_children_html(output, root);
			padded = 0;
			break;
		case PARA:
			pad(output, 2);
			str_append_format(output, "<p>");
			format_children_html(output, root);
			str_append_format(output, "</p>");
			padded = 0;
			break;
		case HRULE:
			pad(output, 2);
			str_append_format(output, "<hr />");
			padded = 0;
			break;
		case HTMLBLOCK:
			pad(output, 2);
			str_append_format(output, "%s", root->contents.str);
			padded = 0;
			break;
		case VERBATIM:
			pad(output, 2);
			str_append_format(output, "%s", "<pre><code>");
			format_string_html(output, root->contents.str);
			str_append_format(output, "%s", "</code></pre>");
			padded = 0;
			break;
		case BULLETLIST:
			pad(output, 2);
			str_append_format(output, "%s", "<ul>");
			padded = 0;
			format_children_html(output, root);
			pad(output, 1);
			str_append_format(output, "%s", "</ul>");
			padded = 0;
			break;
		case ORDEREDLIST:
			pad(output, 2);
			str_append_format(output, "%s", "<ol>");
			padded = 0;
			format_children_html(output, root);
			pad(output, 1);
			str_append_format(output, "</ol>");
			padded = 0;
			break;
		case LISTITEM:
			pad(output, 1);
			str_append_format(output, "<li>");
			padded = 2;
			format_children_html(output, root);
			str_append_format(output, "</li>");
			padded = 0;
			break;
		case BLOCKQUOTE:
			pad(output, 2);
			str_append_format(output, "<blockquote>\n");
			padded = 2;
			format_children_html(output, root);
			pad(output, 1);
			str_append_format(output, "</blockquote>");
			padded = 0;
			break;
		case REFERENCE:
			/* Nonprinting */
			break;
		case NOTE:
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

	if (root->key == DOCUMENT && endnotes != NULL) {
		pad(output, 2);
		format_endnotes_html(output);
	}
}
