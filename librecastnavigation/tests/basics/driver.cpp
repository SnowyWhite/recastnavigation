// Smoke test: every component's headers can be included, both with the
// recastnavigation/ prefix and without it (as upstream includes them), the
// library links, and a navigation mesh can be made from a heightfield.
//
#include <recastnavigation/Recast.h>
#include <recastnavigation/DetourNavMesh.h>
#include <recastnavigation/DetourNavMeshBuilder.h>
#include <recastnavigation/DetourNavMeshQuery.h>
#include <recastnavigation/DetourCrowd.h>
#include <recastnavigation/DetourTileCache.h>
#include <recastnavigation/DebugDraw.h>

#include <DetourStatus.h>

#undef NDEBUG
#include <cassert>

#ifdef DT_VIRTUAL_QUERYFILTER
// With config.librecastnavigation.virtual_query_filter the define comes
// with the library: a filter derived from dtQueryFilter is what the query
// asks, here one that refuses every polygon.
//
struct refuse_all: dtQueryFilter
{
  mutable int asked = 0;

  bool
  passFilter (const dtPolyRef, const dtMeshTile*, const dtPoly*) const override
  {
    ++asked;
    return false;
  }
};
#endif

int main ()
{
  // One triangle for a floor, voxelized, filtered and turned into the
  // polygon mesh Detour is built from.
  //
  const float verts[] = {0, 0, 0,  0, 0, 10,  10, 0, 10,  10, 0, 0};
  const int   tris[]  = {0, 1, 2,  0, 2, 3};
  const float bmin[]  = {-1, -1, -1};
  const float bmax[]  = {11, 1, 11};

  rcContext ctx;
  const float cs (0.5f), ch (0.2f);

  int w, h;
  rcCalcGridSize (bmin, bmax, cs, &w, &h);

  rcHeightfield* hf (rcAllocHeightfield ());
  assert (hf != nullptr);
  assert (rcCreateHeightfield (&ctx, *hf, w, h, bmin, bmax, cs, ch));

  unsigned char areas[2] = {0, 0};
  rcMarkWalkableTriangles (&ctx, 45.0f, verts, 4, tris, 2, areas);
  assert (rcRasterizeTriangles (&ctx, verts, 4, tris, areas, 2, *hf, 1));

  rcCompactHeightfield* chf (rcAllocCompactHeightfield ());
  assert (rcBuildCompactHeightfield (&ctx, 10, 2, *hf, *chf));
  assert (rcErodeWalkableArea (&ctx, 1, *chf));
  assert (rcBuildDistanceField (&ctx, *chf));
  assert (rcBuildRegions (&ctx, *chf, 0, 8, 20));

  rcContourSet* cset (rcAllocContourSet ());
  assert (rcBuildContours (&ctx, *chf, 1.3f, 12, *cset));

  rcPolyMesh* pmesh (rcAllocPolyMesh ());
  assert (rcBuildPolyMesh (&ctx, *cset, 6, *pmesh));
  assert (pmesh->npolys > 0);

  rcPolyMeshDetail* dmesh (rcAllocPolyMeshDetail ());
  assert (rcBuildPolyMeshDetail (&ctx, *pmesh, *chf, 6, 1, *dmesh));

  for (int i (0); i != pmesh->npolys; ++i)
    pmesh->flags[i] = 1;

  dtNavMeshCreateParams params {};
  params.verts            = pmesh->verts;
  params.vertCount        = pmesh->nverts;
  params.polys            = pmesh->polys;
  params.polyAreas        = pmesh->areas;
  params.polyFlags        = pmesh->flags;
  params.polyCount        = pmesh->npolys;
  params.nvp              = pmesh->nvp;
  params.detailMeshes     = dmesh->meshes;
  params.detailVerts      = dmesh->verts;
  params.detailVertsCount = dmesh->nverts;
  params.detailTris       = dmesh->tris;
  params.detailTriCount   = dmesh->ntris;
  params.walkableHeight   = 2.0f;
  params.walkableRadius   = 0.5f;
  params.walkableClimb    = 0.4f;
  rcVcopy (params.bmin, pmesh->bmin);
  rcVcopy (params.bmax, pmesh->bmax);
  params.cs               = cs;
  params.ch               = ch;
  params.buildBvTree      = true;

  unsigned char* data (nullptr);
  int size (0);
  assert (dtCreateNavMeshData (&params, &data, &size));

  dtNavMesh* nm (dtAllocNavMesh ());
  assert (dtStatusSucceed (nm->init (data, size, DT_TILE_FREE_DATA)));

  // A query finds the floor's polygon under a point on it.
  //
  dtNavMeshQuery* q (dtAllocNavMeshQuery ());
  assert (dtStatusSucceed (q->init (nm, 64)));

  const float center[] = {5, 0, 5};
  const float extents[] = {1, 2, 1};
  dtQueryFilter filter;
  dtPolyRef ref (0);
  float nearest[3];
  assert (dtStatusSucceed (q->findNearestPoly (center, extents, &filter, &ref, nearest)));
  assert (ref != 0);

#ifdef DT_VIRTUAL_QUERYFILTER
  {
    refuse_all f;
    dtPolyRef r (0);
    assert (dtStatusSucceed (q->findNearestPoly (center, extents, &f, &r, nearest)));
    assert (r == 0 && f.asked > 0);
  }
#endif

  // The crowd and the tile cache, so that their code links as well.
  //
  dtCrowd* crowd (dtAllocCrowd ());
  assert (crowd->init (8, 0.5f, nm));
  dtFreeCrowd (crowd);

  dtTileCache* tc (dtAllocTileCache ());
  assert (tc != nullptr);
  dtFreeTileCache (tc);

  dtFreeNavMeshQuery (q);
  dtFreeNavMesh (nm);
  rcFreePolyMeshDetail (dmesh);
  rcFreePolyMesh (pmesh);
  rcFreeContourSet (cset);
  rcFreeCompactHeightfield (chf);
  rcFreeHeightField (hf);
}
