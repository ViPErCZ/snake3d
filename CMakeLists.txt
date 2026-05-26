cmake_minimum_required(VERSION 3.22)
project(Snake3)

set(CMAKE_CXX_STANDARD 23)

# ============================================================================
# Dependencies
# ============================================================================
find_package(glfw3 3.1 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(OpenAL REQUIRED)
find_package(assimp 5.2 REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

option(USE_ENET "Enable ENet networking" ON)
if (USE_ENET)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(ENET QUIET enet)
    if (NOT ENET_FOUND)
        pkg_check_modules(ENET QUIET libenet)
    endif()
    if (NOT ENET_FOUND)
        find_path(ENET_INCLUDE_DIRS enet/enet.h)
        find_library(ENET_LIBRARIES NAMES enet libenet)
        if (ENET_INCLUDE_DIRS AND ENET_LIBRARIES)
            set(ENET_FOUND TRUE)
        endif()
    endif()
    if (NOT ENET_FOUND)
        message(FATAL_ERROR "ENet not found. Install libenet-dev and ensure pkg-config can find it.")
    endif()
    add_definitions(-DSNAKE3_USE_ENET)
    include_directories(${ENET_INCLUDE_DIRS})
endif()

include_directories(/usr/include/freetype2)
include_directories(/usr/include/AL)

# ============================================================================
# Asset copy target
# ============================================================================
add_custom_target(copy_assets ALL
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/Assets
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/examples/snake3/Assets ${CMAKE_BINARY_DIR}/Assets
        COMMENT "Cleaning and copying Assets folder"
)

# ============================================================================
# Dear ImGui (debug overlay - gated by IS_DEBUG)
# ============================================================================
include(FetchContent)
FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG v1.91.5
)
FetchContent_MakeAvailable(imgui)

add_library(imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(imgui PUBLIC glfw GL)

# ============================================================================
# H2 phase: ENGINE / GAME source split.
#
# ENGINE_SOURCES = reusable engine library (snake3d_engine) - no Snake3-specific
# vocabulary. GAME_SOURCES = Snake3 game on top of the engine. See
# ENGINE_BOUNDARY.md for classification rules.
#
# Physical file move (H3) keeps these in-place for now; targets just compile
# different subsets. After H3 the engine code physically lives under engine/
# and the game under examples/snake3/.
# ============================================================================

set(ENGINE_SOURCES
        # --- Manager (engine-side, excl. LevelManager/EatManager which are game)
        Manager/ResourceManager.cpp
        Manager/ResourceManager.h
        Manager/RenderManager.cpp
        Manager/RenderManager.h
        Manager/KeyboardManager.cpp
        Manager/KeyboardManager.h
        Manager/ShaderProgram.cpp
        Manager/ShaderProgram.h
        Manager/ShaderRegistry.cpp
        Manager/ShaderRegistry.h
        Manager/ShaderFeature.h
        Manager/MaterialPlaceholder.cpp
        Manager/MaterialPlaceholder.h
        Manager/VboIndexer.cpp
        Manager/VboIndexer.h
        Manager/Camera.cpp
        Manager/Camera.h
        Manager/TextureManager.cpp
        Manager/TextureManager.h
        Manager/SoundManager.cpp
        Manager/SoundManager.h
        Manager/UboBindings.h
        Manager/UniformBuffer.cpp
        Manager/UniformBuffer.h
        Manager/FrameUbo.cpp
        Manager/FrameUbo.h
        Manager/MaterialUbo.cpp
        Manager/MaterialUbo.h
        # --- Renderer / OpenGL
        Renderer/Opengl/BaseRenderer.cpp
        Renderer/Opengl/BaseRenderer.h
        Renderer/Opengl/RenderStats.cpp
        Renderer/Opengl/RenderStats.h
        Renderer/Opengl/BloomRenderer.cpp
        Renderer/Opengl/BloomRenderer.h
        Renderer/Opengl/DepthMapRenderer.cpp
        Renderer/Opengl/DepthMapRenderer.h
        Renderer/Opengl/PlanarReflectionRenderer.cpp
        Renderer/Opengl/PlanarReflectionRenderer.h
        Renderer/Opengl/Node3DRenderer.cpp
        Renderer/Opengl/Node3DRenderer.h
        Renderer/Opengl/Node2DRenderer.cpp
        Renderer/Opengl/Node2DRenderer.h
        Renderer/Opengl/Line.h
        # --- Renderer / Scene
        Renderer/Opengl/Scene/Scene.cpp
        Renderer/Opengl/Scene/Scene.h
        Renderer/Opengl/Scene/SceneRenderer.cpp
        Renderer/Opengl/Scene/SceneRenderer.h
        # --- Renderer / Material
        Renderer/Opengl/Material/BaseMaterial.h
        Renderer/Opengl/Material/ShaderMaterial.cpp
        Renderer/Opengl/Material/ShaderMaterial.h
        Renderer/Opengl/Material/RenderContext.h
        Renderer/Opengl/Material/TextureSlots.h
        Renderer/Opengl/Material/MaterialInstance.cpp
        Renderer/Opengl/Material/MaterialInstance.h
        Renderer/Opengl/Material/MaterialBuilder.cpp
        Renderer/Opengl/Material/MaterialBuilder.h
        Renderer/Opengl/Material/IUniform.h
        Renderer/Opengl/Material/Feature/IMaterialFeature.h
        Renderer/Opengl/Material/Feature/HoleMapFeature.cpp
        Renderer/Opengl/Material/Feature/HoleMapFeature.h
        Renderer/Opengl/Material/Feature/FogFeature.h
        Renderer/Opengl/Material/Feature/UvTransformFeature.h
        Renderer/Opengl/Material/Feature/AlbedoFeature.cpp
        Renderer/Opengl/Material/Feature/AlbedoFeature.h
        Renderer/Opengl/Material/Feature/NormalMapFeature.cpp
        Renderer/Opengl/Material/Feature/NormalMapFeature.h
        Renderer/Opengl/Material/Feature/SpecularFeature.cpp
        Renderer/Opengl/Material/Feature/SpecularFeature.h
        Renderer/Opengl/Material/Feature/ShadowFeature.cpp
        Renderer/Opengl/Material/Feature/ShadowFeature.h
        Renderer/Opengl/Material/Feature/LightingFeature.cpp
        Renderer/Opengl/Material/Feature/LightingFeature.h
        Renderer/Opengl/Material/Feature/PlanarReflectionFeature.cpp
        Renderer/Opengl/Material/Feature/PlanarReflectionFeature.h
        Renderer/Opengl/Material/Feature/PbrFeature.cpp
        Renderer/Opengl/Material/Feature/PbrFeature.h
        Renderer/Opengl/Material/Feature/IblFeature.cpp
        Renderer/Opengl/Material/Feature/IblFeature.h
        Renderer/Opengl/Material/Feature/BonesFeature.h
        Renderer/Opengl/Material/Feature/RainRippleFeature.h
        Renderer/Opengl/Material/Uniform/TextureUniform.cpp
        Renderer/Opengl/Material/Uniform/TextureUniform.h
        Renderer/Opengl/Material/Uniform/TextureArrayUniform.cpp
        Renderer/Opengl/Material/Uniform/TextureArrayUniform.h
        Renderer/Opengl/Material/Uniform/FadeOutUniform.cpp
        Renderer/Opengl/Material/Uniform/FadeOutUniform.h
        Renderer/Opengl/Material/Uniform/FadeInUniform.cpp
        Renderer/Opengl/Material/Uniform/FadeInUniform.h
        Renderer/Opengl/Material/Uniform/TimerUniform.cpp
        Renderer/Opengl/Material/Uniform/TimerUniform.h
        Renderer/Opengl/Material/Uniform/CallbackUniform.cpp
        Renderer/Opengl/Material/Uniform/CallbackUniform.h
        Renderer/Opengl/Material/Particle/BaseProcessMaterial.cpp
        Renderer/Opengl/Material/Particle/BaseProcessMaterial.h
        Renderer/Opengl/Material/Particle/ParticleProcessMaterial.cpp
        Renderer/Opengl/Material/Particle/ParticleProcessMaterial.h
        Renderer/Opengl/Material/Interface/BlendingInterface.cpp
        Renderer/Opengl/Material/Interface/BlendingInterface.h
        Renderer/Opengl/Material/2D/LabelSettings.cpp
        Renderer/Opengl/Material/2D/LabelSettings.h
        Renderer/Opengl/Material/2D/Font.cpp
        Renderer/Opengl/Material/2D/Font.h
        # --- Renderer / Model (Standard meshes, Debug, Collision, Utils)
        Renderer/Opengl/Model/Character.cpp
        Renderer/Opengl/Model/Character.h
        Renderer/Opengl/Model/SpinnerMesh.cpp
        Renderer/Opengl/Model/SpinnerMesh.h
        Renderer/Opengl/Model/Standard/MeshNode3D.cpp
        Renderer/Opengl/Model/Standard/MeshNode3D.h
        Renderer/Opengl/Model/Standard/StandardMesh.cpp
        Renderer/Opengl/Model/Standard/StandardMesh.h
        Renderer/Opengl/Model/Standard/BoxMesh.cpp
        Renderer/Opengl/Model/Standard/BoxMesh.h
        Renderer/Opengl/Model/Standard/PlaneMesh.cpp
        Renderer/Opengl/Model/Standard/PlaneMesh.h
        Renderer/Opengl/Model/Standard/SphereMesh.cpp
        Renderer/Opengl/Model/Standard/SphereMesh.h
        Renderer/Opengl/Model/Standard/CapsuleMesh.cpp
        Renderer/Opengl/Model/Standard/CapsuleMesh.h
        Renderer/Opengl/Model/Standard/CylinderMesh.cpp
        Renderer/Opengl/Model/Standard/CylinderMesh.h
        Renderer/Opengl/Model/Standard/QuadMesh3D.cpp
        Renderer/Opengl/Model/Standard/QuadMesh3D.h
        Renderer/Opengl/Model/Standard/TringleMesh3D.cpp
        Renderer/Opengl/Model/Standard/TringleMesh3D.h
        Renderer/Opengl/Model/Standard/ArrayMesh.cpp
        Renderer/Opengl/Model/Standard/ArrayMesh.h
        Renderer/Opengl/Model/Standard/AnimationArrayMesh.cpp
        Renderer/Opengl/Model/Standard/AnimationArrayMesh.h
        Renderer/Opengl/Model/Standard/WireframeArrowMesh.cpp
        Renderer/Opengl/Model/Standard/WireframeArrowMesh.h
        Renderer/Opengl/Model/Standard/SkyboxNode3D.cpp
        Renderer/Opengl/Model/Standard/SkyboxNode3D.h
        Renderer/Opengl/Model/Standard/GPUParticle3D.cpp
        Renderer/Opengl/Model/Standard/GPUParticle3D.h
        Renderer/Opengl/Model/Standard/Animation/AnimationPlayer.cpp
        Renderer/Opengl/Model/Standard/Animation/AnimationPlayer.h
        Renderer/Opengl/Model/Standard/2D/BaseNode2D.cpp
        Renderer/Opengl/Model/Standard/2D/BaseNode2D.h
        Renderer/Opengl/Model/Standard/2D/LabelNode2D.cpp
        Renderer/Opengl/Model/Standard/2D/LabelNode2D.h
        Renderer/Opengl/Model/Standard/2D/ImageNode2D.cpp
        Renderer/Opengl/Model/Standard/2D/ImageNode2D.h
        Renderer/Opengl/Model/Standard/2D/MeshNode2D.cpp
        Renderer/Opengl/Model/Standard/2D/MeshNode2D.h
        Renderer/Opengl/Model/Standard/2D/QuadNode2D.cpp
        Renderer/Opengl/Model/Standard/2D/QuadNode2D.h
        Renderer/Opengl/Model/Standard/2D/TringleNode2D.cpp
        Renderer/Opengl/Model/Standard/2D/TringleNode2D.h
        Renderer/Opengl/Model/Standard/2D/GPUParticle2D.cpp
        Renderer/Opengl/Model/Standard/2D/GPUParticle2D.h
        Renderer/Opengl/Model/Debug/LightNode3D.cpp
        Renderer/Opengl/Model/Debug/LightNode3D.h
        Renderer/Opengl/Model/Debug/DirectionalLightNode3D.cpp
        Renderer/Opengl/Model/Debug/DirectionalLightNode3D.h
        Renderer/Opengl/Model/Collision/CollisionShape3D.cpp
        Renderer/Opengl/Model/Collision/CollisionShape3D.h
        Renderer/Opengl/Model/Utils/Vao.cpp
        Renderer/Opengl/Model/Utils/Vao.h
        Renderer/Opengl/Model/Utils/Vbo.cpp
        Renderer/Opengl/Model/Utils/Vbo.h
        Renderer/Opengl/Model/Utils/Ebo.cpp
        Renderer/Opengl/Model/Utils/Ebo.h
        Renderer/Opengl/Model/Utils/Mesh.cpp
        Renderer/Opengl/Model/Utils/Mesh.h
        Renderer/Opengl/Model/Utils/Mesh2D.cpp
        Renderer/Opengl/Model/Utils/Mesh2D.h
        Renderer/Opengl/Model/Utils/TextMesh.cpp
        Renderer/Opengl/Model/Utils/TextMesh.h
        Renderer/Opengl/Model/Utils/Tree.h
        # --- Resource
        Resource/ResourceLoader.cpp
        Resource/ResourceLoader.h
        Resource/ObjModelLoader.cpp
        Resource/ObjModelLoader.h
        Resource/TextureLoader.cpp
        Resource/TextureLoader.h
        Resource/ShaderLoader.cpp
        Resource/ShaderLoader.h
        Resource/ShaderPreprocessor.cpp
        Resource/ShaderPreprocessor.h
        Resource/AnimLoader.cpp
        Resource/AnimLoader.h
        # --- Lights
        Lights/Light.cpp
        Lights/Light.h
        Lights/DirectionalLight.cpp
        Lights/DirectionalLight.h
        Lights/PointLight.cpp
        Lights/PointLight.h
        Lights/SpotLight.cpp
        Lights/SpotLight.h
        Lights/IOrientable.h
        Lights/OrientableLight.h
        # --- Physic
        Physic/Shape.h
        Physic/BoxShape.cpp
        Physic/BoxShape.h
        Physic/SphereShape.cpp
        Physic/SphereShape.h
        Physic/CapsuleShape.cpp
        Physic/CapsuleShape.h
        Physic/CylinderShape.cpp
        Physic/CylinderShape.h
        Physic/CollisionCheck.cpp
        Physic/CollisionCheck.h
        Physic/CollisionSystem3D.cpp
        Physic/CollisionSystem3D.h
        Physic/Algorithms/CollisionAlgorithms.cpp
        Physic/Algorithms/CollisionAlgorithms.h
        Physic/Dynamics/DynamicBody.cpp
        Physic/Dynamics/DynamicBody.h
        Physic/Jump/JumpTrajectory.cpp
        Physic/Jump/JumpTrajectory.h
        # --- Tools
        Tools/AnimItem.cpp
        Tools/AnimItem.h
        Tools/Timer.cpp
        Tools/Timer.h
        Tools/Transform.cpp
        Tools/Transform.h
        Tools/Named.cpp
        Tools/Named.h
        Tools/Tagged.cpp
        Tools/Tagged.h
        Tools/Visibility.cpp
        Tools/Visibility.h
        Tools/Vector3i.cpp
        Tools/Vector3i.h
        Tools/Environment.cpp
        Tools/Environment.h
        Tools/WorldEnvironment.cpp
        Tools/WorldEnvironment.h
        Tools/ContextState.cpp
        Tools/ContextState.h
        Tools/CameraOrbit.cpp
        Tools/CameraOrbit.h
        Tools/Capabilities.h
        Tools/Clear.h
        Tools/Blending.h
        Tools/BlendFactor.h
        Tools/DepthFunc.h
        Tools/DrawElement.h
        Tools/BuildSettings.h
        Tools/Layers.h
        # --- Network (generic netcode only - Game/* is game-side)
        Network/NetManager.cpp
        Network/NetManager.h
        Network/NetProtocol.cpp
        Network/NetProtocol.h
        Network/NetMessages.cpp
        Network/NetMessages.h
        Network/NetClientServer.cpp
        Network/NetClientServer.h
        Network/NetDispatcher.cpp
        Network/NetDispatcher.h
        Network/NetClock.cpp
        Network/NetClock.h
        # --- Handler (base + debug-mode handlers - generic)
        Handler/BaseHandler.cpp
        Handler/BaseHandler.h
        Handler/BaseKeydownHandle.h
        Handler/Debug/BaseTransform.cpp
        Handler/Debug/BaseTransform.h
        Handler/Debug/PositionHandler.cpp
        Handler/Debug/PositionHandler.h
        Handler/Debug/RotationHandler.cpp
        Handler/Debug/RotationHandler.h
        Handler/Debug/ScaleHandler.cpp
        Handler/Debug/ScaleHandler.h
        Handler/Debug/CollisionShapeHandler.cpp
        Handler/Debug/CollisionShapeHandler.h
        Handler/Debug/LightsHandler.cpp
        Handler/Debug/LightsHandler.h
        Handler/Debug/ManipulatorHandler.cpp
        Handler/Debug/ManipulatorHandler.h
        Handler/Debug/ImGuiOverlay.cpp
        Handler/Debug/ImGuiOverlay.h
)

set(GAME_LIB_SOURCES
        # Snake3-specific code that's reusable (e.g. by unit tests). Excludes
        # the entry-point shell (main + App glue), which lives in the
        # executable directly.
        # --- Scenes (all Snake3 gameplay)
        examples/snake3/src/Scenes/MainScene.cpp
        examples/snake3/src/Scenes/MainScene.h
        examples/snake3/src/Scenes/PlayerScene.cpp
        examples/snake3/src/Scenes/PlayerScene.h
        examples/snake3/src/Scenes/RemoteSnakeScene.cpp
        examples/snake3/src/Scenes/RemoteSnakeScene.h
        examples/snake3/src/Scenes/BarriersScene.cpp
        examples/snake3/src/Scenes/BarriersScene.h
        examples/snake3/src/Scenes/CoinScene.cpp
        examples/snake3/src/Scenes/CoinScene.h
        examples/snake3/src/Scenes/TorchScene.cpp
        examples/snake3/src/Scenes/TorchScene.h
        examples/snake3/src/Scenes/WeatherScene.cpp
        examples/snake3/src/Scenes/WeatherScene.h
        examples/snake3/src/Scenes/BoltScene.cpp
        examples/snake3/src/Scenes/BoltScene.h
        examples/snake3/src/Scenes/WinnerScene.cpp
        examples/snake3/src/Scenes/WinnerScene.h
        examples/snake3/src/Scenes/MainMenuScene.cpp
        examples/snake3/src/Scenes/MainMenuScene.h
        examples/snake3/src/Scenes/PreloaderScene.cpp
        examples/snake3/src/Scenes/PreloaderScene.h
        examples/snake3/src/Scenes/Preloader2Scene.cpp
        examples/snake3/src/Scenes/Preloader2Scene.h
        examples/snake3/src/Scenes/OrbitSceneBase.cpp
        examples/snake3/src/Scenes/OrbitSceneBase.h
        examples/snake3/src/Scenes/SceneLightFactory.cpp
        examples/snake3/src/Scenes/SceneLightFactory.h
        examples/snake3/src/Scenes/SceneHud.cpp
        examples/snake3/src/Scenes/SceneHud.h
        # --- Snake3-specific game objects
        examples/snake3/src/Renderer/Opengl/Model/Game/SnakeMeshNode3D.cpp
        examples/snake3/src/Renderer/Opengl/Model/Game/SnakeMeshNode3D.h
        examples/snake3/src/Renderer/Opengl/Model/Game/CoinMeshNode3D.cpp
        examples/snake3/src/Renderer/Opengl/Model/Game/CoinMeshNode3D.h
        examples/snake3/src/Renderer/Opengl/Model/Game/BarrelNode3D.cpp
        examples/snake3/src/Renderer/Opengl/Model/Game/BarrelNode3D.h
        examples/snake3/src/Renderer/Opengl/Model/Game/StreetLampNode3D.cpp
        examples/snake3/src/Renderer/Opengl/Model/Game/StreetLampNode3D.h
        examples/snake3/src/Renderer/Opengl/Model/Game/MarkRingNode3D.cpp
        examples/snake3/src/Renderer/Opengl/Model/Game/MarkRingNode3D.h
        examples/snake3/src/Renderer/Opengl/Model/Game/RadarItem.cpp
        examples/snake3/src/Renderer/Opengl/Model/Game/RadarItem.h
        examples/snake3/src/Renderer/Opengl/Model/Game/RadarMeshNode2D.cpp
        examples/snake3/src/Renderer/Opengl/Model/Game/RadarMeshNode2D.h
        examples/snake3/src/Renderer/Opengl/Model/Game/BoltLinesNode2D.cpp
        examples/snake3/src/Renderer/Opengl/Model/Game/BoltLinesNode2D.h
        # --- Snake3 game logic (Manager + Handler)
        examples/snake3/src/Manager/LevelManager.cpp
        examples/snake3/src/Manager/LevelManager.h
        examples/snake3/src/Manager/EatManager.cpp
        examples/snake3/src/Manager/EatManager.h
        examples/snake3/src/Handler/SnakeMoveHandler.cpp
        examples/snake3/src/Handler/SnakeMoveHandler.h
        examples/snake3/src/Handler/EatLocationHandler.cpp
        examples/snake3/src/Handler/EatLocationHandler.h
        examples/snake3/src/Handler/RadarHandler.cpp
        examples/snake3/src/Handler/RadarHandler.h
        # --- Snake3-specific networking
        examples/snake3/src/Network/Game/NetGameController.cpp
        examples/snake3/src/Network/Game/NetGameController.h
        examples/snake3/src/Network/Game/NetGameSnapshot.cpp
        examples/snake3/src/Network/Game/NetGameSnapshot.h
        examples/snake3/src/Network/Game/NetUtils.cpp
        examples/snake3/src/Network/Game/NetUtils.h
        examples/snake3/src/Network/Game/SnakeSnapshotApplier.cpp
        examples/snake3/src/Network/Game/SnakeSnapshotApplier.h
)

# ============================================================================
# snake3d_engine - reusable library (H2 phase: code stays in-place; H3 will
# physically move it under engine/).
# ============================================================================
add_library(snake3d_engine STATIC ${ENGINE_SOURCES})

target_include_directories(snake3d_engine PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(snake3d_engine PUBLIC
        glfw glut GLU GL GLEW freetype openal alut assimp nlohmann_json::nlohmann_json imgui
)
if (USE_ENET)
    target_link_libraries(snake3d_engine PUBLIC ${ENET_LIBRARIES})
endif()
target_compile_definitions(snake3d_engine PUBLIC $<$<CONFIG:Debug>:IS_DEBUG>)

# ============================================================================
# snake3d_game - Snake3 game library (Scenes, game-side nodes, gameplay
# logic, networking). Reusable by unit tests + the game executable shell.
# ============================================================================
add_library(snake3d_game STATIC ${GAME_LIB_SOURCES})
target_link_libraries(snake3d_game PUBLIC snake3d_engine)
target_compile_definitions(snake3d_game PUBLIC $<$<CONFIG:Debug>:IS_DEBUG>)

# ============================================================================
# Snake3 - the game executable (first example built on the engine).
# Contains only the entry-point shell (main + App glue) + links game lib.
# ============================================================================
add_executable(Snake3
        examples/snake3/src/main.cpp
        examples/snake3/src/App.cpp
        examples/snake3/src/App.h
)

target_link_libraries(Snake3 PRIVATE snake3d_game)
target_compile_definitions(Snake3 PRIVATE $<$<CONFIG:Debug>:IS_DEBUG>)

# Snake3 target depends on copy_assets so partial rebuilds get fresh shaders.
add_dependencies(Snake3 copy_assets)

file(COPY examples/snake3/Assets DESTINATION ${CMAKE_BINARY_DIR})

# ============================================================================
# Catch2 + Tests
# ============================================================================
enable_testing()

FetchContent_Declare(
        catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.4.0
)
FetchContent_MakeAvailable(catch2)

file(GLOB TEST_SOURCES "Tests/*.cpp")

add_executable(Tests ${TEST_SOURCES})

target_link_libraries(Tests PRIVATE
        snake3d_engine
        snake3d_game
        Catch2::Catch2WithMain
)

include(CTest)
include(Catch)
catch_discover_tests(Tests)
