
### Instructions for package maintainers

Below are manual build instructions to guide package developers in
building most C-based projects under Github accounts
[outpaddling](https://github.com/outpaddling/) and
[auerlab](https://github.com/auerlab/).
These instructions are not intended to be understood or employed by
end-users.

After installing all build dependencies via the package manager:

1. Fetch and unpack the distfile
2. Run "make depend" to update Makefile.depend
3. Run "make install"

The Makefile respects all standard make/environment variables such as CC,
CFLAGS, PREFIX, etc.  It is assumed that the package manager provides
these as environment variables or make variables, though some have
reasonable defaults (e.g. CC="cc"). The default install prefix is ../local.

The package manager must set PREFIX in order to install to the correct
location.
All files are installed under `${DESTDIR}${PREFIX}`.  DESTDIR is empty by
default and should be provided by the package manager to perform a
staged install.

Dependencies required for the build should be found under either
`${PREFIX}` or `${LOCALBASE}` (which defaults to `${PREFIX}`).

View the Makefile for full details.
