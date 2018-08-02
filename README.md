# Markov typing

[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](https://github.com/akstrfn/markov-typing/blob/master/LICENCE)

[![typing](https://asciinema.org/a/jWkjpdBxkjWrfb3hBFDBB8z4z.png)](https://asciinema.org/a/jWkjpdBxkjWrfb3hBFDBB8z4z/?autoplay=1&speed=2&theme=tango&size=big)

An experimental typing tutor in alpha stage. A shot at implementing typing
tutor that generates random characters in a way that maximizes a chance for a
mistake to be made. In effect, this means that the weakest character
combinations have the highest chance to be in the practice sentence. The
driving idea for this project is the fact that different character combinations
make our fingers more prone to subsequent mistakes hence practicing every
possible pair of characters should improve typing speed and decrease mistakes.

The underlying model is based on Markov chain where probabilities are the ratio
of correct and wrong hits of character pairs. In addition to this, there is a
pressure applied from the speed of typing meaning that even if the character is
typed correctly but the speed was lower that average it would still be counted
as wrong. To further increase (or decrease) the pressure, the average typing
speed is smoothened with exponential moving average, meaning that only fairly
recent averages will be counted (the better you get the harder it becomes).
This also means that if you have a bad streak the pressure is decreased faster.

Very basic interface is implemented in curses with some bugs around symbols. 
UTF-8 is not (yet) implemented.

# Building

You will probably need GCC 8 to compile the program. Clang 6 will also work but
it has to use GCC 8 filesystem library (libstdc++). libc++ will have filesystem
support from clang 7.

Build requirements:

- GCC 8.1
- Curses library
- CMake
- make or ninja

Build steps:

```
cmake -H. -Bbuild -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

If you want to try building with compilers that support C++17 but don't have
the filesystem library try the following:

```
cmake -H. -Bbuild -DCMAKE_BUILD_TYPE=Release -DUSE_BOOST_FS=TRUE
cmake --build build
```

# Running the program

After building you can run the executable ``./build/typing``. The matrix is
stored in JSON format either in ``$XDG_DATA_HOME/MarkovTyping`` or
``$HOME/.local/share/MarkovTyping`` and is loaded from the same location so the
progress is saved. Since this is experimental so far, the same save file might
not work between different commits.

You can get command line options with ``./build/typing --help`` with the
following options:

```
Options:
  -h,--help                   Print this help message and exit
  --lowercase                 Use lowercase characters.
  --uppercase                 Use uppercase characters.
  --symbols                   Use symbols.
  --num                       Use numbers.
  --custom TEXT               Provide custom set of characters.
```

# Warning

This is mostly an experiment to see how such tutor would work and anything can
change and nothing can be considered stable.

# Feedback

If you play with it and have some feedback/ideas please let me know.
