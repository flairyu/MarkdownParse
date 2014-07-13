#include <stdio.h>

#include "markdownparse.h"

bool print_element(element *el, int depth) {
	printf("Depth %d: %d\n", depth, el->key);
}

int main()
{
	char const *markdown = "Hello\n=====\n\nHello foo *this* is";

	element *document = parse_markdown(markdown);

	traverse_tree(document, print_element);

	printf("%s\n", format_markdown(markdown, FORMAT_HTML));

	free_element_tree(document);
	return 0;
}
