#include <stdio.h>

#include "markdownparse.h"

void print_strings(element* parent) {
	element *child;

	if (parent->key == STR) {
		printf("%s\n", parent->contents.str);
	} else {
		child = parent->children;
		while (child) {
			print_strings(child);
			child = child->next;
		}
	}
}

int main()
{
	element *document = parse_markdown("Hello\n=====\n\nHello foo *this* is", 0);

	print_strings(document);

	free_element_tree(document);
	return 0;
}
