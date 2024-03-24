## moepadding 4.0.0

If your code files are cluttered, cramped, and cannot breathe, then you need moepadding.

moepadding is a source code formatting tool that adds (or removes) empty lines between top-scoped code objects such as functions, methods, and class interfaces, creating uniform padding in the file. It works by using a simple curly brace language parser.

**Usage**

```
mpad <filename> [<padding>] [-w]
```

`padding` can be any nonnegative number and represents the number of empty lines you want between entities.

By default, moepadding will not overwrite the input file and will write its results to standard output.
The `-w` flag will force overwrite of the input file.

**Configuration file (.mpad)**

If `padding` is omitted, moepadding will search for a `.mpad` configuration file located in the closest parent directory of the input file.

The config file should contain a single padding value.

If `padding` is specified on the command-line, and there is also a valid config file, then the command-line argument will override the config file value.
