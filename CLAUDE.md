# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build

```bash
# Configure (from repo root)
cmake -B build -DCMAKE_BUILD_TYPE=Debug

# Build game - release
cmake --build cmake-build-release --target Snake3

# Build game - debug
cmake --build cmake-build-debug --target Snake3

# Build and run tests
cmake --build build --target Tests
cd build && ctest --output-on-failure

# Run a single test (Catch2)
./build/Tests "[tag-or-test-name]"
```

Key CMake flags:
- `-DUSE_ENET=ON/OFF` — enable/disable ENet networking (default ON)
- Debug builds automatically define `IS_DEBUG`
- The `copy_assets` target copies `Assets/` into the build directory; it runs as a dependency of `Snake3`

Dependencies: GLFW3, OpenGL/GLEW, glm, assimp, OpenAL/ALut, freetype2, nlohmann_json, ENet, Catch2 (fetched automatically via FetchContent).

## Engine / Game boundary

Snake3 targets a small Limitless-light style **multi-game engine** + game split. The engine layer is reusable; Snake3 is the first concrete game on top.

See [ENGINE_BOUNDARY.md](ENGINE_BOUNDARY.md) for the full classification of which files are engine vs game. New code should be placed accordingly even before the physical file move (planned in phase H3).

## Architecture

### Entry point and app loop

`main.cpp` creates a GLFW window (1920×1080, OpenGL 3.3 Compat) and a global `Camera` and `App`. The `App` class (`App.h/App.cpp`) owns the top-level managers and drives the game loop via `App::run()`.

`App` contains:
- `ResourceManager` — async asset loading (textures, shaders, models, animations) with a thread pool and mutex-protected caches
- `RenderManager` — orchestrates the rendering pipeline
- `KeyboardManager` — input state
- `EatManager` — item collection mechanics
- Scene pointers (`MainScene`, `PreloaderScene`, …) and a `SceneState` enum (`LOADING` / `RUNNING`)
- Network components (`NetManager`, `NetClient`, `NetServer`) — only compiled when `USE_ENET` is on

### Scene system

All gameplay is organized into scenes under `Scenes/`. `MainScene` (≈2000 lines) is the core game scene; it composes sub-scenes (`PlayerScene`, `RemoteSnakeScene`, `CoinScene`, `BarriersScene`, `WeatherScene`, `TorchScene`) and integrates collision, networking, HUD, and the menu. Scenes are built on `Scene` (`Renderer/Opengl/Scene/Scene.h`), which maintains a Node3D hierarchy and delegates rendering to `SceneRenderer`.

### Rendering pipeline

`Renderer/Opengl/` contains the full OpenGL renderer:
- `BaseRenderer` — abstract base with a `MODE` enum (`standard`, `shadowMap`, `reflection`, `bloom`)
- `Node3DRenderer` / `Node2DRenderer` — concrete 3D/2D renderers
- Specialized renderers: `BloomRenderer`, `DepthMapRenderer` (shadow maps), `PlanarReflectionRenderer`, `RainDropRenderer`, `BoltRenderer`
- **Material system** (`Material/`): `StandardMaterial` (PBR), `ShaderMaterial`, 2D materials (labels, fonts), uniform helpers (particle, fade, timer)
- **Model/mesh types** (`Model/`): procedural meshes (`PlaneMesh`, `BoxMesh`, `CylinderMesh`, …), `AnimationArrayMesh`, game-specific nodes (`SnakeMeshNode3D`, `CoinMeshNode3D`, …), 2D nodes (`LabelNode2D`, `ImageNode2D`), and GPU primitives (`VAO`, `VBO`, `EBO`) in `Model/Utils/`

### Networking

Networking uses ENet and lives in `Network/`. `NetManager` is a singleton (Pimpl idiom) that exposes a server/client API. Messages are defined in `NetProtocol`/`NetMessages`, dispatched via `NetDispatcher`, and synchronized with `NetClock`. Game-specific logic is in `Network/Game/`: `NetGameSnapshot` serializes game state, `SnakeSnapshotApplier` applies remote snake updates to the local scene.

### Physics

`Physic/` contains an AABB/SAT collision system. `CollisionSystem3D` is the physics world; `CollisionDetector` runs checks between shapes (`BoxShape`, `SphereShape`, `CapsuleShape`, `CylinderShape`). Collision shapes for rendered objects live in `Renderer/Opengl/Model/Collision/`.

### Handlers

`Handler/` contains stateful event processors keyed to `BaseHandler`. Game handlers: `SnakeMoveHandler` (movement logic), `EatLocationHandler` (pickup logic), `RadarHandler`. Debug handlers under `Handler/Debug/` (position/rotation/scale gizmos, light manipulation) are only active in debug builds.

### Data and resources

- `ItemsDto/` — lightweight data structs (`BaseItem`, `AnimItem`, `Transform`, `Vector3i`) and component interfaces (`Visibility`, `Named`, `Tagged`)
- `Resource/` — asset loaders (`ObjModelLoader` via assimp, `TextureLoader` via stb_image, `ShaderLoader`, `AnimLoader`) unified under `ResourceLoader`
- `Lights/` — `DirectionalLight`, `PointLight`, `SpotLight` all extend `Light`; `OrientableLight` adds orientation
- `Tools/` — `Timer`, `CameraOrbit`, `ContextState` (OpenGL state management), `Layers`, `BuildSettings`

### Tests

Game code is compiled as the static library `snake3d_lib` so tests can link against it. Test sources are in `Tests/` and use Catch2 v3 (`main.cpp` for game-logic tests, `NetProtocolTests.cpp` for networking). Run with CTest or directly via the `Tests` executable.

### Per-material and per-frame UBO (D1.2 + D1.1b + D1.1c)

Per-frame camera/time state and per-material flags/colors/dirLight live in two std140 UBOs at fixed binding points (declared in `Manager/UboBindings.h`):

- `UBO_BINDING_FRAME = 0` — `FrameData` (view/projection/viewPos/uTime, 144 B). Populated once per frame by `RenderManager::populateAndUploadFrameUbo`; `PlanarReflectionRenderer::render3D` overwrites a copy with the mirrored view/viewPos for the reflection pass and re-uploads via `RenderManager::getFrameUbo()`.
- `UBO_BINDING_MATERIAL = 1` — `MaterialDataStd140` (192 B): alpha, ambient light color + intensity, useMaterial/overrideColorMesh/hasAlbedoTexture/normalMapEnabled/specularMapEnabled/pbrEnabled/hasHoleMap/fogEnable/reflectionEnable/rainDropEnable flags, rainSpeed/rainDensity, clipPlane, uvScale/uvOffset, directionLightEnable gate, plus a **per-material DirLight** (direction/ambient/diffuse/specular as 4× vec3+pad = 64 B). Each `MaterialInstance` owns one `MaterialUbo` + CPU shadow + dirty flag.
- `UBO_BINDING_PARTICLES = 2` — particle instance data (unchanged).

DirLight is per-material rather than global because PlayerScene/RemoteSnakeScene construct their own dim local `DirectionalLight` (diffuse 0.1, ambient 0.2) for the snake materials while MainScene's walls/floor use the bright global one. A FrameUBO-global dirLight would erase that distinction. `LightingFeature::bind` and `ShaderMaterial::bind` populate `material_dirLight_*` from their own `directional` pointer; shaders synthesize a local `DirLight dl` from these fields before calling `CalcDirLight*` (the `dl.position` field was dropped — it was dead).

Per-draw flow inside `MaterialInstance::bind`:
1. Bind program, set `model` (still a legacy uniform — kept per-draw on purpose).
2. Expose `&cpu` and `&dirty` through `RenderContext::materialData` / `materialDirty`.
3. Run features; each migrated feature writes directly into `*ctx.materialData` (no `setUniform` call). UBO writes are CPU struct mutations, not GL syscalls.
4. If `dirty`, upload the shadow and clear the flag, then `materialUbo.bind()` at slot 1.
5. Detach the ctx pointers so a reused context can't mutate another instance.

`ShaderMaterial` repopulates its own `MaterialDataStd140` from the generic `uniforms` map at bind time, so custom shaders that `#include "functions/material_data.glsl"` (or `lights.glsl`, which pulls it in) see the same `material_*` state as built-in shaders.

What stays legacy (still per-draw `setUniform`): `model`, point/spot light arrays (D1.1d candidate), `material.shininess` (Mesa `pow(x,0)` quirk — `DirectionalLight::bind` now only sets shininess + sampler ints), IBL gate, 2D shaders (`basic_2d.fs`, label/font), sampler indices set by features (`material.ambient`/`material.diffuse`/`material.specular` `setInt` for texture units).

Shader-side: GLSL blocks live in `examples/snake3/Assets/Shaders/functions/frame_data.glsl` and `material_data.glsl` (both declared `layout(std140)`). For helpers shared between basic.fs and 2D/shadow shaders, `alpha.glsl`/`fog.glsl` keep the legacy uniform path, while `alpha_material.glsl`/`fog_material.glsl` are the UBO variants used only by `basic.fs`. `ShaderProgram` ctor (`setupProgramDefaults`) calls `setUniformBlock("FrameData", 0)` and `setUniformBlock("MaterialData", 1)` on every program — programs that don't declare the block silently no-op (GL_INVALID_INDEX).

Measured impact (Debug build, MainScene steady state, ~609 draws/frame): post-migration ≈ 23,926 `setUniform` calls per frame (~39.3 per draw). Pre-D1.1b+D1.2 estimate adds back ~5 camera/time + ~15 material uniforms per draw, i.e. roughly 36k calls/frame — ~34% reduction. Remaining per-draw budget is dominated by light uniforms, `material.shininess`, sampler-index `setInt`s and a few feature-specific scalars still on the legacy path.
