MarkdownParse
=============

MarkdownParse is a C library for parsing [markdown][] documents. Unlike
many existing libraries, which only provide converted output, it
exposes a simple interface to access the abstract syntax tree of a
document.

The library is a derived work of [jgm/peg-markdown][] and likewise uses
a [parsing expression grammar (PEG)][] to define the syntax. This should
a allow easy modification and extension.

[parsing expression grammar (PEG)]: http://en.wikipedia.org/wiki/Parsing_expression_grammar 
[jgm/peg-markdown]: https://github.com/jgm/peg-markdown
[markdown]: http://daringfireball.net/projects/markdown/

Compilation
-----------

Where `$SOURCE_DIR` denotes the directory into which the MarkdownParse
source has been cloned, create a build directory, generate the build
scripts, and run `make`:

    $ mkdir build && cd build
    $ cmake $SOURCE_DIR
    $ make

To install the library and header file:

    $ sudo make install

Usage
-----

To use MarkdownParse in your code, include the `<markdownparse.h>`
header and call `parse_markdown`. This returns the root of a tree of
`element`s. This tree should later be deallocated with
`free_element_tree`. For example:

    const char* markdown = "MarkdownParse\n=====\n\nThis is *MarkdownParse*.";
    element *document = parse_markdown(markdown);

    // Process the document

    free_element_tree(document);

Each `element` in the resulting tree has a `key` which denotes its
semantics. The key of the root element is always `DOCUMENT`. Each
`element` also has a list of children (which may be
empty), the head of which is pointed to by `children`. Each child
has a `next` member pointing to the next child in the list. To
iterate over an element `el`s children, do the following:

    element *child = el->children;
    while (child != NULL) {
      // Process child

      child = child->next;
    }
