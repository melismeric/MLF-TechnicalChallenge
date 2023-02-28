# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/melismeric/Coding/projects/MLF/build/_deps/googletest-src"
  "/Users/melismeric/Coding/projects/MLF/build/_deps/googletest-build"
  "/Users/melismeric/Coding/projects/MLF/build/_deps/googletest-subbuild/googletest-populate-prefix"
  "/Users/melismeric/Coding/projects/MLF/build/_deps/googletest-subbuild/googletest-populate-prefix/tmp"
  "/Users/melismeric/Coding/projects/MLF/build/_deps/googletest-subbuild/googletest-populate-prefix/src/googletest-populate-stamp"
  "/Users/melismeric/Coding/projects/MLF/build/_deps/googletest-subbuild/googletest-populate-prefix/src"
  "/Users/melismeric/Coding/projects/MLF/build/_deps/googletest-subbuild/googletest-populate-prefix/src/googletest-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/melismeric/Coding/projects/MLF/build/_deps/googletest-subbuild/googletest-populate-prefix/src/googletest-populate-stamp/${subDir}")
endforeach()
