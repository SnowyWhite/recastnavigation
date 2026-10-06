# recastnavigation - Navigation mesh construction and pathfinding C++ library

This is a `build2` package repository for
[`recastnavigation`](https://github.com/recastnavigation/recastnavigation),
the Recast navigation mesh construction toolset and the Detour pathfinding
and spatial reasoning runtime (with DetourCrowd, DetourTileCache and the
debug utilities).

This file contains setup instructions and other details that are more
appropriate for development rather than consumption. If you want to use
`recastnavigation` in your `build2`-based project, then instead see the
accompanying
[`librecastnavigation/PACKAGE-README.md`](librecastnavigation/PACKAGE-README.md)
file.

The upstream source code is the `upstream/` git submodule; the package
overlays it with symlinks (the headers of every component into
`include/recastnavigation/`, each component's `Source/` directory into
`src/`). On Windows, clone with symlinks enabled (`git clone -c
core.symlinks=true ...`, which needs Developer Mode) to work on the package;
consuming it through `bpkg` needs neither.

The development setup for `recastnavigation` uses the standard `bdep`-based
workflow. For example:

```
git clone --recurse-submodules .../recastnavigation.git
cd recastnavigation

bdep init -C @gcc cc config.cxx=g++
bdep update
bdep test
```

The package follows upstream's `main` branch rather than a release, so its
version is a pre-release snapshot (`1.7.0-a.0.z`, the development cycle
after 1.6.0); it changes with every commit. To move to a newer upstream
commit, check it out in `upstream/` and commit the submodule change.
