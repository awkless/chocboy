<!--
SPDX-FileCopyrightText: 2025 Jason Pena <jasonpena@awkless.com>
SPDX-License-Identifier: MIT
-->

# ChocBoy

A chocolatey sweet GameBoy emulator.

## Installation

You will need the following pieces of software:

- CMake [>= 3.25].
- C++17 compliant compiler.
- Conan [>= 2.0.0].

> __WARNING__
>
> If you barely installed Conan, then you need to generate a Conan
> profile with the following command: `conan profile detect --force`.

Perform the following steps if you are using a single configuration build
system:

```
# conan install . --build missing
# cmake --preset conan-release
# cmake --build --preset conan-release
# cmake --install build/Release
```

Preform the following steps if you are using a multi configuration build system
(mainly Windows MSVC):

```
# conan install . --build missing
# cmake --preset conan-default
# cmake --build --preset conan-release
# cmake --install build\Release
```

For more detailed installation instructions, please view the provided
[installation file][install].

## Usage

TODO

## Contribution

This project is open to the following forms of contribution:

- Reporting of bugs.
- Suggestions for features or improvements.
- Improvements or fixes to build system
- Improvements or fixes to source code.
- Improvements or fixes to tests.
- Improvements or fixes to documentation.
- Improvements or fixes to CI/CD workflow.

Please follow the provided [contribution guidelines][contrib] of the project
when creating your pull requests. Keep in mind, these are guidelines, not
steadfast rules, so please use your best judgememnt at all times!

## Licensing

The ChocBoy project abides by the MIT license for the distribution of its source
code and documentation. The project also uses the CC0-1.0 license to place files
in the public domain, which are considered to be to small, or to generic to
place copyright over.

The project uses the [REUSE 3.3 specification][reuse-3.3] to make it easier to
determine who owns the copyright and licensing of any given file in the
codebase. The [Developer Certificate of Origin version 1.1][linux-dco] is also
used to ensure that any contributions made have the right to be merged into the
project, and can be distributed with the project under its main licenses.

[install]: INSTALL.md
[contrib]: CONTRIBUTING.md
[linux-dco]: https://developercertificate.org/
[reuse-3.3]: https://reuse.software/spec-3.3/
