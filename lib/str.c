#include "str.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

string *str_create(const char *init) {
	string *result;
	size_t len;

	result = malloc(sizeof(string));

	if (init == NULL) {
		init = "";
	}

	len = strlen(init);
	result->content = malloc((len + 1) * sizeof(char));
	result->length = len;
	strcpy(result->content, init);

	return result;
}

void str_free(string *str) {
	free(str->content);
	free(str);
}

char *str_free_container(string *str) {
	char *content;

	content = str->content;
	free(str);

	return content;
}

string *str_append(string *str, const char *annex) {
	size_t str_length;
	size_t result_length;
	char *new_content;

	assert(str != NULL);
	assert(annex != NULL);

	result_length = str->length + strlen(annex);
	new_content = malloc((result_length + 1) * sizeof(char));
	strcpy(new_content, str->content);
	strcpy(new_content + str->length, annex);

	free(str->content);
	str->content = new_content;
	str->length = result_length;

	return str;
}

string *str_append_char(string *str, char annex) {
	size_t result_length;
	char *new_content;

	result_length = str->length + 1;
	new_content = malloc((result_length + 1) * sizeof(char));
	strcpy(new_content, str->content);
	new_content[str->length] = annex;

	free(str->content);
	str->content = new_content;
	str->length = result_length;

	return str;
}
