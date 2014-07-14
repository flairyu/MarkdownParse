MarkdownParse
=============

MarkdownParse is a C library for parsing [markdown][] documents. Unlike
many existing libraries, which only provide converted output, it
exposes a simple interface to access the tree of semantic elements for
a document.

The library is a derived work of [jgm/peg-markdown][] and likewise uses
a [parsing expression grammar (PEG)][] to define the syntax. This should
a allow easy modification and extension.

[parsing expression grammar (PEG)]: http://en.wikipedia.org/wiki/Parsing_expression_grammar 
[jgm/peg-markdown]: https://github.com/jgm/peg-markdown
[markdown]: http://daringfireball.net/projects/markdown/

Compilation
-----------

Where `$SOURCE_DIR` denotes the directory in which the MarkdownParse
source resides, create a build directory, generate the build scripts,
and run `make`:

```Shell
$ mkdir build && cd build
$ cmake $SOURCE_DIR
$ make
```

To install the library and header file:

```Shell
$ sudo make install
```

API Summary
-----------

```C
// Convert markdown into the given output format
char *format_markdown(char const *document, int format);

// Convert extended markdown into the given output format
char *format_extended_markdown(char const *document, int extensions, int format);

// Parse markdown into an element tree
element *parse_markdown(char const *string);

// Parse extended markdown into an element tree
element *parse_extended_markdown(char const *string, int extensions);

// Convert an element tree into the given output format
char *format_tree(element *root, int format);

// Apply function to each element in an element tree
void traverse_tree(element *root, bool (*func)(element *, int));

// Deallocate all elements in an element tree
void free_element_tree(element *root);
```

For more details, take a look at the public header file
[`markdownparse.h`](include/markdownparse.h).

Basic Usage
-----------

To use MarkdownParse, you'll need to `#include <markdownparse.h>` and
link to the library with the `-lmarkdownparse` compiler option.

To simply convert markdown to HTML, use `format_markdown` or
`format_extended_markdown`:

```C
char const *markdown = "MarkdownParse\n=====\n\nThis is *MarkdownParse*.";
char *html = format_markdown(markdown, FORMAT_HTML);

// Later...
free(html);
```

Don't forget to `free` the resulting string.

The process of formatting markdown as HTML (or any other format) is
divided into two steps: parse the markdown into an element tree, then
convert that tree to the appropriate function. These steps can be
performed individually with `parse_markdown` and `format_tree`
respectively.

`parse_markdown` takes some markdown and returns the root of a tree of
`element`s. This tree should later be deallocated with
`free_element_tree`. For example:

```C
char const *markdown = "MarkdownParse\n=====\n\nThis is *MarkdownParse*.";
element *document = parse_markdown(markdown);

// Process the document

free_element_tree(document);
```

Each `element` in the resulting tree has a `key` which denotes its
semantics. The key of the root element is always `DOCUMENT`. Each
`element` also has a list of children (which may be empty), the head of
which is pointed to by `children`. Each child has a `next` member
pointing to the next child in the list. You can quickly apply a function
to every `element` in a tree with `traverse_tree`.

Elements with some particular keys also have associated data in their 
`contents`:

- `STR`: `el->contents.str` contains the string contents
- `LINK` and `IMAGE`: `el->contents.link` contains hyperlink or image
  data, such as `label`, `url`, and `title`.

An element tree can be converted to HTML with `format_tree`:

```C
char *html = format_tree(document, FORMAT_HTML);
```

Example
-------

The following program parses a markdown document and replaces all of its
*emphasis* elements with **strong** elements before converting it to
HTML:

```C
#include <stdio.h>
#include <stdbool.h>
#include <markdownparse.h>

bool swap_emph_for_strong(element *el, int depth) {
	if (el->key == EMPH) {
		el->key = STRONG;
	}

	return true;
}

int main(int argc, const char *argv[]) {
	char const *markdown =
		"Test\n"
		"====\n"
		"\n"
		"This is an example *markdown document* to demonstrate\n"
		"the use of the *MarkdownParse* C library.";

	element *document = parse_markdown(markdown);

	traverse_tree(document, swap_emph_for_strong);

	printf("%s\n", format_tree(document, FORMAT_HTML));

	free_element_tree(document);
}
```

Output:

```
<h1>Test</h1>

<p>This is an example <strong>markdown document</strong> to demonstrate
the use of the <strong>MarkdownParse</strong> C library.</p>
```
