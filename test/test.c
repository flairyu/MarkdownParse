#include <stdio.h>

#include "markdownparse.h"

int main()
{
	element *paragraph = parse_markdown("Hello foo *this* is", 0);
	element *child = root->children;
	do {
		printf("%d\n", child->key);
	} while (child = child->next);
	return 0;
}
