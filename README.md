# Coding-Standards

One of my aims as a bioinformatician is to raise the average quality of
code in the field.  Bioinformatics, and scientific computing in general,
has no shortage of brilliant minds, but a severe shortage of experienced
and disciplined developers.  As a result, it abounds in code that implements
ingenious algorithms with low quality code, documentation, and build systems.

This makes the programs less accessible and less reliable, impeding critical
research as researcher struggle to install, learn, and use them.  A
core goal of all my projects is software that is easy to install on any
POSIX platform, well-documented, easy to use, reliable, and performant.

This repository describes the coding standards for all projects under
Github accounts "outpaddling" and "auerlab".  All contributions to these
projects should follow these guidelines to the extent possible.  Patches
and merge requests that fall short will need to be cleaned up before they
are incorporated.

## Modularity

1. Any function that might be useful to another program should be placed
in a library such as
[libxtend](https://github.com/outpaddling/libxtend) or
[biolibc](https://github.com/outpaddling/biolibc), rather than the application.  For
a typical C project, about 2/3 to 3/4 of all the code I write ends up in
libraries.

2. Absolutely no global variables unless there is no alternative.  In my
decades of programming, the only place I've encountered where global variables
are unavoidable is interrupt service routines (ISRs).  They have no place
in application programming.  There are other constructs available in modern
languages that make modular programming convenient and efficient.

3. Write cohesive code, i.e. keep related code together.
Each function should serve one purpose and
each purpose should be served by one function.  Initialize loop variables
in or immediately before the loop, not pages earlier.

## Performance

1. Use the most efficient available algorithm for the purpose.  This
may mean using a simple O(N^2) algorithm rather than a more complex
O(N*log N) algorithms where N is guaranteed to be small.

2. Use a fully compiled language for any potentially long-running code.
Interpreted languages are at least 2 orders of magnitude slower for the
same algorithm.  Just-in-time compiled languages like Java and Numba do
much better, but still fall far short of C, C++, and Fortran while also
using far more memory.

https://acadix.biz/RCUG/HTML/ch15s04.html#compiled-interpreted

3. Don't use loops (including hidden loops like string, vector, or matrix
copying if the language supports them).  There is often an approach that
uses scalar operations instead.

4. If you must use loops:

    1. Move as much code as possible out of the loop
    2. Minimize the number of iterations

## Resource use

1. Always try to minimize memory use.  Using more memory rarely speeds up
a program and more often slows it down.  Programs that use less memory have
a better cache hit ratio and hence far fewer memory wait cycles.  Occasionally
it is advantageous to bring large amounts of data into memory, but work hard
to avoid this strategy.  As an example, adding two matrices stored in files
and saving the result to another file does not require the use of arrays.
(Think about it.)  Using arrays for this task only slows down the program
and limits the size of arrays it can process.

## Portability

All code should run on any POSIX platform and any CPU architecture.

1. Write endian-independent code (e.g. use bit operations and masks instead
of bit fields: TODO: sample code).

2. Avoid *nixisms: Look for a portable implementation rather than lace
the code with #ifdefs for specific platforms.

3. If there is a good reason to write platform-dependent code (e.g. to
double program speed by using AVX instructions), keep a portable
implementation alongside the x86-optimized code.

4. Before writing platform-specific code, first see of the compiler's optimizer
can optimize portable code.  Modern compilers can generate SSE, AVX, Altivec,
etc. instructions automatically using portable compiler flags like
```-march-native```.

## Simplicity

1. The simplest solution is the easiest to maintain and usually the fastest.

2. Minimize complexity, not lines of code.  Writing cryptic, overly clever
code makes matters worse, not better.

## Documentation

1. Every library function has a man page.  These can be generated from block
comments by the c2man and script2man scripts.  See libxtend and biolibc
source files for examples.

2. Every application has a man page.  If the application is too complicated
to describe in the man page format:

    1. Consider whether the application is too big and should be broken
    into separate programs.
    
    2. If it really must be so complex, write a simple man page providing
    an overview and in it provide a link to the full manual in a more
    appropriate format.

3. Use highly descriptive variable and constant names to reduce the need
for comments.

4. Comments should never state WHAT a piece of code is doing.  This insults
the reader's intelligence and wastes space.  Explain WHY the code is doing
what it does, i.e. how it serves the purpose and why you chose to do it this
way.

```
    buff_size *= 2;    // Double buff_size (a useless comment)
    buff_size *= 2;    // Avoid having to realloc too frequently (useful)
```

## Build system

Write a simple build system that's easy to follow and portable.

1. Do not bundle dependencies.  The build system should build your program
and nothing else.  This is not only a good idea, it's policy for many
mainstream package managers:

https://fedoraproject.org/wiki/Bundled_Libraries?rd=Packaging:Bundled_Libraries

https://www.debian.org/doc/debian-policy/ch-source.html#s-embeddedfiles

https://wiki.gentoo.org/wiki/Why_not_bundle_dependencies

https://docs.freebsd.org/en/books/porters-handbook/special/#bundled-libs

Instead, write software that works with the mainstream versions of all
dependencies, which are installed separately.

2. A simple Makefile that respects standard build variables such as CC,
CXX, CFLAGS, CXXFLAGS, LDFLAGS, etc. is enough for most projects.
Configure tools like GNU autoconf and cmake may seem to make things easier,
but they are cans of worms.  Most of them end up becoming extremely complex
in the attempt to make them work on multiple platforms, and when they don't,
it's a nightmare for the end user.  They'd have an easier time with a simple
Makefile.

3. Make the build-system package-friendly.  If it's trivial to create a
Debian package, a FreeBSD port, a MacPort, a pkgsrc packages, etc., then
you can stay out of the software deployment business and focus on
development.  This can be achieved simply with a clean Makefile that
respects standard build variables such as CC, CFLAGS, LDFLAGS, etc., which
most package managers already provide as environment variables or via the
make command.
