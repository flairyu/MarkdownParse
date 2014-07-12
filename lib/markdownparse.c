#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "markdownparse.h"
#include "parse.h"

#define TABSTOP 4

/* preformat_text - allocate and copy text buffer while
 * performing tab expansion. */
static GString *preformat_text(const char *text) {
    GString *buf;
    char next_char;
    int charstotab;

    int len = 0;

    buf = g_string_new("");

    charstotab = TABSTOP;
    while ((next_char = *text++) != '\0') {
        switch (next_char) {
        case '\t':
            while (charstotab > 0)
                g_string_append_c(buf, ' '), len++, charstotab--;
            break;
        case '\n':
            g_string_append_c(buf, '\n'), len++, charstotab = TABSTOP;
            break;
        default:
            g_string_append_c(buf, next_char), len++, charstotab--;
        }
        if (charstotab == 0)
            charstotab = TABSTOP;
    }
    g_string_append(buf, "\n\n");
    return(buf);
}

/* process_raw_blocks - traverses an element list, replacing any RAW elements with
 * the result of parsing them as markdown text, and recursing into the children
 * of parent elements.  The result should be a tree of elements without any RAWs. */
static element * process_raw_blocks(element *input, int extensions, element *references, element *notes) {
    element *current = NULL;
    element *last_child = NULL;
    char *contents;
    current = input;

    while (current != NULL) {
        if (current->key == RAW) {
            /* \001 is used to indicate boundaries between nested lists when there
             * is no blank line.  We split the string by \001 and parse
             * each chunk separately. */
            contents = strtok(current->contents.str, "\001");
            current->key = LIST;
            current->children = parse_content(contents, extensions, references, notes);
            last_child = current->children;
            while ((contents = strtok(NULL, "\001"))) {
                while (last_child->next != NULL)
                    last_child = last_child->next;
                last_child->next = parse_content(contents, extensions, references, notes);
            }
            free(current->contents.str);
            current->contents.str = NULL;
        }
        if (current->children != NULL)
            current->children = process_raw_blocks(current->children, extensions, references, notes);
        current = current->next;
    }
    return input;
}

element *parse_markdown(const char *markdown, int extensions) {
    element *result;
    element *references;
    element *notes;
    GString *formatted_text;

    formatted_text = preformat_text(markdown);

    references = parse_references(formatted_text->str, extensions);
    notes = parse_notes(formatted_text->str, extensions, references);
    result = parse_content(formatted_text->str, extensions, references, notes);

    return process_raw_blocks(result, extensions, references, notes);
}

void free_element_tree(element * elt) {
    element * next = NULL;
    while (elt != NULL) {
        next = elt->next;
        free_element_contents(*elt);
        if (elt->children != NULL) {
            free_element_tree(elt->children);
            elt->children = NULL;
        }
        free(elt);
        elt = next;
    }
}
