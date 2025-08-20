# Coding-Standards

This repository describes the coding standards for projects under
Github accounts "outpaddling" and "auerlab".  All contributions to these
projects must follow these guidelines to the maximum extent possible.
Patches and merge requests that fall short will need to be cleaned up before
they are incorporated.

## Core principle

Software development is inherently expensive.  It takes years of training and
practice to become a proficient programmer.  Very few people ever really
master the art of software engineering.

The high cost of software development makes it imperative that the
code we write be made as reusable as possible.
Every time we write new code, we should attempt to make it as widely
available as possible, so that the time and effort invested in its
creation need not be duplicated.   This is the core principle behind
the specifics that follow.

To achieve this primary goal, we need to make our software reliable and
portable
across as many hardware platforms as possible.  Portability is a function
of language selection, CPU efficiency, memory efficiency, etc.
For basic system code (e.g. a string or math function), this could mean
serving any platform from the smallest microcontroller to the
largest HPC cluster.  Other software will have a narrower range of
feasible platforms, but should still be as broad as possible.

## Software in science

One of my aims as a researcher with prior training and experience
as a software and systems engineer, is to raise the average quality of
code in the field.  Scientific computing
has no shortage of brilliant minds, but a severe shortage of experienced
and disciplined developers.  As a result, it abounds in code that implements
ingenious algorithms with low quality code, documentation, and build systems.

This makes the programs less accessible, less efficient, and less reliable,
impeding critical
research as researchers struggle to install, learn, and use them.  A
core goal of all my projects is software that is easy to install on any
POSIX platform, well-documented, easy to use, reliable, and highly CPU
and memory efficient.

## Priorities

General priorities for most of my software are as follows:

1. Reliability/robustness
2. Clear and concise documentation
3. Performance
4. Everything else

## Rule number 1

Never let quality slip.  Start with a simple, minimalist skeleton of a
program and make the code and build system impeccable.

Then add only useful features, with great care to maintain or improve
the quality of every aspect of the project.

## Modularity

1.  Always try to write code that can be used anywhere rather than just
    in this project.

    Any function that might be useful to another program should be placed
    in a library such as
    [libxtend](https://github.com/outpaddling/libxtend) or
    [biolibc](https://github.com/outpaddling/biolibc), rather than reside in
    an application.  For a typical C project, about 2/3 to 3/4 of all the
    code I develop ends up in libraries, so that it can easily be utilized
    by any other application.
    
    We can employ top-down programming so that the needs of applications drive
    the API design, while at the same time designing the code for more
    general use.
    To achieve this, first develop each new function as part of the
    application it will serve, but design it to be independent of the program
    so that it can be easily factored out to a library after it has been
    fully tested.
    
    For example, the libxtend strupper() function was developed as part of
    fastq-trim, which must use case-insensitive comparison for DNA/RNA
    sequences.  Fastq-trim also uses a
    string comparison method that tolerates a few differences due to
    DNA sequencing read errors, hence strcasecmp() will not work.  The
    adapter sequence to be compared to all reads is converted to upper case
    using strupper() ahead of time so that the LC->UC conversion is done only
    once, rather than during every comparison.  Once fastq-trim was
    fully tested, strupper() was moved to libxtend, since it is potentially
    useful to many other programs.

2. Absolutely no global variables unless there is no alternative.  In my
decades of programming, the only places I've encountered where global
variables are unavoidable are interrupt service routines (ISRs) and other event
handlers, i.e. subprograms that are called asynchronously rather than via
a normal interface.  They have no place in application programming.
There are other constructs available in modern languages that make modular
programming convenient and efficient.  The notion that using global
variables will greatly speed up a program by reducing function call overhead
is a myth.  Well-designed functions have low overhead that will not be
noticeable in most cases.

3. Write cohesive code, i.e. keep related code together. Each subprogram
should serve exactly one purpose and each purpose should be served by
exactly one subprogram.  Initialize loop variables in or immediately
before the loop, not where it is out-of-sight such as in the variable
definition at the start of a larger block.  The reader should not have
to jump around the program to find all the information needed to understand a
code block.

## Hardware Portability

MAKE NO ASSUMPTIONS ABOUT THE HARDWARE THAT WILL RUN YOUR CODE.

Some code designed for big data computations could also be useful on
a 16-bit microcontroller and vice-versa.

1.  Choose data types according to the needs of the problem.  E.g. In C,
    an integer variable that may contain values beyond the range of a 32-bit
    integer type should be defined as uint64_t or int64_t.  The size
    of "int", "long", and "size_t" vary across CPUs and
    may be smaller than 64 bits on some systems.
    Some people complain about the variable size of these data types in C,
    but this feature allows you to write portable code that will
    perform optimally on all platforms.  Use int when it doesn't matter if a
    variable is 16 bits or 32, and long when it doesn't matter if it's 32
    or 64.  This will help the compiler avoid multiple precision integer
    arithmetic, which takes twice as long as a single integer instruction.
    When size matters due to range requirements, use int64_t, int32_t, etc.

2.  Write endian-independent code

    Use bit operations and masks instead of bit fields, so that the
    code behaves the same way on big and little endian CPUs.  See
    https://github.com/outpaddling/Coding-Standards/blob/main/endian.c.
    
    If saving numeric data in binary format, choose an endianness for
    the file contents and use functions like htole32()
    [host to little-endian 32-bit] in endian.h as needed.  These
    functions are not POSIX standardized as of 2023, but they are common and
    mostly portable.
    
    ```
    #include <endian.h>
    
    uint32_t    saved_bits, int_value;
    
    ...
    
    // Make sure value is written in little-endian format
    saved_bits = htole32(int_value);
    fwrite(&saved_bits, sizeof(saved_bits), 1, stream);
    ```

3. If there is a good reason to write hardware-specific code (e.g. to
increase program speed by using AVX instructions), keep a portable
implementation alongside the x86-optimized code.  A sub-optimal solution
for Power, ARM, and RISC-V is better than nothing.  Users should never
have to wait for us to "add support for processor X".

4. Before writing platform-specific code for speed, first see if the
compiler's optimizer
can optimize portable code.  Modern compilers can generate SSE, AVX, Altivec,
etc. instructions automatically using portable compiler flags like
```-march-native```.  In some cases this may produce similar or even
better results than hand-written non-portable code.  Maintaining multiple
non-portable versions of code is rarely worth the marginal performance
gain.

2.  OS Portability

Level the playing field.  Allow users to choose their OS based on
its technical merits, such as reliability and performance, rather than
force them to use an OS supported by your software.

POSIX is powerful.  Using strictly POSIX code, we can meet virtually all
needs.  No need to hinder users by using unnecessary extensions from an
OS they don't use.

All code should run on any POSIX platform (and any CPU architecture).

1. Avoid *nixisms

    Look for portable implementations rather than lace
    the code with #ifdefs for specific platforms.  The first solution you
    come up with when coding on BSD, Linux, or Mac may not be portable.  Don't
    get attached to it.  If it doesn't work on other platforms, look for a more
    portable solution before adding #ifdefs to your code.  Then you'll have a
    permanent solution that will likely work on platforms you haven't even
    tested yet, rather than more work down the road to support other new
    platforms.
    
    Examples:
    
    1. Assuming "sh" is "bash" is a common mistake that will not work
    on Debian or BSD, which use Almquist shell derivatives rather than
    bash.
    
    2. Parsing files in /proc is not portable.  /proc is deprecated on
    FreeBSD and the files use different formats on different systems.
    
    3. Don't require Linux-only features such as cgroups or BSD-only
    features such as FreeBSD jails.
    Supporting them is fine, but make them optional.
    
    4. If no standardized API exists for what you're doing, create one
    and publish it as a separate, installable library (such as
    libxtend, biolibc, libusb, etc).  Doing this once will
    prevent countless applications from using redundant #ifdefs
    in the future.
    
2.  Code should build using STOCK tools on any currently supported *nix
    distribution.
    Users should not be required to install a newer compiler in order to build
    the code.  This is a common problem for RHEL-based platforms, which use
    older tools than the bleeding-edge distributions that many developers use.
    It may be tempting to use the latest C++ features, but it's not worth
    the headaches it will cause if users need to run your code on RHEL.

## Interoperability and Interchangeability

Write software accessible to the widest possible audience.  If there are
relevant standards in the field being served, adhere to them.  Graphical
programs should run under any desktop environment.  GUI systems
such as Qt make this as simple as possible.  Programs that only
work well under a certain desktop environment force others to waste time
duplicating their functionality.  Scientific programs should input and
output files compatible with related software.  The more choices scientists
have for software tools, the fewer delays their research will encounter.

## Robustness

Check for every possible error condition.  No exceptions (except
perhaps output to the standard output and standard error).  This means
checking the status of every I/O operation, memory allocation, etc.
If a function returns a status value, the code should check it and take
appropriate action if it indicates a problem.

## Testing

1. Test new code frequently.  Do not write more than a few dozen lines of
code without fully testing the changes.  This way if there is a bug,
it will be easy to find.  If you write or change hundreds of lines of new code
before testing, you'll waste many hours trying to find new bugs.

2. You can facilitate this by investing
a very small amount of time preparing a test script and a small sample
input that triggers all program features and can be processed in seconds
or minutes.

3.  Test on multiple platforms, using different compilers/interpreters,
    even if you don't intend to fully support them all.  This will help you
    identify bugs before end-users do.
    
    Each compiler will issue warnings about potential problems that other
    compilers overlook.  Many real bugs are discovered by simply building
    with another compiler or newer version of the same compiler.
    
    Different compilers, libraries, and operating systems organize memory
    differently.  The fact that a program seems to work fine with one
    setup does not mean it is free of bugs.  A stray pointer or subscript
    might have no visible impact, because the memory it corrupts will
    be updated will not be referenced while it contains corrupted data.
    On another system, such corruption could cause a segmentation fault,
    or worse, invalid output.

## RUSC (Reduced Unnecessary Software Complexity)

Simplicity is the ultimate sophistication.

Apply the wisdom of the RISC (Reduced Instruction Set Computer) hardware
design ideal to software development.

1. The simplest solution is always the easiest to maintain and usually the
fastest, or at least close to it.  Simpler code will have fewer bugs, which
means end-users also waste less of their time and your time discussing
problems.

2. Minimize COMPLEXITY, not lines of code.  Writing cryptic, overly clever
code to make it more compact or 1% faster is just showing off,
and makes maintenance harder, not easier.  Readability is as important as
any other trait.

```
// Technically faster, but cryptic and probably has no effect on run time
// Use tricks like this only if they provide a meaningful speedup
buff_size <<= 1;

// Intent is clear here
buff_size *= 2;
```

3. Don't overoptimize at the expense of simplicity and readability.
Man-hours usually cost more than CPU-hours (except rare applications that use
massive amounts of CPU time), so minimizing development and maintenance
time is usually worth more than a 10% reduction in run-time.  Do the math
before you complicate the code with cleverness that benefits only your ego.
If users run a program once a week and it takes 20 minutes, reducing run
time to 15 minutes is inconsequential.  If it uses 2,000 core-hours on an
HPC cluster, reducing this to 1,500 is worth significant effort.

## Minimize dependencies, especially big ones

Utilizing existing libraries and programs rather than reinventing the
wheel is generally a smart move.  But not always.

1. Making your code depend on a huge library (e.g. boost) in order to use
one or two functions from it may incur more cost than benefit.  It increases
build/install time and increases that possibility of breakage down the
road due to API changes and bugs in the library.

2. Make sure that any dependencies you add are high quality.  If the only
existing wheel is a square limestone slab, it's time to reinvent it.

3. If there are multiple implementations of a dependency (e.g.
BLAS/OpenBLAS), stick to standards (avoid using extensions supported by
only one or a few of them) so that they will be interchangeable
for your code.  This will increase the odds of a successful deployment
for all of your users, since any one implementation could have critical
bugs at a given time.

## Readability

1. All code is consistently indented with blank lines separating code blocks.

2. All variable names must be descriptive and unambiguous.  Do not use
abbreviations that cloud the meaning of the variable.  Highly descriptive
variable names make the code self-documenting, reducing the need for
comments.

```
    int     count;          // Ambiguous
    int     record_count;   // Still ambiguous, what's in the record?
    int     client_count;   // Better
```

3. Comments should be brief and should add information that the code
does not already reveal.

```
    int     client_count;   // Integer variable to count clients (useless)
    int     client_count;   // Total clients processed so far (helpful)
```

4. Comments should never state WHAT a piece of code is doing.  This only
insults the reader's intelligence and doesn't aid understanding.  Explain
WHY the code is doing what it is doing, i.e. how it serves the purpose and
why you chose to do it this way.

```
    buff_size *= 2;     // Double buff_size (insults the reader's intelligence)
    buff_size *= 2;     // Double to avoid too many realloc()s (helpful)
```

3. cleverness * wisdom = constant

Making the meaning of your code obvious is far more important than showing
off your intricate knowledge of programming.  For example, there's no
practical benefit to omitting ```== 0``` or similar in a loop or if
condition.  It saves a few keystrokes, but does not help performance.
It just makes the reader work harder to understand the code.

```
char    *p, *name;

for (p = name; *p; ++p)           // Just showing off
for (p = name; *p != 0; ++p)      // Not clear that we're scanning chars
for (p = name; *p != '\0'; ++p)   // Clear
```

## Performance

1. Use the most efficient available algorithm *for the purpose*.  This
may mean using a simple O(N^2) algorithm rather than a more complex
O(N*log N) algorithms where N is guaranteed to be small.

2. Use a fully compiled language where performance is important.
Interpreted languages are orders of magnitude slower for the
same algorithm.  Just-in-time compiled languages like Java and Numba do
much better, but still fall far short of C, C++, and Fortran while also
using far more memory.

    C is a simple language that anyone can master, and using it, you can write
    very fast code without having to be overly clever.

    https://acadix.biz/RCUG/HTML/ch15s04.html#compiled-interpreted

3. Don't use loops (including hidden loops like string, vector, or matrix
copying if the language supports them) if they can be avoided.  There is
often an approach that uses scalar operations instead.

4. If you must use loops:

    1. Move as much code as possible out of the loop
    2. Minimize the number of iterations

## Memory use

*Always* try to minimize memory use.

1. Using more memory rarely speeds up
    a program and more often slows it down.  Programs that use less memory
    have a better cache hit ratio, and hence far better average memory response
    times.  Cache access is an order of magnitude faster than DRAM access.
    Suppose cache access time is 3 nanoseconds and DRAM access takes 30
    nanoseconds.  If data are found in the cache 90% of the time, then our
    average memory access time is 3 ns * .9 + 30 ns * .1 = 5.7 ns.  A slight
    drop in the hit ratio to 80% give us 3 ns * .8 + 30 ns * .2 = 8.4 ns.
    That's a 47% increase in memory access time, which is often a
    significant portion of total run time.
    
    One of the most common mistakes among programmers is using arrays
    where they are not needed, simply because it is intuitive.  Many
    programmers, upon seeing the words "list of ..." in the specification,
    immediately think of an array, vector, or similar data structure,
    and code their program to inhale all the data into memory before
    processing it.  A *streaming* data processing design is far better
    choice when it is feasible.  I.e. read one input value into a
    *scalar* variable, process it, output the results.
    
    Occasionally
    it is advantageous to bring large amounts of data into memory (e.g.
    sorting, hash tables), but usually not.  As an example, adding two
    matrices stored in files
    and saving the result to another file does not require the use of arrays.
    This can easily be done using a streaming design.
    (Think about it.)  Using arrays for this task only slows down the program,
    and limits the size of the matrices it can process efficiently to the
    computer's RAM capacity (swap is not efficient here).  Also, the
    virtual memory size (RAM + swap) is an absolute limit on the matrix size.
    If we use a streaming design instead,
    the only limit on matrix size is the amount of disk space,
    generally many times larger than RAM and swap.

2. Higher memory use increases the likelihood that the program will not
be able to run at all.  It's foolish to assume that every computer
running our program has as much RAM as our development machine, or that
all RAM is available on today's multitasking systems.  Some computers
have far less RAM in total, and some will be running other programs
that occupy a large portion of it.  If our program can't run because
there is not enough memory available, this might mean that CPUs sit
idle while a more memory-efficient program could be utilizing them.

### Apple Unified Memory

Apple's unified memory reduces the cache miss penalty, resulting in a
smaller performance benefit for reducing memory use.  However, large
amounts of unified memory are very expensive, so unified memory systems
tend to have less total RAM than DIMM-based computers.  Also, it is
physically impossible for unified memory systems to match the RAM
capacity of external memory, due to physical size limits.  This makes
limiting memory use even more important for unified memory Macs.

Lastly, if we minimize memory use to the point where the cache hit
ratio is very high, then unified memory has minimal benefit, since
memory beyond the cache is seldom accessed.  We can often make any computer
perform as well as one with unified memory by minimizing memory use.

## Documentation

1. Every library function has a man page.  These can be generated from block
comments by the 
[auto-c2man](https://github.com/outpaddling/auto-dev/tree/main/Scripts)
and 
[auto-script2man](https://github.com/outpaddling/auto-dev/tree/main/Scripts)
scripts.  See 
[libxtend](https://github.com/outpaddling/libxtend) and
[biolibc](https://github.com/outpaddling/biolibc)
source files for examples.

2. Every executable has a man page.  If the application is too complicated
to describe in the man page format:

    1. Consider whether the application is too big and should be broken
    into separate, more cohesive programs.
    
    2. If it really must be so complex, write a simple man page providing
    an overview of its purpose, and in it tell the reader where to find the
    full manual in a more appropriate format.  Don't make users waste time
    searching for documentation.

Some people might argue that "man" is a bad format for documentation.
This is a bogus generalization.  Man pages are certainly not suitable
for complex programs.  As an example, try finding the information you
need in "man bash" or "man tcsh".  However, man pages are very convenient
for simple programs and library functions (which are always simple if
you're writing cohesive code).  If a program is complex, it should have
a man page anyway, but the man page can simply point the user to detailed
documentation in a more navigable format such as HTML.

## Build system

Write a simple build system that's easy to follow and portable.

1.  A simple Makefile that respects standard build variables such as CC,
    CXX, CFLAGS, CXXFLAGS, LDFLAGS, INSTALL, etc. is sufficient for most projects
    and easier to debug than more sophisticated build systems.  See
    https://github.com/outpaddling/Coding-Standards/blob/main/makevars.md
    for detailed info on standard/common variables.
    
    Respecting these variables means two things:
    
    Use them.  E.g. use CC, not CCOMPILER, in your make rules.
    
    Allow them to be overridden by environment variables or make arguments.
    This can be achieved by assigning with ?= rather than = or :=.  ?= sets
    a variable only if it was not already set in the environment or by
    a make argument.  If a value is actually required, it can be added
    using +=.  Suppose we have a program that must be compiled with the
    C macro POSIX defined:
    
    ```
    # Does not respect environment or make arguments and compiles with
    # gcc -march=native -O4.  This will not work where gcc is not installed.
    # If it does, it results in non-portable executables that will not run
    # on a lesser CPU because the compiler generates machine code for the
    # CPU where it is compiled.
    CC      = gcc
    CFLAGS  = -march=native -O4 -DPOSIX
    
    prog:   prog.c
	    ${CC} ${CFLAGS} prog.c -o prog
    ```
    
    ```
    # Respects environment and make arguments
    CC      ?= gcc                  # Use gcc only if CC is not specified
    CFLAGS  ?= -march=native -O4    # Use only if CFLAGS not specified
    CFLAGS  += -DPOSIX              # Append to CFLAGS in all cases
    
    prog:   prog.c
	    ${CC} ${CFLAGS} prog.c -o prog
    ```
    
    Now suppose we compile as follows:
    
    ```
    make CC=cc CFLAGS="-O2 -pipe -Wall"
    
    ```
    or
    ```
    env CC=cc CFLAGS="-O2 -pipe -Wall" make
    ```
    With the first makefile, we may see
    ```
    gcc -march=native -O4 prog.c -o prog
    ```
    Some make programs allow make arguments like CC=cc to override assignments
    like CC=gcc in the makefile, but others do not.
    
    With the second makefile, we will always see
    ```
    cc -O2 -pipe -Wall prog.c -o prog
    ```

    Configure tools like GNU autoconf and cmake aim to make things easier,
    but they are cans of worms.  Most of them end up becoming extremely
    complex in the attempt to make them work in various environments,
    and almost invariably fail to achieve this goal.
    When they don't work (which is often)
    it's a nightmare for the end user to figure out why the configure
    tool aborted or generated a non-functional Makefile, Ninja-file, etc.
    They'd have a much easier time debugging a simple Makefile.  This
    falls under the famous David Wheeler quip, sometimes called the
    "Fundamental theorom of software engineering":
    
    "We can solve any problem by adding another level of indirection".
    
    Corrolary: "...except the problem of too many levels of indirection."
    
    A few of the problems often caused by configure tools:
    
    -   They often alter the build depending on what other software
	is discovered at configure-time.  This leads to inconsistent
	installations, causing confusion and failures for end-users
	and package manager maintainers.
    -   They often detect the CPU-type and add non-portable optimizations.
	Hence, building on a high-end machine may produce binaries
	that don't work on lesser machines.
    -   Some actually attempt to download and install dependencies,
	which is the job of a package manager, not the build system
	of a particular software project.  This actually makes it very
	difficult to create packages for package managers such as
	Debian packages, FreeBSD ports, MacPorts, etc., which would
	free users from having to build the software at all.

2. Use portable commands.  E.g., there is generally no reason to set
CC to "gcc", since "cc" and "gcc" are the same on GNU-based systems such
as Linux and "cc" is "clang" on FreeBSD and MacOS.  Setting CC to "cc"
will likely use the compiler that the user wants in most cases.  If they
really want "gcc" instead of the native compiler, they can easily specify
that by running "make CC=gcc".

3. Do not bundle dependencies.  The build system should build this project
and nothing else.  This is not only a good idea, it's policy for many
mainstream package managers:

    https://fedoraproject.org/wiki/Bundled_Libraries?rd=Packaging:Bundled_Libraries
    
    https://www.debian.org/doc/debian-policy/ch-source.html#s-embeddedfiles
    
    https://wiki.gentoo.org/wiki/Why_not_bundle_dependencies
    
    https://docs.freebsd.org/en/books/porters-handbook/special/#bundled-libs
    
    Instead, write software that works with the mainstream versions of all
    dependencies, which are installed separately.  It's easier to maintain
    compatibility with mainstream libraries than to maintain a bundled
    fork of each of them.

4. Make the build-system package-friendly.  If it's trivial to create a
Debian package, a FreeBSD port, a MacPort, a pkgsrc packages, etc., then
we'll more likely get free help from packager maintainers, so we can stay
out of the software management business and focus on development.
Users won't have any trouble deploying the software, and we won't have
to waste time helping them with deployment issues.
This can be achieved very simply with a clean Makefile that
respects standard build variables such as CC, CFLAGS, LDFLAGS, etc., which
most package managers already provide as environment variables or via the
make command.

## Containers

Containers are powerful tools and extremely useful when we need to isolate
an environment for some reason. For example, they can be used very
effectively to house numerous virtual web servers on the same hardware in
complete isolation from each other, providing a high level of security
and hardware utilization with
low overhead.  They can also be used for testing software in a pristine
environment without the need to maintaining separate hardware for testing.
They largely serve the same purpose as a virtual machine or emulator, but
with much lower overhead.  The main limitation is that they can only run
an operating system or application highly compatible with the host.

Unfortunately, it seems that every great innovation quickly becomes a solution
looking for problems and people flock to it like bugs to a lamp, employing it
in inappropriate situations in an attempt to look sophisticated.

Before you get caught up in a fad that will cost you and your users a lot
of time and effort, think about the real cost and benefits.  
Well-designed application software rarely benefits from being
containerized.  If you have a clean build system, maintain compatibility with
mainstream prerequisite software, and take care to avoid conflicts with other
applications, there will be no need to maintain a container and make users
jump through the additional hoops of using it.  They should be able to just
install it with a simple command and run it.

In the case of containers, they are often used to cover up inadequacies in
software design rather than correct them.  E.g., they can be used as a
mechanism to bundle outdated, buggy libraries without causing conflicts with
other versions, since they are isolated from each other.  Sweeping problems
into a container is a prime example of "sweeping problems under the rug".
It's a poor alternative to correcting them with proper code maintenance, and
only enables more problems to accumulate.  It is clearly not a sustainable
approach to software development and deployment.

Use containers where they have a legitimate benefit, but never to isolate
quality issues from the host system.

## OOP in C

Object oriented design is a good practice and important for keeping complex
programs maintainable.  Unfortunately, languages with object-oriented
support tend to be either overly complex, or interpreted which leads to
poor performance.

Fortunately, it is possible to easily implement a basic object-oriented design
in any language with structures and typedefs, such as C.  C is a simple
language that any scientist can master and provides the best general
performance of any programming language if used properly.

A web search for OOP in C will reveal lots of advice on this subject.

1. Treat C structures like C++ or Java classes.  This is easy to achieve
following a few simple practices:

    1. A C function should only directly access the members of one
    structure type.  Use accessors and mutators for all other structure
    types.  This makes it a member of that class and no others.
    
    2. Prefix function names with a tag representing the class name.
    This is an effective substitute for name spaces, e.g.
    ```myclass::function(args)``` in C++ can be
    ```myclass_function(args)``` in C.
    
    3. Realize that the object name before the '.' in C++ or Java is
    really just a pointer argument, referenced as ```this->``` inside the
    function.  Hence, ```object.function(x,y);``` in C++ or Java is basically
    equivalent to ```function(&object,x,y);``` in C.
    OOP is about high-level program organization, not the arrangement
    of language tokens.

2. Organize classes into separate source files as you would in C++
or Java.  I.e., create one header file per structure type and one or more
source files to contain the function (method) definitions.

3. Create accessor and mutator functions or macros for all structure
members.  That said, we must keep in mind that C has no "private" modifier
to protect them from direct access by non-member functions, so it's up to
the programmer to respect class boundaries.  The
[auto-gen-get-set](https://github.com/outpaddling/auto-dev/tree/main/Scripts)
script can do most of the work here.

## Creeping Feature Syndrome

Creeping feature syndrome (A.K.A. "feature creep")
is a situation where good intentions ruin
software over time.  Features that aren't all that helpful but seem like a
good idea gradually add to the complexity of the code and eventually erode
maintainability and reliability.

We all like to be creative and make end-users happy, but there are negative
consequences to consider as well.
Before adding a cool new feature that you or one of your users feels will add
convenience, assess the real value vs the cost of writing and more
importantly, maintaining the additional code.

Follow the example of Dennis Ritchie and friends.  They designed C and Unix
by stripping away redundant and low-value features from earlier languages
and operating systems and then committed to keeping them out.  For example,
C follows the rule that if a feature can be implemented reasonably well by
a library function, then it should not be part of the language.  Hence
```
if ( strcmp(string1,string2) == 0 )
```
instead of
```
if ( string1 == string2 )
```
The latter is a little prettier, but has no objective benefit.  It makes the
compiler more complex for no good reason.

If your software already provides a simple way to accomplish a task,
then don't add more features just to make the same task a little more
intuitive.

Also think through the full complexity of what the feature needs to
accomplish.  For example, is the string comparison above case sensitive?
How will you provide for both case sensitive and case insensitive
comparison using built-in operators?  You may realize that it's better
to just leave this to library functions once you consider how complex
the problem really is.
