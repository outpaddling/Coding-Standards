
### Instructions for package maintainers

Below are manual build instructions to guide package developers in
building most C-based projects under Github accounts
[outpaddling](https://github.com/outpaddling/) and
[auerlab](https://github.com/auerlab/).
These instructions are not intended to be understood or employed by
end-users.

1. Install all build and runtime dependencies via the package manager
2. Fetch and unpack the distfile
3. Run "make install" in the dist directory

Nothing more should be required.  If any patches are necessary for your
platform, please submit them to the project so that they might be
incorporated into the next release.

The Makefile respects all standard make/environment variables such as CC,
CFLAGS, PREFIX, etc.  It is assumed that the package manager provides
these as environment variables or make variables, though some variables have
reasonable defaults (e.g. CC="cc"). The default install prefix is ../local,
so that projects and their dependencies can be cave-man installed to
the same private prefix from sibling directories for testing.

The package manager must set PREFIX in order to install to the correct
location for your package manager.
All files are installed under `${DESTDIR}${PREFIX}`.  DESTDIR is empty by
default, but is provided by most package managers to perform a
staged install.  See the GNU coding standards for more information on
DESTDIR.

Dependencies required for the build must be found under either
`${PREFIX}` or `${LOCALBASE}`, i.e. headers under ${PREFIX}/include
or ${LOCALBASE}/include, libraries under ${PREFIX}/lib or
${LOCALBASE}/lib.  ${LOCALBASE} defaults to `${PREFIX}`,
but the two can be separated for testing or for multiple installations
with different build options.

View the Makefile for full details.
