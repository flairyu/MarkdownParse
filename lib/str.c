#include "str.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

static inline size_t nearest_power(size_t num) {
	const size_t MAX_SIZE = -1;

	if (num > MAX_SIZE / 2) {
		return MAX_SIZE;
	}

	size_t power = 1;
	while (power < num) {
		power <<= 1;
	}

	return power;
}

static void str_expand(string *str, size_t length) {
	if (str->length + length >= str->allocated_length) {
		str->allocated_length = nearest_power(str->length + length + 1);
		str->content = realloc(str->content, str->allocated_length);
	}
}

string *str_create(const char *init) {
	string *result;
	size_t length;

	result = malloc(sizeof(string));
	result->length = 0;
	result->allocated_length = 0;

	if (init == NULL) {
		init = "";
	}

	length = strlen(init);

	str_expand(result, length + 2);
	strcpy(result->content, init);
	result->length = length;

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
	assert(str != NULL);
	assert(annex != NULL);

	str_expand(str, strlen(annex));
	strcpy(str->content + str->length, annex);
	str->length = str->length + strlen(annex);

	return str;
}

string *str_append_char(string *str, char annex) {
	assert(str != NULL);

	str_expand(str, 1);
	str->content[str->length] = annex;
	str->length = str->length + 1;

	return str;
}
