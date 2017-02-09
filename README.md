stringsearch
===========

This was a small utility I wrote to help find text strings in old software dumps and possibly identify unused/hidden debugging functions. It was very useful, but I also recently realized there's already a GNU utility ([strings](https://linux.die.net/man/1/strings)) that does the same thing, with more features. Oh well. Putting it up on github for posterity; I don't think it will actually be useful to anyone. I've never had any issues with it, but it might be buggy. I've only compiled/tested it in Linux, not if/how well it works in Windows.

Output:
-------
Prints start offset of a found string in hexadecimal on the left, followed by the found string

Usage:
------
Pipe in contents from stdin:
> cat test.bin | strsrch

Or specify a filename as an argument:
> strsrch test.bin

Specify minimum string length with -l (default is 10):
> strsrch -l 20 test.bin

Specify string buffer size with -b (default is 4096):
> strsrch -b 1048576 test.bin
