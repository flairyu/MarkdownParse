#ifndef STRING_H
#define STRING_H

#include <string.h>

struct string {
	char *content;
	size_t length;
};

typedef struct string string;

string *str_create(const char *init);
void str_free(string *str);
char *str_free_container(string *str);
string *str_append(string *str, const char *annex);
string *str_append_char(string *str, char annex);

#endif
