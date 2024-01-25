If your code files are cluttered, cramped, and cannot breathe, then you need moepadding.

moepadding is a simple source code formatting tool that adds (or removes) empty lines between top-scoped code objects such as functions, methods, and class interfaces, creating uniform padding in the file. It works by using a very simple curly brace language parser.

**Usage**

mpad < filename > < padding > [-w]

`padding` can be any nonnegative number and represents the number of empty lines you want between entities.

By default, moepadding will not overwrite the input file and will write out to ***moeout***.
The ***-w*** flag will force overwrite of the input file.
