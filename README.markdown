A toy CSS parser
================

A CSS validator for the command line, with a bit of linting built in.

In progress, and intended only as a mechanism to teach myself C: if you need
to validate/minify CSS, I'd suggest using something like [YUI Compressor][]
or Norm's [CSS Prepare][].

[YUI Compressor]: http://developer.yahoo.com/yui/compressor/
[CSS Prepare]: http://github.com/norm/CSS-Prepare

Done
----

*   Base structure (`StatefulString`, `Token`, and `Tokenizer` "objects").
*   Unit test framework ([Check][])
*   Tokenization, consistent with the rules laid out in [the spec][css3].

[css3]: http://www.w3.org/TR/css3-syntax/#tokenization
[check]: http://check.sourceforge.net/

Next Steps
----------

*   Decide on an error handling mechanism.
*   Turn the token stream into a parse tree.
*   Validate the tree.
*   Implementing some linting tests (vendor-prefixes, for example)
*   Integrate some of Norm's tests (maybe?)
