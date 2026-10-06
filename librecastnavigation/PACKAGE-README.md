# librecastnavigation - Navigation mesh construction and pathfinding C++ library

This is a `build2` package for the
[`recastnavigation`](https://github.com/recastnavigation/recastnavigation)
C++ library. It provides Recast, which builds navigation meshes from level
geometry (voxelization, region partitioning, contour tracing, polygon mesh
and detail mesh generation), and Detour, the runtime that pathfinds and
steers on them: the navigation mesh and its queries, crowd simulation
(DetourCrowd), tile caching with dynamic obstacles (DetourTileCache), and
the debug drawing and dump utilities (DebugUtils).

The package tracks the upstream `main` branch after the 1.6.0 release (see
the `upstream/` submodule for the exact commit), hence the pre-release
version.


## Usage

To start using `librecastnavigation` in your project, add the following
`depends` value to your `manifest`, adjusting the version constraint as
appropriate:

depends: librecastnavigation ^1.7.0-a.0.1
```

Then import the library in your `buildfile`:

```
import libs = librecastnavigation%lib{recastnavigation}
```

All five upstream components are in this one library, which is how
upstream's own `recastnavigation.pc` presents them.

The headers are installed into `include/recastnavigation/` and can be
included either with that prefix or, as upstream and its own sources do,
without it:

```c++
#include <recastnavigation/Recast.h>
#include <DetourNavMesh.h>
```

Upstream's CMake build generates a `recastnavigation/version.h`; this
package does not provide it.
