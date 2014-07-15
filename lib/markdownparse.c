#include <stdlib.h>
#include <string.h>

#include "markdownparse.h"
#include "parse.h"
#include "utility.h"
#include "str.h"
#include "formats/formats.h"

/* preformat_text - allocate and copy text buffer while
 * performing tab expansion. */
static string *preformat_text(char const *text) {
	int const TABSTOP = 4;

	string *buffer = str_create("");
	int length = 0;
	int num_tab_characters = TABSTOP;

	char next_char;
	while ((next_char = *text++) != '\0') {
		switch (next_char) {
			case '\t':
				while (num_tab_characters > 0) {
					str_append_char(buffer, ' ');
					length++;
					num_tab_characters--;
				}
				break;
			case '\n':
				str_append_char(buffer, '\n');
				length++;
				num_tab_characters = TABSTOP;
				break;
			default:
				str_append_char(buffer, next_char);
				length++;
				num_tab_characters--;
		}

		if (num_tab_characters == 0) {
			num_tab_characters = TABSTOP;
		}
	}

	str_append(buffer, "\n\n");
	return buffer;
}

/* process_raw_blocks - traverses an element list, replacing any RAW elements with
 * the result of parsing them as markdown text, and recursing into the children
 * of parent elements.  The result should be a tree of elements without any RAWs. */
static element * process_raw_blocks(element *input, int extensions, element *references, element *notes) {
	element *current = input;
	while (current != NULL) {
		if (current->key == RAW) {
			/* \001 is used to indicate boundaries between nested lists when there
			 * is no blank line.  We split the string by \001 and parse
			 * each chunk separately. */
			char *contents = strtok(current->contents.str, "\001");
			current->key = LIST;
			current->children = parse_content(contents, extensions, references, notes);

			element *last_child = current->children;
			while ((contents = strtok(NULL, "\001"))) {
				while (last_child->next != NULL) {
					last_child = last_child->next;
				}

				last_child->next = parse_content(contents, extensions, references, notes);
			}

			free(current->contents.str);
			current->contents.str = NULL;
		}

		if (current->children != NULL) {
			current->children = process_raw_blocks(current->children, extensions, references, notes);
		}

		current = current->next;
	}

	return input;
}

char *format_markdown(char const *markdown, int format) {
	return format_extended_markdown(markdown, 0, format);
}

char *format_extended_markdown(char const *markdown, int extensions, int format) {
	element *document = parse_extended_markdown(markdown, extensions);
	char *output = format_tree(document, format);
	free_element_tree(document);

	return output;
}

element *parse_markdown(char const *markdown) {
	return parse_extended_markdown(markdown, 0);
}

element *parse_extended_markdown(char const *markdown, int extensions) {
	string *formatted_text = preformat_text(markdown);

	element *references = parse_references(formatted_text->content, extensions);
	element *notes = parse_notes(formatted_text->content, extensions, references);
	element *content = parse_content(formatted_text->content, extensions, references, notes);

	content = process_raw_blocks(content, extensions, references, notes);

	element *document = mk_element(LIST);
	document->children = content;

	return document;
}

char *format_tree(element *root, int format) {
	string *formatted = str_create("");

	if (format == FORMAT_HTML) {
		format_tree_html(formatted, root);
	}

	char *result = formatted->content;
	str_free_container(formatted);

	return result;
}

static void traverse_tree_depth(element *root, bool (*func)(element *, int), int depth) {
	if (func(root, depth)) {
		element *child = root->children;

		while (child != NULL) {
			traverse_tree_depth(child, func, depth + 1);
			child = child->next;
		}
	}
}

void traverse_tree(element *root, bool (*func)(element *, int)) {
	traverse_tree_depth(root, func, 0);
}

void free_element_tree(element *root) {
	element *child = root->children;

	while (child != NULL) {
		free_element_tree(child);
		child = child->next;
	}

	free_element_contents(*root);
	free(root);
}
