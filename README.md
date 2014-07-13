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
