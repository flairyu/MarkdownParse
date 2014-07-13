#include <stdio.h>

#include "markdownparse.h"

bool print_element(element *el, int depth) {
	printf("Depth %d: %d\n", depth, el->key);
}

int main()
{
	element *document = parse_markdown("Hello\n=====\n\nHello foo *this* is");

	traverse_tree(document, print_element);

	free_element_tree(document);
	return 0;
}
