# Overview

Paramotopy is command line software for repeated solving parameterized polynomial systems using [Bertini](https://bertini.nd.edu).


# Using

The main website for paramotopy is at [paramotopy.com](https://paramotopy.com).  There are some examples, and some guidance.  The manual is located in this repo, in the `documentation` folder.

# Building

Paramotopy requires several libraries to successfully install.  These libraries are:

* mpfr
* gmp
* bertini-serial
* boost (>= 1.53)
* mpich3

**important note:** Bertini must have been built and installed from source

If these libraries have been installed in standard locations, e.g. `/usr/local/include` for header files and `/usr/local/lib` for library files, then the command

    ./configure

will work fine.

If you have the libraries installed in non-standard locations, you may need to add each location of the headers to the `CPPFLAGS` variable and the location of the libraries to the `LDFLAGS` variable.  For example, if you have mpfr, gmp, and bertini installed in `/usr/local/mpfr`, `/usr/local/gmp`, and `/usr/local/bertini`, respectively, then you would run


    ./configure CPPFLAGS="-I/usr/local/mpfr/include -I/usr/local/gmp/include -I/usr/local/bertini/include" LDFLAGS="-L/usr/local/mpfr/lib -L/usr/local/gmp/lib -L/usr/local/bertini/lib"

In this case, you probably also will also need to add the location of the Bertini library to `LD_RUN_PATH` and `LD_LIBRARY_PATH`.

If you are wanting to install with a particular compiler, or your compiler is not in the `PATH` location, you will need to run

    ./configure CXX="/path/to/g++" MPICXX="/path/to/mpic++"


If you have non-standard locations for libraries and a specific compiler to use, you can run, for instance


    ./configure CXX="/path/to/g++" MPICXX="/path/to/mpic++" CPPFLAGS="-I/usr/local/mpfr/include -I/usr/local/gmp/include -I/usr/local/bertini/include" LDFLAGS="-L/usr/local/mpfr/lib -L/usr/local/gmp/lib -L/usr/local/bertini/lib"


Additional compiler flags can be added to `CPPFLAGS`, if desired.


After `./configure [OPTIONS]` has successfully ran, then you can install paramotopy with

    make; make install

## notes to self (silviana), for running `make dist`

Silviana builds on MacOS, using Homebrew.  

* use this to make the gnu tools installed by Homebrew supercede those provided by Mac:

```
PATH="$(brew --prefix)/opt/gnu-tar/libexec/gnubin:$PATH"
```

* use this to make `tar` not fail when running `make dist`:

```
export TAR_OPTIONS = --owner=0 --group=0 --numeric-owner
```

note: an improvement would be to move to CMake, but I have no time for that right now.
