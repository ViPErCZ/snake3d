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
        engine/src/Manager/ResourceManager.cpp
        engine/include/snake3d/Manager/ResourceManager.h
        engine/src/Manager/RenderManager.cpp
        engine/include/snake3d/Manager/RenderManager.h
        engine/src/Manager/KeyboardManager.cpp
        engine/include/snake3d/Manager/KeyboardManager.h
        engine/src/Manager/ShaderProgram.cpp
        engine/include/snake3d/Manager/ShaderProgram.h
        engine/src/Manager/ShaderRegistry.cpp
        engine/include/snake3d/Manager/ShaderRegistry.h
        engine/include/snake3d/Manager/ShaderFeature.h
        engine/src/Manager/MaterialPlaceholder.cpp
        engine/include/snake3d/Manager/MaterialPlaceholder.h
        engine/src/Manager/VboIndexer.cpp
        engine/include/snake3d/Manager/VboIndexer.h
        engine/src/Manager/Camera.cpp
        engine/include/snake3d/Manager/Camera.h
        engine/src/Manager/TextureManager.cpp
        engine/include/snake3d/Manager/TextureManager.h
        engine/src/Manager/SoundManager.cpp
        engine/include/snake3d/Manager/SoundManager.h
        engine/include/snake3d/Manager/UboBindings.h
        engine/src/Manager/UniformBuffer.cpp
        engine/include/snake3d/Manager/UniformBuffer.h
        engine/src/Manager/FrameUbo.cpp
        engine/include/snake3d/Manager/FrameUbo.h
        engine/src/Manager/MaterialUbo.cpp
        engine/include/snake3d/Manager/MaterialUbo.h
        # --- Renderer / OpenGL
        engine/src/Renderer/Opengl/BaseRenderer.cpp
        engine/include/snake3d/Renderer/Opengl/BaseRenderer.h
        engine/src/Renderer/Opengl/RenderStats.cpp
        engine/include/snake3d/Renderer/Opengl/RenderStats.h
        engine/src/Renderer/Opengl/BloomRenderer.cpp
        engine/include/snake3d/Renderer/Opengl/BloomRenderer.h
        engine/src/Renderer/Opengl/DepthMapRenderer.cpp
        engine/include/snake3d/Renderer/Opengl/DepthMapRenderer.h
        engine/src/Renderer/Opengl/PlanarReflectionRenderer.cpp
        engine/include/snake3d/Renderer/Opengl/PlanarReflectionRenderer.h
        engine/src/Renderer/Opengl/Node3DRenderer.cpp
        engine/include/snake3d/Renderer/Opengl/Node3DRenderer.h
        engine/src/Renderer/Opengl/Node2DRenderer.cpp
        engine/include/snake3d/Renderer/Opengl/Node2DRenderer.h
        engine/include/snake3d/Renderer/Opengl/Line.h
        # --- Renderer / Scene
        engine/src/Renderer/Opengl/Scene/Scene.cpp
        engine/include/snake3d/Renderer/Opengl/Scene/Scene.h
        engine/src/Renderer/Opengl/Scene/SceneRenderer.cpp
        engine/include/snake3d/Renderer/Opengl/Scene/SceneRenderer.h
        # --- Renderer / Material
        engine/include/snake3d/Renderer/Opengl/Material/BaseMaterial.h
        engine/src/Renderer/Opengl/Material/ShaderMaterial.cpp
        engine/include/snake3d/Renderer/Opengl/Material/ShaderMaterial.h
        engine/include/snake3d/Renderer/Opengl/Material/RenderContext.h
        engine/include/snake3d/Renderer/Opengl/Material/TextureSlots.h
        engine/src/Renderer/Opengl/Material/MaterialInstance.cpp
        engine/include/snake3d/Renderer/Opengl/Material/MaterialInstance.h
        engine/src/Renderer/Opengl/Material/MaterialBuilder.cpp
        engine/include/snake3d/Renderer/Opengl/Material/MaterialBuilder.h
        engine/include/snake3d/Renderer/Opengl/Material/IUniform.h
        engine/include/snake3d/Renderer/Opengl/Material/Feature/IMaterialFeature.h
        engine/src/Renderer/Opengl/Material/Feature/HoleMapFeature.cpp
        engine/include/snake3d/Renderer/Opengl/Material/Feature/HoleMapFeature.h
        engine/include/snake3d/Renderer/Opengl/Material/Feature/FogFeature.h
        engine/include/snake3d/Renderer/Opengl/Material/Feature/UvTransformFeature.h
        engine/src/Renderer/Opengl/Material/Feature/AlbedoFeature.cpp
        engine/include/snake3d/Renderer/Opengl/Material/Feature/AlbedoFeature.h
        engine/src/Renderer/Opengl/Material/Feature/NormalMapFeature.cpp
        engine/include/snake3d/Renderer/Opengl/Material/Feature/NormalMapFeature.h
        engine/src/Renderer/Opengl/Material/Feature/SpecularFeature.cpp
        engine/include/snake3d/Renderer/Opengl/Material/Feature/SpecularFeature.h
        engine/src/Renderer/Opengl/Material/Feature/ShadowFeature.cpp
        engine/include/snake3d/Renderer/Opengl/Material/Feature/ShadowFeature.h
        engine/src/Renderer/Opengl/Material/Feature/LightingFeature.cpp
        engine/include/snake3d/Renderer/Opengl/Material/Feature/LightingFeature.h
        engine/src/Renderer/Opengl/Material/Feature/PlanarReflectionFeature.cpp
        engine/include/snake3d/Renderer/Opengl/Material/Feature/PlanarReflectionFeature.h
        engine/src/Renderer/Opengl/Material/Feature/PbrFeature.cpp
        engine/include/snake3d/Renderer/Opengl/Material/Feature/PbrFeature.h
        engine/src/Renderer/Opengl/Material/Feature/IblFeature.cpp
        engine/include/snake3d/Renderer/Opengl/Material/Feature/IblFeature.h
        engine/include/snake3d/Renderer/Opengl/Material/Feature/BonesFeature.h
        engine/include/snake3d/Renderer/Opengl/Material/Feature/RainRippleFeature.h
        engine/src/Renderer/Opengl/Material/Uniform/TextureUniform.cpp
        engine/include/snake3d/Renderer/Opengl/Material/Uniform/TextureUniform.h
        engine/src/Renderer/Opengl/Material/Uniform/TextureArrayUniform.cpp
        engine/include/snake3d/Renderer/Opengl/Material/Uniform/TextureArrayUniform.h
        engine/src/Renderer/Opengl/Material/Uniform/FadeOutUniform.cpp
        engine/include/snake3d/Renderer/Opengl/Material/Uniform/FadeOutUniform.h
        engine/src/Renderer/Opengl/Material/Uniform/FadeInUniform.cpp
        engine/include/snake3d/Renderer/Opengl/Material/Uniform/FadeInUniform.h
        engine/src/Renderer/Opengl/Material/Uniform/TimerUniform.cpp
        engine/include/snake3d/Renderer/Opengl/Material/Uniform/TimerUniform.h
        engine/src/Renderer/Opengl/Material/Uniform/CallbackUniform.cpp
        engine/include/snake3d/Renderer/Opengl/Material/Uniform/CallbackUniform.h
        engine/src/Renderer/Opengl/Material/Particle/BaseProcessMaterial.cpp
        engine/include/snake3d/Renderer/Opengl/Material/Particle/BaseProcessMaterial.h
        engine/src/Renderer/Opengl/Material/Particle/ParticleProcessMaterial.cpp
        engine/include/snake3d/Renderer/Opengl/Material/Particle/ParticleProcessMaterial.h
        engine/src/Renderer/Opengl/Material/Interface/BlendingInterface.cpp
        engine/include/snake3d/Renderer/Opengl/Material/Interface/BlendingInterface.h
        engine/src/Renderer/Opengl/Material/2D/LabelSettings.cpp
        engine/include/snake3d/Renderer/Opengl/Material/2D/LabelSettings.h
        engine/src/Renderer/Opengl/Material/2D/Font.cpp
        engine/include/snake3d/Renderer/Opengl/Material/2D/Font.h
        # --- Renderer / Model (Standard meshes, Debug, Collision, Utils)
        engine/src/Renderer/Opengl/Model/Character.cpp
        engine/include/snake3d/Renderer/Opengl/Model/Character.h
        engine/src/Renderer/Opengl/Model/SpinnerMesh.cpp
        engine/include/snake3d/Renderer/Opengl/Model/SpinnerMesh.h
        engine/src/Renderer/Opengl/Model/Standard/MeshNode3D.cpp
        engine/include/snake3d/Renderer/Opengl/Model/Standard/MeshNode3D.h
        engine/src/Renderer/Opengl/Model/Standard/StandardMesh.cpp
        engine/include/snake3d/Renderer/Opengl/Model/Standard/StandardMesh.h
        engine/src/Renderer/Opengl/Model/Standard/BoxMesh.cpp
        engine/include/snake3d/Renderer/Opengl/Model/Standard/BoxMesh.h
        engine/src/Renderer/Opengl/Model/Standard/PlaneMesh.cpp
        engine/include/snake3d/Renderer/Opengl/Model/Standard/PlaneMesh.h
        engine/src/Renderer/Opengl/Model/Standard/SphereMesh.cpp
        engine/include/snake3d/Renderer/Opengl/Model/Standard/SphereMesh.h
        engine/src/Renderer/Opengl/Model/Standard/CapsuleMesh.cpp
        engine/include/snake3d/Renderer/Opengl/Model/Standard/CapsuleMesh.h
        engine/src/Renderer/Opengl/Model/Standard/CylinderMesh.cpp
        engine/include/snake3d/Renderer/Opengl/Model/Standard/CylinderMesh.h
        engine/src/Renderer/Opengl/Model/Standard/QuadMesh3D.cpp
        engine/include/snake3d/Renderer/Opengl/Model/Standard/QuadMesh3D.h
        engine/src/Renderer/Opengl/Model/Standard/TringleMesh3D.cpp
        engine/include/snake3d/Renderer/Opengl/Model/Standard/TringleMesh3D.h
        engine/src/Renderer/Opengl/Model/Standard/ArrayMesh.cpp
        engine/include/snake3d/Renderer/Opengl/Model/Standard/ArrayMesh.h
        engine/src/Renderer/Opengl/Model/Standard/AnimationArrayMesh.cpp
        engine/include/snake3d/Renderer/Opengl/Model/Standard/AnimationArrayMesh.h
        engine/src/Renderer/Opengl/Model/Standard/WireframeArrowMesh.cpp
        engine/include/snake3d/Renderer/Opengl/Model/Standard/WireframeArrowMesh.h
        engine/src/Renderer/Opengl/Model/Standard/SkyboxNode3D.cpp
        engine/include/snake3d/Renderer/Opengl/Model/Standard/SkyboxNode3D.h
        engine/src/Renderer/Opengl/Model/Standard/GPUParticle3D.cpp
        engine/include/snake3d/Renderer/Opengl/Model/Standard/GPUParticle3D.h
        engine/src/Renderer/Opengl/Model/Standard/Animation/AnimationPlayer.cpp
        engine/include/snake3d/Renderer/Opengl/Model/Standard/Animation/AnimationPlayer.h
        engine/src/Renderer/Opengl/Model/Standard/2D/BaseNode2D.cpp
        engine/include/snake3d/Renderer/Opengl/Model/Standard/2D/BaseNode2D.h
        engine/src/Renderer/Opengl/Model/Standard/2D/LabelNode2D.cpp
        engine/include/snake3d/Renderer/Opengl/Model/Standard/2D/LabelNode2D.h
        engine/src/Renderer/Opengl/Model/Standard/2D/ImageNode2D.cpp
        engine/include/snake3d/Renderer/Opengl/Model/Standard/2D/ImageNode2D.h
        engine/src/Renderer/Opengl/Model/Standard/2D/MeshNode2D.cpp
        engine/include/snake3d/Renderer/Opengl/Model/Standard/2D/MeshNode2D.h
        engine/src/Renderer/Opengl/Model/Standard/2D/QuadNode2D.cpp
        engine/include/snake3d/Renderer/Opengl/Model/Standard/2D/QuadNode2D.h
        engine/src/Renderer/Opengl/Model/Standard/2D/TringleNode2D.cpp
        engine/include/snake3d/Renderer/Opengl/Model/Standard/2D/TringleNode2D.h
        engine/src/Renderer/Opengl/Model/Standard/2D/GPUParticle2D.cpp
        engine/include/snake3d/Renderer/Opengl/Model/Standard/2D/GPUParticle2D.h
        engine/src/Renderer/Opengl/Model/Debug/LightNode3D.cpp
        engine/include/snake3d/Renderer/Opengl/Model/Debug/LightNode3D.h
        engine/src/Renderer/Opengl/Model/Debug/DirectionalLightNode3D.cpp
        engine/include/snake3d/Renderer/Opengl/Model/Debug/DirectionalLightNode3D.h
        engine/src/Renderer/Opengl/Model/Collision/CollisionShape3D.cpp
        engine/include/snake3d/Renderer/Opengl/Model/Collision/CollisionShape3D.h
        engine/src/Renderer/Opengl/Model/Utils/Vao.cpp
        engine/include/snake3d/Renderer/Opengl/Model/Utils/Vao.h
        engine/src/Renderer/Opengl/Model/Utils/Vbo.cpp
        engine/include/snake3d/Renderer/Opengl/Model/Utils/Vbo.h
        engine/src/Renderer/Opengl/Model/Utils/Ebo.cpp
        engine/include/snake3d/Renderer/Opengl/Model/Utils/Ebo.h
        engine/src/Renderer/Opengl/Model/Utils/Mesh.cpp
        engine/include/snake3d/Renderer/Opengl/Model/Utils/Mesh.h
        engine/src/Renderer/Opengl/Model/Utils/Mesh2D.cpp
        engine/include/snake3d/Renderer/Opengl/Model/Utils/Mesh2D.h
        engine/src/Renderer/Opengl/Model/Utils/TextMesh.cpp
        engine/include/snake3d/Renderer/Opengl/Model/Utils/TextMesh.h
        engine/include/snake3d/Renderer/Opengl/Model/Utils/Tree.h
        # --- Resource
        engine/src/Resource/ResourceLoader.cpp
        engine/include/snake3d/Resource/ResourceLoader.h
        engine/src/Resource/ObjModelLoader.cpp
        engine/include/snake3d/Resource/ObjModelLoader.h
        engine/src/Resource/TextureLoader.cpp
        engine/include/snake3d/Resource/TextureLoader.h
        engine/src/Resource/ShaderLoader.cpp
        engine/include/snake3d/Resource/ShaderLoader.h
        engine/src/Resource/ShaderPreprocessor.cpp
        engine/include/snake3d/Resource/ShaderPreprocessor.h
        engine/src/Resource/AnimLoader.cpp
        engine/include/snake3d/Resource/AnimLoader.h
        engine/src/Resource/MaterialLoader.cpp
        engine/include/snake3d/Resource/MaterialLoader.h
        # --- Lights
        engine/src/Lights/Light.cpp
        engine/include/snake3d/Lights/Light.h
        engine/src/Lights/DirectionalLight.cpp
        engine/include/snake3d/Lights/DirectionalLight.h
        engine/src/Lights/PointLight.cpp
        engine/include/snake3d/Lights/PointLight.h
        engine/src/Lights/SpotLight.cpp
        engine/include/snake3d/Lights/SpotLight.h
        engine/include/snake3d/Lights/IOrientable.h
        engine/include/snake3d/Lights/OrientableLight.h
        # --- Physic
        engine/include/snake3d/Physic/Shape.h
        engine/src/Physic/BoxShape.cpp
        engine/include/snake3d/Physic/BoxShape.h
        engine/src/Physic/SphereShape.cpp
        engine/include/snake3d/Physic/SphereShape.h
        engine/src/Physic/CapsuleShape.cpp
        engine/include/snake3d/Physic/CapsuleShape.h
        engine/src/Physic/CylinderShape.cpp
        engine/include/snake3d/Physic/CylinderShape.h
        engine/src/Physic/CollisionCheck.cpp
        engine/include/snake3d/Physic/CollisionCheck.h
        engine/src/Physic/CollisionSystem3D.cpp
        engine/include/snake3d/Physic/CollisionSystem3D.h
        engine/src/Physic/Algorithms/CollisionAlgorithms.cpp
        engine/include/snake3d/Physic/Algorithms/CollisionAlgorithms.h
        engine/src/Physic/Dynamics/DynamicBody.cpp
        engine/include/snake3d/Physic/Dynamics/DynamicBody.h
        engine/src/Physic/Jump/JumpTrajectory.cpp
        engine/include/snake3d/Physic/Jump/JumpTrajectory.h
        # --- Tools
        engine/src/Tools/AnimItem.cpp
        engine/include/snake3d/Tools/AnimItem.h
        engine/src/Tools/Timer.cpp
        engine/include/snake3d/Tools/Timer.h
        engine/src/Tools/Transform.cpp
        engine/include/snake3d/Tools/Transform.h
        engine/src/Tools/Named.cpp
        engine/include/snake3d/Tools/Named.h
        engine/src/Tools/Tagged.cpp
        engine/include/snake3d/Tools/Tagged.h
        engine/src/Tools/Visibility.cpp
        engine/include/snake3d/Tools/Visibility.h
        engine/src/Tools/Vector3i.cpp
        engine/include/snake3d/Tools/Vector3i.h
        engine/src/Tools/Environment.cpp
        engine/include/snake3d/Tools/Environment.h
        engine/src/Tools/WorldEnvironment.cpp
        engine/include/snake3d/Tools/WorldEnvironment.h
        engine/src/Tools/ContextState.cpp
        engine/include/snake3d/Tools/ContextState.h
        engine/src/Tools/CameraOrbit.cpp
        engine/include/snake3d/Tools/CameraOrbit.h
        engine/include/snake3d/Tools/Capabilities.h
        engine/include/snake3d/Tools/Clear.h
        engine/include/snake3d/Tools/Blending.h
        engine/include/snake3d/Tools/BlendFactor.h
        engine/include/snake3d/Tools/DepthFunc.h
        engine/include/snake3d/Tools/DrawElement.h
        engine/include/snake3d/Tools/BuildSettings.h
        engine/include/snake3d/Tools/Layers.h
        # --- Network (generic netcode only - Game/* is game-side)
        engine/src/Network/NetManager.cpp
        engine/include/snake3d/Network/NetManager.h
        engine/src/Network/NetProtocol.cpp
        engine/include/snake3d/Network/NetProtocol.h
        engine/src/Network/NetMessages.cpp
        engine/include/snake3d/Network/NetMessages.h
        engine/src/Network/NetClientServer.cpp
        engine/include/snake3d/Network/NetClientServer.h
        engine/src/Network/NetDispatcher.cpp
        engine/include/snake3d/Network/NetDispatcher.h
        engine/src/Network/NetClock.cpp
        engine/include/snake3d/Network/NetClock.h
        # --- Handler (base + debug-mode handlers - generic)
        engine/src/Handler/BaseHandler.cpp
        engine/include/snake3d/Handler/BaseHandler.h
        engine/include/snake3d/Handler/BaseKeydownHandle.h
        engine/src/Handler/Debug/BaseTransform.cpp
        engine/include/snake3d/Handler/Debug/BaseTransform.h
        engine/src/Handler/Debug/PositionHandler.cpp
        engine/include/snake3d/Handler/Debug/PositionHandler.h
        engine/src/Handler/Debug/RotationHandler.cpp
        engine/include/snake3d/Handler/Debug/RotationHandler.h
        engine/src/Handler/Debug/ScaleHandler.cpp
        engine/include/snake3d/Handler/Debug/ScaleHandler.h
        engine/src/Handler/Debug/CollisionShapeHandler.cpp
        engine/include/snake3d/Handler/Debug/CollisionShapeHandler.h
        engine/src/Handler/Debug/LightsHandler.cpp
        engine/include/snake3d/Handler/Debug/LightsHandler.h
        engine/src/Handler/Debug/ManipulatorHandler.cpp
        engine/include/snake3d/Handler/Debug/ManipulatorHandler.h
        engine/src/Handler/Debug/ImGuiOverlay.cpp
        engine/include/snake3d/Handler/Debug/ImGuiOverlay.h
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
        ${CMAKE_CURRENT_SOURCE_DIR}/engine/include
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
