#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "markdownparse.h"

int main()
{
	char buffer[4096];
	char *input = NULL;
	long num_characters = 0;

	while (fgets(buffer, 4096, stdin)) {
		long prev_num_characters = num_characters;
		input = realloc(input, num_characters + strlen(buffer) + 1);
		strcpy(input + prev_num_characters, buffer);
		num_characters += strlen(buffer);
	}

	printf("%s\n", mdp_format_markdown(input, MDP_FORMAT_HTML));

	return 0;
}
