# H4c — Engine canonical include path reorganization

## Goal
Physically move all engine headers and sources from root-level subdirs (`Manager/`, `Renderer/`, `Physic/`, `Tools/`, `Lights/`, `Resource/`, `Handler/`, `Network/`) into `engine/include/snake3d/` (public headers) and `engine/src/` (implementation), then rewrite all include sites to use angle-bracket `<snake3d/...>` style, so the engine presents a clean public API surface ready for external consumer apps.

## Context

**Current state:** Engine sources live at repo root under 8 top-level directories (`Handler/`, `Lights/`, `Manager/`, `Network/`, `Physic/`, `Renderer/`, `Resource/`, `Tools/`). Game layer lives in `examples/snake3/src/`. The engine CMake target (`snake3d_engine`) exposes `${CMAKE_CURRENT_SOURCE_DIR}` (repo root) as its PUBLIC include path. H4b complete — all engine headers 100% free of `using namespace`. H4c unblocked per ENGINE_BOUNDARY.md.

**Scope numbers (grep-verified):**
- ENGINE_SOURCES list: 273 entries (149 `.h` + 124 `.cpp`) across 27 directories
- `stdafx.h` at repo root: included by 7 engine headers with relative `../stdafx.h` paths
- Include sites needing rewrite: ~282 cross-subdir engine-internal, ~232 game-side engine references, ~42 Tests relative engine references — ~556 total quoted includes; same-directory within-subdir quoted includes in engine `.cpp` files (~244) become `<snake3d/...>` too, bringing the practical total to ~800 sites
- `RainDropRenderer` and `BoltRenderer` referenced only in build-cache object files; no source files on disk — not part of the move

**Related tickets:**
- H4b: done (unblocks H4c) — ENGINE_BOUNDARY.md
- H5: feature plugin registry — benefits from clean include surface here
- Target layout: ENGINE_BOUNDARY.md §"Target layout (post-H3)"

**Branch:** `feature/physics`

## Plan

- [ ] **Step 0 — Safety checkpoint**
  - Create `git tag pre-H4c` on HEAD of `feature/physics`
  - Confirm build green: `cmake -B build && cmake --build build --target snake3d_engine snake3d_game Tests && cd build && ctest --output-on-failure`

- [ ] **Step 1 — Decide and document include style**
  - Decision (recommended): angle-bracket `<snake3d/X/Y.h>` for all engine public headers
  - Within engine `.cpp` files (now in `engine/src/`), engine header includes also switch to `<snake3d/...>` (no relative `../` paths)
  - `stdafx.h` at repo root moves to `engine/include/stdafx.h` (NOT under `snake3d/` subdirectory); 7 engine headers switch from `"../stdafx.h"` to `<stdafx.h>`. Game's `main.cpp` finds it via `engine/include` exposed transitively through `snake3d_engine`.

- [ ] **Step 2 — Create empty target directory tree** (mkdir 27 subdirs in `engine/include/snake3d/` mirroring current layout; mirror in `engine/src/`)

- [ ] **Step 3 — Batch 1: move `stdafx.h`**

- [x] **Step 4 — Batch 2: move `Lights/` (10 files)**

- [x] **Step 5 — Batch 3: move `Tools/` (30 files)**

- [x] **Step 6 — Batch 4: move `Resource/` (14 files)**

- [x] **Step 7 — Batch 5: move `Network/` (12 files)**

- [x] **Step 8 — Batch 6: move `Physic/` (19 files, 3 subdirs: Algorithms, Dynamics, Jump)**

- [x] **Step 9 — Batch 7: move `Handler/` (19 files incl. Debug subdir)**

- [ ] **Step 10 — Batch 8: move `Manager/` (28 files)**

- [ ] **Step 11 — Batch 9: move `Renderer/` (141 files, 3 sub-batches: top+Scene, Material subtree, Model subtree)**

- [ ] **Step 12 — CMakeLists.txt update**: prefix ENGINE_SOURCES paths, change `target_include_directories(snake3d_engine PUBLIC engine/include)`, add game-layer include path

- [ ] **Step 13 — Rewrite game-side includes (~232 sites)** in `examples/snake3/src/`

- [ ] **Step 14 — Rewrite Tests includes (~42 sites)**

- [ ] **Step 15 — Update CI workflow** (verify no path hardcoding)

- [ ] **Step 16 — Final validation**: clean Debug + Release build, ctest, confirm root engine dirs gone, update ENGINE_BOUNDARY.md status

- [ ] **Step 17 — Cleanup**: `git rm -r` empty root engine dirs, prep PR

## Decisions log

- **2026-05-28** — `stdafx.h` → `engine/include/stdafx.h` (not under `snake3d/` subpath). Engine + game both find it as `<stdafx.h>` via `snake3d_engine` PUBLIC include dir.
- **2026-05-28** — Renderer layout preserves full physical tree: `engine/include/snake3d/Renderer/Opengl/Material/...` etc. Avoids extra include rewrites. ENGINE_BOUNDARY.md target description (`snake3d/Material/` siblings) is aspirational; literal mirror is the practical landing.
- **2026-05-28** — `snake3d_game` PUBLIC include path on `examples/snake3/src` — Tests (linking snake3d_game) inherits transitively. Matches current behavior so Tests' game references work unchanged.
- **2026-05-28** — Sequencing: per-batch incremental CMakeLists update + build checkpoint after each subdir batch. Surgical, errors caught early.

## Files touched

## Open questions

- **`stdafx.h` disposition**: move to `engine/include/stdafx.h` (preferred — accessible as `<stdafx.h>` for all consumers) vs. inline its 4 GL includes into the 7 headers that need them (cleaner long-term but more churn, arguably H4d scope). Plan assumes the move.
- **Renderer path in `engine/include/snake3d/`**: ENGINE_BOUNDARY.md target lists `Material/`/`Scene/` as siblings of `Renderer/`, but actual tree has `Renderer/Opengl/Material/` + `Renderer/Opengl/Scene/`. Recommend keeping full physical tree under `engine/include/snake3d/Renderer/Opengl/...` to preserve include compatibility.
- **Tests location**: stays at repo root (per ENGINE_BOUNDARY.md target). Plan assumes no move.
- **`snake3d_game`'s PUBLIC include**: adding `examples/snake3/src` as PUBLIC means Tests (which links snake3d_game) inherits it. Intentional? Or PRIVATE preferred?
- **Batch sequencing**: (a) update CMakeLists incrementally after each batch — most surgical, catches errors early. (b) all `git mv` first, then big CMakeLists + include rewrite pass. (a) recommended per spec's safeguard.
