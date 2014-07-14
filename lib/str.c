#include "str.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

static inline size_t nearest_power(size_t num) {
	size_t const MAX_SIZE = -1;

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

string *str_create(char const *init) {
	string *result = malloc(sizeof(string));
	result->length = 0;
	result->allocated_length = 0;

	if (init == NULL) {
		init = "";
	}

	size_t length = strlen(init);

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
	char *content = str->content;
	free(str);

	return content;
}

string *str_append(string *str, char const *annex) {
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

string *str_append_format(string *str, const char *format, ...) {
	assert(str != NULL);

	va_list args;
	va_start(args, format);
	int annex_length = vsnprintf(NULL, 0, format, args);
	va_end(args);

	char *annex = malloc((annex_length + 1) * sizeof(char));

	va_start(args, format);
	vsnprintf(annex, annex_length + 1, format, args);
	va_end(args);

	str_append(str, annex);

	free(annex);
	va_end(args);

	return str;
}
