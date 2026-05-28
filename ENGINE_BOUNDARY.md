# Engine / Game Boundary

This document defines what is **engine code** (reusable across games) vs **game
code** (Snake3-specific). It exists as a mental model now (H1 phase) — file
layout still mixes both. Code move comes in H3.

## Goal

Snake3 is the first concrete game built on top of the engine layer. The engine
is designed to be reused for further games (e.g. a planned 3D Pac-Man-like
prototype). Open-source distribution requires a clean public API surface.

Until the H3 code move, **new code should be classified mentally** as engine
or game and placed accordingly:

- Engine code: generic, reusable, no Snake3-specific assumptions, no references
  to gameplay rules / Snake3 entities.
- Game code: free to reference Snake3 entities, gameplay rules, scene-specific
  flow.

## Engine layer (reusable)

### `Manager/`
- `Camera`, `KeyboardManager`, `RenderManager`, `ResourceManager`
- `ShaderProgram`, `ShaderRegistry`, `ShaderFeature`, `MaterialPlaceholder`
- `SoundManager`, `TextureManager`, `VboIndexer`

### `Renderer/Opengl/`
- `BaseRenderer`, `Node2DRenderer`, `Node3DRenderer`, `BloomRenderer`,
  `DepthMapRenderer`, `PlanarReflectionRenderer`, `RainDropRenderer`,
  `BoltRenderer`
- `Material/` (entire subtree — `MaterialBuilder`, `MaterialInstance`,
  `BaseMaterial`, `ShaderMaterial`, `Feature/*`, `Uniform/*`, `Particle/*`,
  `TextureSlots`, `RenderContext`)
- `Model/Standard/` (entire subtree — `MeshNode3D`, `AnimationArrayMesh`,
  `ArrayMesh`, `BoxMesh`, `CapsuleMesh`, `CylinderMesh`, `PlaneMesh`,
  `QuadMesh3D`, `SkyboxNode3D`, `GPUParticle3D`, `2D/*`, `Animation/*`)
- `Model/Debug/` — `DirectionalLightNode3D`, `LightNode3D` (light gizmos)
- `Model/Collision/` — collision visualization
- `Model/Utils/` — `VAO`, `VBO`, `EBO`, `Mesh`, `Tree`
- `Model/Character`, `Model/SpinnerMesh`
- `Scene/` — `Scene` base class, `SceneRenderer`

### `Resource/`
- `ShaderLoader`, `ShaderPreprocessor`, `ResourceLoader`, `TextureLoader`,
  `ObjModelLoader`, `AnimLoader`

### `Lights/`
- `Light`, `DirectionalLight`, `PointLight`, `SpotLight`, `OrientableLight`

### `Physic/`
- `CollisionSystem3D`, `CollisionDetector`, `CollisionCheck`
- `BoxShape`, `SphereShape`, `CapsuleShape`, `CylinderShape`
- `Algorithms/*`, `Dynamics/*`

### `Tools/`
- `Timer`, `CameraOrbit`, `ContextState`, `Layers`, `BuildSettings`
- `Environment`, `AnimItem`

### `Network/` (engine-side only)
- `NetManager`, `NetClient`, `NetClientServer`, `NetClock`, `NetDispatcher`
- `NetMessages`, `NetProtocol`

### `Handler/`
- `BaseHandler`, `BaseKeydownHandle`
- `Debug/` (entire subtree — `ManipulatorHandler`, `PositionHandler`,
  `RotationHandler`, `ScaleHandler`, `CollisionShapeHandler`, `LightsHandler`,
  `BaseTransform`)

### `ItemsDto/`
- `BaseItem`, `AnimItem`, `Transform`, `Vector3i`, component interfaces

## Game layer (Snake3-specific)

### `Scenes/`
**All of:** `MainScene`, `PlayerScene`, `RemoteSnakeScene`, `BarriersScene`,
`CoinScene`, `TorchScene`, `WeatherScene`, `BoltScene`, `WinnerScene`,
`MainMenuScene`, `PreloaderScene`, `Preloader2Scene`, `SceneHud`,
`SceneLightFactory`, `OrbitSceneBase`.

### `Renderer/Opengl/Model/Game/`
**All of:** `SnakeMeshNode3D`, `CoinMeshNode3D`, `BarrelNode3D`,
`StreetLampNode3D`, `MarkRingNode3D`, `RadarItem`, `RadarMeshNode2D`,
`BoltLinesNode2D`.

### `Network/Game/`
**All of:** `NetGameController`, `NetGameSnapshot`, `SnakeSnapshotApplier`,
`NetUtils`, `NetWorldSource`, `NetWorldSink`.

### `Manager/`
- `LevelManager` — Snake3 level boxes, holes, eat counter, lives.
- `EatManager` — Snake3 coin pickup logic.

### `Handler/`
- `SnakeMoveHandler` — Snake3 movement state machine.
- `EatLocationHandler` — Snake3 coin location queries.
- `RadarHandler` — Snake3 HUD radar (consider moving if other games use radar).

### `Assets/`
All current assets are Snake3-specific. Engine examples/templates will live
under `examples/<name>/Assets/` post-H3.

### Top-level
- `main.cpp` — Snake3 bootstrap (creates `App`, runs main loop).
- `App.cpp/h` — Snake3 application class. Currently includes `MainScene`
  directly — coupling marked as known issue (see below).

## Known coupling issues (to address in H3/H4)

These violate engine/game separation and should be cleaned up during code move
or API audit phases. Listed for visibility, not blocking H1.

### Game leak inside engine code
- `MeshNode3D::setDirectionalLight/setSpotLights/setPointLights` — base mesh
  node should not have direct light setters. Lights belong to scene-level
  binding, propagated via uniform context or per-material `LightingFeature`.
- `using namespace Manager;` and `using namespace Material;` at top of engine
  headers (e.g. `IMaterialFeature.h`, `ResourceManager.h`). Headers should
  use fully-qualified names; namespace pollution leaks into game code.

### Engine entry points game-dependent
- `App.cpp` directly includes `MainScene.h`. Engine layer should provide a
  generic `Application` base class; Snake3 derives and instantiates its main
  scene. Same for `main.cpp`.

### Snake3-specific names in engine paths
- `Renderer/Opengl/Model/Game/` directory exists inside the engine renderer
  tree. H3 will move its contents into `examples/snake3/src/Model/`.

## Cross-cutting principles

When writing new code:

1. **No Snake3 vocabulary in engine code.** No "snake", "coin", "level box",
   "eat counter", "barriers" referenced in `Renderer/`, `Manager/` (except
   `LevelManager`/`EatManager`), `Material/`, `Physic/`, etc.

2. **Engine code uses interfaces, not concrete game classes.** If engine code
   needs a callback from gameplay, it takes `std::function<>` or an abstract
   base class, not a Snake3 type.

3. **Game code may freely use engine APIs.** That's the point — engine is the
   library, game is the consumer.

4. **Engine public headers are forward-declaration-friendly.** Heavy includes
   (e.g. GLEW, GLM full headers) stay in implementation files. Game code
   includes minimal interface.

5. **No `using namespace` in engine public headers.** Internal `.cpp` files
   may use it for brevity; headers exported as public API must use qualified
   names.

## Target layout (post-H3)

```
snake3d/
├── engine/
│   ├── include/snake3d/     <- public engine API
│   │   ├── Material/
│   │   ├── Scene/
│   │   ├── Manager/
│   │   ├── Renderer/
│   │   ├── Lights/
│   │   ├── Physic/
│   │   ├── Network/         <- generic netcode only
│   │   ├── Resource/
│   │   ├── Handler/
│   │   └── Tools/
│   └── src/                 <- engine implementation (.cpp + impl headers)
├── examples/
│   ├── snake3/              <- Snake3 game (current code, moved)
│   │   ├── src/
│   │   └── Assets/
│   ├── pacman3d/            <- (future, when second game starts)
│   └── minimal/             <- "hello triangle" reference for new authors
└── Tests/                   <- engine + per-example tests
```

## Status

- **H1:** mental model — ✓ done.
- **H2:** CMake split into `snake3d_engine` + `snake3d_game` libs + Snake3 executable + Tests — ✓ done.
- **H3:** physical file move into `examples/snake3/src/` and `examples/snake3/Assets/` — ✓ done.
- **H4a:** open source bootstrap (README, CONTRIBUTING, MIT LICENSE, .clang-format, CI workflow) + selective public-header `using namespace` cleanup (IMaterialFeature.h) — partial.
- **H4b:** full `using namespace` audit across engine headers — **done**. Per-header migration with explicit `using std::shared_ptr;` etc. in consumer `.cpp` files, one engine subdir at a time:
  - ✓ Lights/, Resource/, Manager/ (Camera.h unblocked), Physic/, Handler/Debug/, Tools/ + Transform.h unblock.
  - ✓ Namespace rename `ItemsDto` → `Animation` + unification with `Animations` (struct renamed `Animation` → `AnimationClip`).
  - ✓ Renderer/Opengl/: Model/Utils/, all leaf headers (Line, LabelSettings, BlendingInterface, Material/Uniform), Model/Standard/2D/, Model/Collision, Material/Particle, Material/Feature, AnimationPlayer, Material/ top-level, Scene/SceneRenderer + 5 specialized renderers, Model/Standard/.
  - ✓ Final cluster cleanup: Vbo.h + MeshNode3D.h + Scene.h cleaned together with their downstream cascade (Mesh2D, all Standard/* meshes, CollisionShape3D, LightNode3D, all Handler/Debug/*, all game scenes + game meshes). ~30 impl files received `using namespace X;` after includes per project convention.
  - Engine-side headers (Renderer/, Manager/, Physic/, Tools/, Lights/, Resource/, Handler/, Network/) are now 100% free of `using namespace X;` at top level.
  - **Game-side parked headers**: App.h, EatLocationHandler.h, SnakeMoveHandler.h, RadarHandler.h, LevelManager.h, EatManager.h still carry `using namespace` lines. Game-layer concerns, outside engine boundary — can be addressed independently when convenient.
- **H4c:** engine sources physically reorganized into `engine/include/snake3d/` (public headers) + `engine/src/` (implementation) — ✓ done. All ~800 include sites rewritten to angle-bracket `<snake3d/...>` form. Root-level engine dirs (`Lights/`, `Tools/`, `Resource/`, `Network/`, `Physic/`, `Handler/`, `Manager/`, `Renderer/`) removed. `stdafx.h` lives at `engine/include/stdafx.h`. CMake exposes `engine/include` as `snake3d_engine`'s PUBLIC include path; `examples/snake3/src` is `snake3d_game`'s PUBLIC include path; Thirdparty single-header libraries (stb_image) are PRIVATE to engine + game targets. Repo root is no longer on any target's PUBLIC include path. The post-H3 target layout above is now the literal repo layout. 89/89 tests pass; clean Debug + Release builds green.
- **H5:** feature plugin registry (data-driven `IMaterialFeature` factory).
