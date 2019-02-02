# Markov typing

[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](https://github.com/akstrfn/markov-typing/blob/master/LICENCE)

Click on the image below for a video demo.

[![typing](https://img.youtube.com/vi/L8mEgqyT8FQ/maxresdefault.jpg)](https://www.youtube.com/watch?v=L8mEgqyT8FQ)

An experimental typing tutor in alpha stage. Implements typing tutor that
generates random characters in a way that maximizes a chance for a mistake to
be made. In effect, this means that the weakest character combinations have the
highest chance to be in the practice sentence. The driving idea for this
project is the fact that different character combinations make our fingers more
prone to subsequent mistakes hence practicing every possible pair of characters
should improve typing speed and decrease mistakes.

The underlying model is based on Markov chain where probabilities are the ratio
of correct and wrong hits of character pairs. In addition to this, there is a
pressure applied from the speed of typing meaning that even if the character is
typed correctly but the speed was lower that average it would still be counted
as wrong. To further increase (or decrease) the pressure, the average typing
speed is smoothened with exponential moving average, meaning that only fairly
recent averages will be counted (the better you get the harder it becomes).
This also means that if you have a bad streak the pressure is decreased faster.

# Packages

Currently only the development package is available for Arch Linux in
[AUR](https://aur.archlinux.org/packages/markov-typing-git/).

After installing you can start the executable from the command line:

```
markov-typing
```

# Building

You will probably need GCC 8 to compile the program. Clang 6 will also work but
it has to use GCC 8 filesystem library (libstdc++). libc++ has filesystem
support starting from Clang 7.

Build requirements:

- GCC 8.1
- Qt5
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

After building you can run the executable ``./build/markov-typing``. The matrix is
stored in JSON format either in ``$XDG_DATA_HOME/MarkovTyping`` or
``$HOME/.local/share/MarkovTyping`` and is loaded from the same location so the
progress is saved. Since this is experimental so far, the same save file might
not work between different commits.

# Warning

This is mostly an experiment to see how such tutor would work and anything can
change and nothing can be considered stable.

# Feedback

If you play with it and have some feedback/ideas please let me know.
