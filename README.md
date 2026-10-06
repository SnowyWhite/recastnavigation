# librecastnavigation - <SUMMARY>

This is a `build2` package repository for [`librecastnavigation`](https://<UPSTREAM-URL>),
a <SUMMARY-OF-FUNCTIONALITY>.

This file contains setup instructions and other details that are more
appropriate for development rather than consumption. If you want to use
`librecastnavigation` in your `build2`-based project, then instead see the accompanying
[`PACKAGE-README.md`](<PACKAGE>/PACKAGE-README.md) file.

The development setup for `librecastnavigation` uses the standard `bdep`-based workflow.
For example:

```
git clone .../librecastnavigation.git
cd librecastnavigation

bdep init -C @gcc cc config.cxx=g++
bdep update
bdep test
```
