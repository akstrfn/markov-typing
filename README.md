# Markov typing tutor

An experimental typing tutor in alpha stage. A shot at implementing typing
tutor that generates random characters in such a way that maximizes the chance
that a mistake will be made. This means that the weakest character combinations
are practiced the most. The driving idea for this project is the fact that
different character combinations make our fingers more prone to subsequent
mistakes hence practicing every possible pair of characters should improve
typing speed and decrease mistakes.

The underlying model is based on Markov chain where probabilities are the ratio
of correct and wrong hits of character pairs. In addition to this there is
a pressure applied from the speed of typing meaning that even if the character
is typed correctly but the speed was lower that average it would still be
counted as wrong. To further increase (or decrease) the pressure, the average
typing speed is smoothened with exponential moving average, meaning that only
very recent averages will be counted (the better you get the harder it
becomes). This also means that if you have a bad streak the pressure is
decreased faster.

Very basic interface is implemented in curses with some bugs around symbols and
utf8 is not yet supported.

# Building

You will probably need GCC-8 to compile the program. Clang should also work but
due to lack of filesystem support even in Clang-6 you will need GCC-8 support.

Build requirements:

- Curses library
- CMake
- make or ninja

Build steps:

```
cmake -H. -Bbuild -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

If you want to try building with compilers that support C++17 but don't have
filesystem try the following:

```
cmake -H. -Bbuild -DCMAKE_BUILD_TYPE=Release -DUSE_BOOST_FS=TRUE
cmake --build build
```

# Running the program

After building you can run the executable ``./build/typing``. After practicing
the Markov matrix is stored in JSON format either in
``$XDG_DATA_HOME/DeliberateTyping`` or ``$HOME/.local/share/DeliberateTyping``
and is loaded from the same location on new start of the program.

You can get command line options with ``./build/typing --help`` with the
following options:

```
Options:
  -h,--help                   Print this help message and exit
  --lowercase                 Use lowercase.
  --uppercase                 Use uppercase.
  --symbols                   Use symbols.
  --num                       Use numbers.
  --custom TEXT               Provide custom set of letters
```

# Warning

This is mostly an experiment to see how such tutor would work and anything can
change and nothing can be considered stable.

# Feedback

If you play with it and have some feedback/ideas please let me know.
