<!--
SPDX-FileCopyrightText: 2025 Jason Pena <jasonpena@awkless.com>
SPDX-License-Identifier: MIT
-->

# Installing ChocBoy

## Prerequisites

You will need the following pieces of software to fully build and install the
ChocBoy project:

- C++17 compliant compiler.
- CMake [>= 3.23]
- Conan [>= 2.0.0]

These pieces of software are optional, but useful to have:

- Git [>= 2.20.0]
- PGP tool.

## Obtaining Valid Sources

You can directly clone the `main` branch, since it is always kept in semi-stable
state:

```
# git clone --depth 1 https://github.com/awkless/chocboy.git
```

You can also directly clone a tagged release version of the ChocBoy project
to produce and install a more stable build:

```
# git clone --depth 1 -b [tagged_release] https://github.com/awkless/chocboy.git
```

> __NOTE__
>
> ChocBoy uses Semantic Versioning, thus substitute the version
> number of the tagged release for [tagged\_release] in the command above.

You can also just visit <https://github.com/awkless/chocboy> and download
a copy of the ChocBoy project from your favorite browser.

### Validating Sources

If you are a paranoid individual (like me), then you can verify the
authenticity of tagged releases and commits through my GPG signature.
Obtain my public key <jasonpena@awkless.com> from
<https://keys.openpgp.org/about/usage>.

Once you have my public key, verify the fingerprint by making sure it
matches the following sequence of digits (spaces were added for readability):

```
8A14 E0DF A45D F309 72BA  BA29 A39C 4170 CBB2 3146
```

If, for whatever reason, the fingerprint of the public key does not match, then
immediately send an email to <jasonpena@awkless.com> with your findings. __DO
NOT SUBMIT A BUG REPORT__. After you have sent your findings get rid of the
public key from your keyring, because the key may have been compromised.

If the fingerprint is fine, then you can begin verifying your copy of the
ChocBoy project with `git-verify-commit` and `git-verify-tag`. For example:

```
# git verify-tag v1.0.0
```

or

```
# git verify-commit 12f45c
```

You can also list the signatures with `git-log`:

```
# git log --show-signature
```

If, for whatever reason, `git-verify-tag` or `git-verify-commit` shows that any
signature is bad, then send an email of your findings to
<jasonpena@awkless.com>. __DO NOT SUBMIT A BUG REPORT__. Also get rid of your
copy of ChocBoy, because it may be compromised.

## Installation Steps

Change directory to the top-level of your copy of the ChocBoy project
before proceeding to the following installation steps.

To begin, you need to setup all dependencies of the ChocBoy project
through Conan. If you just installed Conan, then you need to generate a profile
before getting the dependencies like so:

```
# conan profile detect --force
```

After you have setup your Conan profile, collect the dependencies that ChocBoy
needs:

```
# conan install . --build missing
```

Perform the following steps if you are using a single configuration build
system (mainly Linux):

```
# cmake --preset conan-release
# cmake --build --preset conan-release
# cmake --install build/Release
```

Preform the following steps if you are using a multi configuration build system
(mainly Windows MSVC):

```
# cmake --preset conan-default
# cmake --build --preset conan-release
# cmake --install build\Release
```

If you are using Linux, then you may need to prepend the `sudo` command for root
privileges when using `cmake --install`.

## Troubleshooting Installation Errors

Before sending a bug report about installation issues, please try the following
pieces of advice. If you already tried everything listed here, then go ahead and
issue a bug report.

### Conan Complains About C++ Standard Mismatch

Conan [>= 2.0.0] autodetects the C++ standard that your system uses. ChocBoy
requires at minimum a C++17 compliant compiler. Your compiler suite may
default to a C++ version lower than C++17, which causes Conan to set the
`compiler.cppstd` variable to that lower C++ version.

You can fix this issue by editing your Conan profile's `cppstd` to be set to
`cppstd=17`. If you do not want to modify your Conan profile, or you have no
idea where to find it, then you can pass `-s compiler.cppstd=17` when
invoking Conan during the installation. For example:

```
# conan install . --build=missing -s compiler.cppstd=17
```

### Conan Complains About Missing Packages

Certain dependencies that ChocBoy requires are little bit finicky (SDL3 can be a
real pain). Conan may not have the required recipies to handle the requirements
of a given dependency, and will try to use your system's package manager (if
available) to obtain them. By default Conan will only check to see if you have
the software that ChocBoy's dependencies need, and if you do not have the
required software Conan will error, and tell you that you need to install a
specific package.

You can give Conan permission to just install the required packages itself by
passing `-c tools.system.package_manager:mode=install`. For example:

```
# conan install . --build missing -c tools.system.package_manager:mode=install
```

If you are using a Linux system, then you may need to give Conan sudo privileges
by also passing `-c tools.system.package_manager:sudo=True`. For example:

```
# conan install . --build missing -c tools.system.package_manager:mode=install -c tools.system.package_manager:sudo=True
```

If nothing works, then you need to send a bug report. This may be an issue with
Conan itself, but give me a heads up just in case it is an issue with the
project before submitting anything to the people who maintain Conan.

### The `libiconv/1.17` Package Fails With Weird Compiler Error About `mbrtowc`

If you are encountering an a build failure similar to what is described by this
[bug report][issue-27413], then you will need to add this little snippet to your
default conan profile:

```
[conf]
tools.build:cflags=["-std=gnu17"]
```

This error is caused by GCC 15 using the C23 standard by default, which causes
this issue due to the `mbrtowc` prototype being declared without parameters,
while its function definition has four parameters. This was fine in older
standards of C, because the ancient K&R syntax allowed this. However, C23 fully
removes K&R syntax, which causes the `mbrtowc` prototype to be interpreted as a
function with no arguments despite its function definition having four.

The `libiconv/1.18` package at Conan Center fixes this upstream issue, but
`libgettext/0.22` still uses `libiconv/1.17`, and has yet to be updated in the
Conan Center. Plus `libgettext/0.22` cannot be bypassed, because
`pulseaudio/17.0` needs it, and `pulseaudio/17.0` is needed by `sdl/3.2.16`.
So the SDL library is causing pain once again, but in a indirect fashion for
funsies.

For now, this little hack works. Hopefully, it will be fixed soon so I can
remove this section.

[issue-27413]: https://github.com/conan-io/conan-center-index/issues/27413
