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

# Minimal example assets live in their own MinimalAssets/ root so they never
# collide with copy_assets (which wipes ${CMAKE_BINARY_DIR}/Assets first).
add_custom_target(copy_minimal_assets ALL
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/MinimalAssets
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/examples/minimal/Assets ${CMAKE_BINARY_DIR}/MinimalAssets
        COMMENT "Cleaning and copying Minimal example assets"
)

# Pacman3D example assets live in their own PacmanAssets/ root (same pattern as
# MinimalAssets/) so they never collide with copy_assets.
add_custom_target(copy_pacman_assets ALL
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/PacmanAssets
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/examples/pacman3d/Assets ${CMAKE_BINARY_DIR}/PacmanAssets
        COMMENT "Cleaning and copying Pacman3D example assets"
)

# Gumman3D example assets live in their own GummanAssets/ root.
add_custom_target(copy_gumman_assets ALL
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/GummanAssets
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/examples/gumman3d/Assets ${CMAKE_BINARY_DIR}/GummanAssets
        COMMENT "Cleaning and copying Gumman3D example assets"
)

# RTS3D example assets live in their own RtsAssets/ root.
add_custom_target(copy_rts_assets ALL
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/RtsAssets
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/examples/rts/Assets ${CMAKE_BINARY_DIR}/RtsAssets
        COMMENT "Cleaning and copying RTS3D example assets"
)

# FPS example assets live in their own FpsAssets/ root.
add_custom_target(copy_fps_assets ALL
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/FpsAssets
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/examples/fps/Assets ${CMAKE_BINARY_DIR}/FpsAssets
        COMMENT "Cleaning and copying FPS example assets"
)

# Engine-owned shader prelude (frame_data.glsl, material_data.glsl, ...).
# Copied into ${CMAKE_BINARY_DIR}/EngineShaders, which ShaderLoader uses as
# its default include root: any shader can `#include "snake3d/frame_data.glsl"`
# and the loader resolves it here when not found next to the shader itself.
# This is the single source of truth for the UBO layout - examples no longer
# duplicate it. Every executable that compiles shaders depends on this target.
add_custom_target(copy_engine_shaders ALL
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/EngineShaders
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/engine/shaders ${CMAKE_BINARY_DIR}/EngineShaders
        COMMENT "Copying engine shader prelude (EngineShaders/)"
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
        # --- Application base class (engine entry point)
        engine/src/Application.cpp
        engine/include/snake3d/Application.h
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
        engine/src/Renderer/Opengl/RefractionRenderer.cpp
        engine/include/snake3d/Renderer/Opengl/RefractionRenderer.h
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
        engine/src/Renderer/Opengl/Material/Feature/EmissiveFeature.cpp
        engine/include/snake3d/Renderer/Opengl/Material/Feature/EmissiveFeature.h
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
        engine/src/Renderer/Opengl/Model/Standard/SelectionRingNode3D.cpp
        engine/include/snake3d/Renderer/Opengl/Model/Standard/SelectionRingNode3D.h
        engine/src/Renderer/Opengl/Model/Standard/StandardMesh.cpp
        engine/include/snake3d/Renderer/Opengl/Model/Standard/StandardMesh.h
        engine/src/Renderer/Opengl/Decal/DecalManager.cpp
        engine/include/snake3d/Renderer/Opengl/Decal/DecalManager.h
        engine/src/Renderer/Opengl/Model/Standard/BoxMesh.cpp
        engine/include/snake3d/Renderer/Opengl/Model/Standard/BoxMesh.h
        engine/src/Renderer/Opengl/Model/Standard/PlaneMesh.cpp
        engine/include/snake3d/Renderer/Opengl/Model/Standard/PlaneMesh.h
        engine/src/Renderer/Opengl/Model/Standard/HeightfieldMesh.cpp
        engine/include/snake3d/Renderer/Opengl/Model/Standard/HeightfieldMesh.h
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
        engine/src/Resource/FeatureRegistry.cpp
        engine/include/snake3d/Resource/FeatureRegistry.h
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
        engine/src/Physic/CharacterController.cpp
        engine/include/snake3d/Physic/CharacterController.h
        engine/src/Physic/Jump/JumpTrajectory.cpp
        engine/include/snake3d/Physic/Jump/JumpTrajectory.h
        # --- Tools
        engine/src/Tools/AnimItem.cpp
        engine/include/snake3d/Tools/AnimItem.h
        engine/src/Tools/Timer.cpp
        engine/include/snake3d/Tools/Timer.h
        engine/src/Tools/Transform.cpp
        engine/include/snake3d/Tools/Transform.h
        engine/src/Tools/Frustum.cpp
        engine/include/snake3d/Tools/Frustum.h
        engine/src/Tools/NavGrid.cpp
        engine/include/snake3d/Tools/NavGrid.h
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
        examples/snake3/src/Renderer/Opengl/Model/Game/LitMeshNode3D.cpp
        examples/snake3/src/Renderer/Opengl/Model/Game/LitMeshNode3D.h
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
        ${CMAKE_CURRENT_SOURCE_DIR}/engine/include
)

# Thirdparty single-header libraries (stb_image, etc.) live at repo root and
# are an implementation detail of the engine — exposed only to engine sources.
target_include_directories(snake3d_engine PRIVATE
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
target_include_directories(snake3d_game PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/snake3/src
)
# Thirdparty single-header libraries (stb_image, etc.) live at repo root and
# are an implementation detail of the game — exposed only to game sources.
target_include_directories(snake3d_game PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(snake3d_game PUBLIC snake3d_engine)
target_compile_definitions(snake3d_game PUBLIC $<$<CONFIG:Debug>:IS_DEBUG>)

# ============================================================================
# Snake3 - the game executable (first example built on the engine).
# Contains only the entry-point shell (main + App glue) + links game lib.
# ============================================================================
add_executable(Snake3
        examples/snake3/src/main.cpp
        examples/snake3/src/Snake3App.cpp
        examples/snake3/src/Snake3App.h
)

target_link_libraries(Snake3 PRIVATE snake3d_game)
target_compile_definitions(Snake3 PRIVATE $<$<CONFIG:Debug>:IS_DEBUG>)

# Snake3 target depends on copy_assets so partial rebuilds get fresh shaders.
add_dependencies(Snake3 copy_assets copy_engine_shaders)

file(COPY examples/snake3/Assets DESTINATION ${CMAKE_BINARY_DIR})

# ============================================================================
# Minimal - tiny "hello, engine" example. Links ONLY snake3d_engine (NOT
# snake3d_game) to prove the H6 Application base class is reusable without any
# Snake3 game code.
# ============================================================================
add_executable(Minimal
        examples/minimal/src/main.cpp
        examples/minimal/src/MinimalApp.cpp
        examples/minimal/src/MinimalApp.h
)

target_link_libraries(Minimal PRIVATE snake3d_engine)
target_compile_definitions(Minimal PRIVATE $<$<CONFIG:Debug>:IS_DEBUG>)

add_dependencies(Minimal copy_minimal_assets copy_engine_shaders)

# ============================================================================
# Pacman3d - second playable prototype (grid maze, player, pellets, score).
# Links ONLY snake3d_engine (NOT snake3d_game) to prove the engine is reusable
# for a genuinely different game than Snake3.
# ============================================================================
add_executable(Pacman3d
        examples/pacman3d/src/main.cpp
        examples/pacman3d/src/Pacman3dApp.cpp
        examples/pacman3d/src/Pacman3dApp.h
        examples/pacman3d/src/Maze.h
        examples/pacman3d/src/Scenes/PacmanCommon.h
        examples/pacman3d/src/Scenes/MazeScene.cpp
        examples/pacman3d/src/Scenes/MazeScene.h
        examples/pacman3d/src/Scenes/PelletScene.cpp
        examples/pacman3d/src/Scenes/PelletScene.h
        examples/pacman3d/src/Scenes/PlayerScene.cpp
        examples/pacman3d/src/Scenes/PlayerScene.h
        examples/pacman3d/src/Scenes/GhostScene.cpp
        examples/pacman3d/src/Scenes/GhostScene.h
        examples/pacman3d/src/Scenes/Pacman3dMainScene.cpp
        examples/pacman3d/src/Scenes/Pacman3dMainScene.h
        examples/pacman3d/src/Scenes/HudScene.cpp
        examples/pacman3d/src/Scenes/HudScene.h
)

target_link_libraries(Pacman3d PRIVATE snake3d_engine)
target_compile_definitions(Pacman3d PRIVATE $<$<CONFIG:Debug>:IS_DEBUG>)

add_dependencies(Pacman3d copy_pacman_assets copy_engine_shaders)

# ============================================================================
# Gumman3d - third playable prototype: a Jazz-Jackrabbit-style 2.5D platformer.
# Links ONLY snake3d_engine (NOT snake3d_game). First real user of the engine
# dynamic-body / gravity physics path. Needs nlohmann_json for the level loader.
# ============================================================================
add_executable(Gumman3d
        examples/gumman3d/src/main.cpp
        examples/gumman3d/src/Gumman3dApp.cpp
        examples/gumman3d/src/Gumman3dApp.h
        examples/gumman3d/src/Level/LevelData.h
        examples/gumman3d/src/Level/LevelLoader.cpp
        examples/gumman3d/src/Level/LevelLoader.h
        examples/gumman3d/src/Scenes/GummanCommon.h
        examples/gumman3d/src/Scenes/FlubberFeature.h
        examples/gumman3d/src/Scenes/WaterFeature.h
        examples/gumman3d/src/Scenes/HoleFeature.h
        examples/gumman3d/src/Scenes/BackgroundScene.cpp
        examples/gumman3d/src/Scenes/BackgroundScene.h
        examples/gumman3d/src/Scenes/FireworksScene.cpp
        examples/gumman3d/src/Scenes/FireworksScene.h
        examples/gumman3d/src/Scenes/LevelScene.cpp
        examples/gumman3d/src/Scenes/LevelScene.h
        examples/gumman3d/src/Scenes/CheckpointScene.cpp
        examples/gumman3d/src/Scenes/CheckpointScene.h
        examples/gumman3d/src/Scenes/SpringScene.cpp
        examples/gumman3d/src/Scenes/SpringScene.h
        examples/gumman3d/src/Scenes/HazardScene.cpp
        examples/gumman3d/src/Scenes/HazardScene.h
        examples/gumman3d/src/Scenes/CrusherScene.cpp
        examples/gumman3d/src/Scenes/CrusherScene.h
        examples/gumman3d/src/Scenes/WindScene.cpp
        examples/gumman3d/src/Scenes/WindScene.h
        examples/gumman3d/src/Scenes/CoinScene.cpp
        examples/gumman3d/src/Scenes/CoinScene.h
        examples/gumman3d/src/Scenes/GemScene.cpp
        examples/gumman3d/src/Scenes/GemScene.h
        examples/gumman3d/src/Scenes/DiamondMesh.cpp
        examples/gumman3d/src/Scenes/DiamondMesh.h
        examples/gumman3d/src/Scenes/HealthScene.cpp
        examples/gumman3d/src/Scenes/HealthScene.h
        examples/gumman3d/src/Scenes/CarrotScene.cpp
        examples/gumman3d/src/Scenes/CarrotScene.h
        examples/gumman3d/src/Scenes/OneUpScene.cpp
        examples/gumman3d/src/Scenes/OneUpScene.h
        examples/gumman3d/src/Scenes/BarrelScene.cpp
        examples/gumman3d/src/Scenes/BarrelScene.h
        examples/gumman3d/src/Scenes/CrateScene.cpp
        examples/gumman3d/src/Scenes/CrateScene.h
        examples/gumman3d/src/Scenes/WallScene.cpp
        examples/gumman3d/src/Scenes/WallScene.h
        examples/gumman3d/src/Scenes/GateScene.cpp
        examples/gumman3d/src/Scenes/GateScene.h
        examples/gumman3d/src/Scenes/ShadowScene.cpp
        examples/gumman3d/src/Scenes/ShadowScene.h
        examples/gumman3d/src/Scenes/AmbientFxScene.cpp
        examples/gumman3d/src/Scenes/AmbientFxScene.h
        examples/gumman3d/src/Scenes/DecorScene.cpp
        examples/gumman3d/src/Scenes/DecorScene.h
        examples/gumman3d/src/Scenes/PipeScene.cpp
        examples/gumman3d/src/Scenes/PipeScene.h
        examples/gumman3d/src/Scenes/RopeScene.cpp
        examples/gumman3d/src/Scenes/RopeScene.h
        examples/gumman3d/src/Scenes/WaterScene.cpp
        examples/gumman3d/src/Scenes/WaterScene.h
        examples/gumman3d/src/Scenes/PlantScene.cpp
        examples/gumman3d/src/Scenes/PlantScene.h
        examples/gumman3d/src/Scenes/OxygenScene.cpp
        examples/gumman3d/src/Scenes/OxygenScene.h
        examples/gumman3d/src/Scenes/PlayerScene.cpp
        examples/gumman3d/src/Scenes/PlayerScene.h
        examples/gumman3d/src/Scenes/EnemyScene.cpp
        examples/gumman3d/src/Scenes/EnemyScene.h
        examples/gumman3d/src/Scenes/BulletScene.cpp
        examples/gumman3d/src/Scenes/BulletScene.h
        examples/gumman3d/src/Scenes/HudScene.cpp
        examples/gumman3d/src/Scenes/HudScene.h
        examples/gumman3d/src/Scenes/MenuScene.cpp
        examples/gumman3d/src/Scenes/MenuScene.h
        examples/gumman3d/src/Scenes/Gumman3dMainScene.cpp
        examples/gumman3d/src/Scenes/Gumman3dMainScene.h
)

target_link_libraries(Gumman3d PRIVATE snake3d_engine nlohmann_json::nlohmann_json)
target_compile_definitions(Gumman3d PRIVATE $<$<CONFIG:Debug>:IS_DEBUG>)

add_dependencies(Gumman3d copy_gumman_assets copy_engine_shaders)

# ============================================================================
# Rts - fourth playable prototype: a 3D real-time strategy demo.
# Links ONLY snake3d_engine (NOT snake3d_game): mouse unit selection, free
# movement + steering, fog-of-war, animated combat, AOE2-style HUD.
# ============================================================================
add_executable(Rts
        examples/rts/src/main.cpp
        examples/rts/src/RtsApp.cpp
        examples/rts/src/RtsApp.h
        examples/rts/src/Game/CombatController.cpp
        examples/rts/src/Game/CombatController.h
        examples/rts/src/Game/FogOfWar.cpp
        examples/rts/src/Game/FogOfWar.h
        examples/rts/src/Game/GroundLayersFeature.cpp
        examples/rts/src/Game/GroundLayersFeature.h
        examples/rts/src/Game/MovementController.cpp
        examples/rts/src/Game/MovementController.h
        examples/rts/src/Game/SelectionController.cpp
        examples/rts/src/Game/SelectionController.h
        examples/rts/src/Game/Unit.h
        examples/rts/src/Game/WindSwayFeature.h
        examples/rts/src/Scenes/RtsCommon.h
        examples/rts/src/Scenes/BirdScene.cpp
        examples/rts/src/Scenes/BirdScene.h
        examples/rts/src/Scenes/DeerScene.cpp
        examples/rts/src/Scenes/DeerScene.h
        examples/rts/src/Scenes/FishScene.cpp
        examples/rts/src/Scenes/FishScene.h
        examples/rts/src/Scenes/RtsMainScene.cpp
        examples/rts/src/Scenes/RtsMainScene.h
        examples/rts/src/Scenes/FogScene.cpp
        examples/rts/src/Scenes/FogScene.h
        examples/rts/src/Scenes/HudScene.cpp
        examples/rts/src/Scenes/HudScene.h
        examples/rts/src/Scenes/MenuScene.cpp
        examples/rts/src/Scenes/MenuScene.h
        examples/rts/src/Scenes/OrderMarkerScene.cpp
        examples/rts/src/Scenes/OrderMarkerScene.h
        examples/rts/src/Scenes/RadarScene.cpp
        examples/rts/src/Scenes/RadarScene.h
        examples/rts/src/Scenes/TerrainMesh.cpp
        examples/rts/src/Scenes/TerrainMesh.h
        examples/rts/src/Scenes/TerrainScene.cpp
        examples/rts/src/Scenes/TerrainScene.h
        examples/rts/src/Scenes/UnitScene.cpp
        examples/rts/src/Scenes/UnitScene.h
)

target_link_libraries(Rts PRIVATE snake3d_engine)
target_compile_definitions(Rts PRIVATE $<$<CONFIG:Debug>:IS_DEBUG>)
# Repo root so RtsApp can include the single-header stb_image (declarations only) to decode
# the attack-cursor PNG to RGBA for glfwCreateCursor; the implementation links from the engine.
target_include_directories(Rts PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

add_dependencies(Rts copy_rts_assets copy_engine_shaders)

# ============================================================================
# Fps - fifth engine-only example: a WW2 / Return-to-Castle-Wolfenstein style
# first-person shooter. Links ONLY snake3d_engine (NOT snake3d_game).
# Z-up world, capsule dynamic-body player, hitscan shooting, soldier AI, two
# maps with a loading transition.
# ============================================================================
add_executable(Fps
        examples/fps/src/main.cpp
        examples/fps/src/FpsApp.cpp
        examples/fps/src/FpsApp.h
        examples/fps/src/Game/FpsCommon.h
        examples/fps/src/Game/FpsMaterials.cpp
        examples/fps/src/Game/FpsMaterials.h
        examples/fps/src/Game/Raycaster.cpp
        examples/fps/src/Game/Raycaster.h
        examples/fps/src/Game/Soldier.h
        examples/fps/src/Level/LevelData.h
        examples/fps/src/Level/LevelLoader.cpp
        examples/fps/src/Level/LevelLoader.h
        examples/fps/src/Scenes/FpsMainScene.cpp
        examples/fps/src/Scenes/FpsMainScene.h
        examples/fps/src/Scenes/LevelScene.cpp
        examples/fps/src/Scenes/LevelScene.h
        examples/fps/src/Scenes/PlayerScene.cpp
        examples/fps/src/Scenes/PlayerScene.h
        examples/fps/src/Scenes/WeaponController.cpp
        examples/fps/src/Scenes/WeaponController.h
        examples/fps/src/Scenes/HudScene.cpp
        examples/fps/src/Scenes/HudScene.h
        examples/fps/src/Scenes/EnemyScene.cpp
        examples/fps/src/Scenes/EnemyScene.h
        examples/fps/src/Scenes/ImpactScene.cpp
        examples/fps/src/Scenes/ImpactScene.h
        examples/fps/src/Scenes/FpsWeatherScene.cpp
        examples/fps/src/Scenes/FpsWeatherScene.h
        examples/fps/src/Scenes/PickupScene.cpp
        examples/fps/src/Scenes/PickupScene.h
        examples/fps/src/Scenes/GoalScene.cpp
        examples/fps/src/Scenes/GoalScene.h
        examples/fps/src/Scenes/PreloaderScene.cpp
        examples/fps/src/Scenes/PreloaderScene.h
)
target_link_libraries(Fps PRIVATE snake3d_engine nlohmann_json::nlohmann_json)
target_compile_definitions(Fps PRIVATE $<$<CONFIG:Debug>:IS_DEBUG>)
add_dependencies(Fps copy_fps_assets copy_engine_shaders)

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
add_dependencies(Tests copy_engine_shaders)

include(CTest)
include(Catch)
catch_discover_tests(Tests)

# ============================================================================
# Engine dev tools (opt-in: -DBUILD_ENGINE_TOOLS=ON). Standalone offline
# utilities that link assimp but NOT the engine runtime — kept out of normal
# builds so they don't slow them down.
#   bake_animation — bake a skinned animation to a per-frame OBJ flipbook.
# ============================================================================
option(BUILD_ENGINE_TOOLS "Build engine offline dev tools (bake_animation, ...)" OFF)
if (BUILD_ENGINE_TOOLS)
    add_executable(bake_animation engine/tools/bake_animation.cpp)
    target_link_libraries(bake_animation PRIVATE assimp)
    target_compile_features(bake_animation PRIVATE cxx_std_17)
endif()
