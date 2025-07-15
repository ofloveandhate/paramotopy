# Overview

Paramotopy is command line software for repeated solving parameterized polynomial systems using [Bertini](https://bertini.nd.edu).

Bertini 1 offers a built-in parameter homotopy.  But it solves only at one parameter point at a time.  Paramotopy essentially implements the loop you'd have to write, if you wanted to use Bertini's parameter homotopy many times.  Plus, Paramotopy includes tools for resolving path failures, etc, be re-solving at just those parameter points.  Nifty!

# Using

The main website for paramotopy is at [paramotopy.com](https://paramotopy.com).  There are some examples, and some guidance.  The manual pdf is located in this repo, in the `documentation/source` folder.  I know, it's not source.  Feel free to help me out in providing a better build system for it than manual tex-ing.

# Quick guide to building

As of July 2025 Paramotopy now uses CMake, and thus requires version 1.7 of Bertini 1, which now also uses Cmake.

## 1. Install dependencies

Paramotopy requires several libraries to successfully install.  These libraries are:

* mpfr
* gmp
* bertini-serial (>=1.7)
* boost (>= 1.53)
* mpi, any implementation, probably openmpi or mpich

**important note:** Bertini 1 must have been built and installed from source.  Due to some changes in Bertini 1.7 (install location, build system, and header files location), Paramotopy now requires Bertini 1.7 or later.

## 2. Download Paramotopy source

Use git to clone the repo.

## 3. Standard CMake build and install

Move to directory.

`mkdir build && cd build`

`cmake ../`

`make`

`make install`, possibly with `sudo`


