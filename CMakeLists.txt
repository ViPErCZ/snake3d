cmake_minimum_required(VERSION 3.22)
project(Snake3)

set(CMAKE_CXX_STANDARD 23)

find_package(glfw3 3.1 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(OpenAL REQUIRED)
find_package(assimp 5.2 REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

include_directories(/usr/include/freetype2)
include_directories(/usr/include/AL)

add_custom_target(copy_assets ALL
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/Assets
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Assets ${CMAKE_BINARY_DIR}/Assets
        COMMENT "Cleaning and copying Assets folder"
)

add_executable(Snake3 main.cpp ItemsDto/BaseItem.cpp ItemsDto/BaseItem.h stdafx.h App.cpp App.h Manager/ResourceManager.cpp Manager/ResourceManager.h Renderer/Opengl/BaseRenderer.cpp Renderer/Opengl/BaseRenderer.h Manager/RenderManager.cpp Manager/RenderManager.h Manager/KeyboardManager.cpp Manager/KeyboardManager.h Handler/SnakeMoveHandler.cpp Handler/SnakeMoveHandler.h Handler/BaseHandler.cpp Handler/BaseHandler.h Physic/CollisionDetector.cpp Physic/CollisionDetector.h Manager/EatManager.cpp Manager/EatManager.h Handler/EatLocationHandler.cpp Handler/EatLocationHandler.h Handler/RadarHandler.cpp Handler/RadarHandler.h Handler/BaseKeydownHandle.h Manager/ShaderManager.cpp Manager/ShaderManager.h Manager/LevelManager.cpp Manager/LevelManager.h Manager/VboIndexer.cpp Manager/VboIndexer.h Manager/Camera.cpp Manager/Camera.h Renderer/Opengl/Model/Character.cpp Renderer/Opengl/Model/Character.h Renderer/Opengl/Model/Utils/Vao.cpp Renderer/Opengl/Model/Utils/Vao.h Renderer/Opengl/Model/Utils/Vbo.cpp Renderer/Opengl/Model/Utils/Vbo.h Renderer/Opengl/Model/Utils/Ebo.cpp Renderer/Opengl/Model/Utils/Ebo.h Renderer/Opengl/Model/Utils/Mesh.cpp Renderer/Opengl/Model/Utils/Mesh.h Resource/ObjModelLoader.cpp Resource/ObjModelLoader.h Resource/TextureLoader.cpp Resource/TextureLoader.h Resource/ShaderLoader.cpp Resource/ShaderLoader.h Manager/TextureManager.cpp Manager/TextureManager.h Renderer/Opengl/DepthMapRenderer.cpp Renderer/Opengl/DepthMapRenderer.h Renderer/Opengl/Line.h Renderer/Opengl/BloomRenderer.cpp Renderer/Opengl/BloomRenderer.h Renderer/Opengl/Model/RainModel.cpp Renderer/Opengl/Model/RainModel.h Renderer/Opengl/RainDropRenderer.cpp Renderer/Opengl/RainDropRenderer.h Renderer/Opengl/Model/MeshModel.cpp Renderer/Opengl/Model/MeshModel.h Resource/AnimLoader.cpp Resource/AnimLoader.h ItemsDto/AnimItem.cpp ItemsDto/AnimItem.h Renderer/Opengl/Model/Utils/Tree.h
        Particle/FireParticleSystem.cpp
        Particle/FireParticleSystem.h
        Particle/SmokeParticleSystem.cpp
        Particle/SmokeParticleSystem.h
        Renderer/Opengl/FireRenderer.cpp
        Renderer/Opengl/FireRenderer.h
        Renderer/Opengl/PlanarReflectionRenderer.cpp
        Renderer/Opengl/PlanarReflectionRenderer.h
        Renderer/Opengl/BoltRenderer.cpp
        Renderer/Opengl/BoltRenderer.h
        Renderer/Opengl/Effects/LightningFlashEffect.cpp
        Renderer/Opengl/Effects/LightningFlashEffect.h
        Handler/Debug/PositionHandler.cpp
        Handler/Debug/PositionHandler.h
        Renderer/Opengl/Model/Standard/StandardMesh.cpp
        Renderer/Opengl/Model/Standard/StandardMesh.h
        Renderer/Opengl/Model/Standard/PlaneMesh.cpp
        Renderer/Opengl/Model/Standard/PlaneMesh.h
        Renderer/Opengl/Model/Standard/BoxMesh.cpp
        Renderer/Opengl/Model/Standard/BoxMesh.h
        Renderer/Opengl/Model/Standard/CylinderMesh.cpp
        Renderer/Opengl/Model/Standard/CylinderMesh.h
        Renderer/Opengl/Node3DRenderer.cpp
        Renderer/Opengl/Node3DRenderer.h
        Renderer/Opengl/Material/StandardMaterial.cpp
        Renderer/Opengl/Material/StandardMaterial.h
        Renderer/Opengl/Material/BaseMaterial.h
        Renderer/Opengl/Material/PlanarReflectionMaterial.cpp
        Renderer/Opengl/Material/PlanarReflectionMaterial.h
        Renderer/Opengl/Material/ShaderMaterial.cpp
        Renderer/Opengl/Material/ShaderMaterial.h
        Renderer/Opengl/Material/IAlbedoMaterial.h
        Tools/Timer.cpp
        Tools/Timer.h
        Tools/WorldEnvironment.cpp
        Tools/WorldEnvironment.h
        Tools/Environment.cpp
        Tools/Environment.h
        Lights/DirectionalLight.cpp
        Lights/DirectionalLight.h
        Lights/SpotLight.cpp
        Lights/SpotLight.h
        Tools/Clear.h
        ItemsDto/Transform.cpp
        ItemsDto/Transform.h
        Lights/PointLight.cpp
        Lights/PointLight.h
        Renderer/Opengl/Model/Standard/SphereMesh.cpp
        Renderer/Opengl/Model/Standard/SphereMesh.h
        Renderer/Opengl/Model/Standard/CapsuleMesh.cpp
        Renderer/Opengl/Model/Standard/CapsuleMesh.h
        Renderer/Opengl/Model/Standard/ArrayMesh.cpp
        Renderer/Opengl/Model/Standard/ArrayMesh.h
        Renderer/Opengl/Model/Standard/AnimationArrayMesh.cpp
        Renderer/Opengl/Model/Standard/AnimationArrayMesh.h
        Renderer/Opengl/Model/Standard/Animation/AnimationPlayer.cpp
        Renderer/Opengl/Model/Standard/Animation/AnimationPlayer.h
        Renderer/Opengl/Scene/Scene.cpp
        Renderer/Opengl/Scene/Scene.h
        Renderer/Opengl/Scene/SceneRenderer.cpp
        Renderer/Opengl/Scene/SceneRenderer.h
        Renderer/Opengl/Material/IUniform.cpp
        Renderer/Opengl/Material/IUniform.h
        Renderer/Opengl/Material/Uniform/TextureUniform.cpp
        Renderer/Opengl/Material/Uniform/TextureUniform.h
        Renderer/Opengl/Material/Uniform/TextureArrayUniform.cpp
        Renderer/Opengl/Material/Uniform/TextureArrayUniform.h
        Resource/ResourceLoader.cpp
        Resource/ResourceLoader.h
        Renderer/Opengl/Model/SpinnerMesh.cpp
        Renderer/Opengl/Model/SpinnerMesh.h
        Scenes/MainScene.cpp
        Scenes/MainScene.h
        Renderer/Opengl/Model/Standard/MeshNode3D.cpp
        Renderer/Opengl/Model/Standard/MeshNode3D.h
        Scenes/PlayerScene.cpp
        Scenes/PlayerScene.h
        Renderer/Opengl/Model/Game/SnakeMeshNode3D.cpp
        Renderer/Opengl/Model/Game/SnakeMeshNode3D.h
        ItemsDto/Vector3i.cpp
        ItemsDto/Vector3i.h
        Renderer/Opengl/Model/Game/CoinMeshNode3D.cpp
        Renderer/Opengl/Model/Game/CoinMeshNode3D.h
        ItemsDto/Visibility.cpp
        ItemsDto/Visibility.h
        Renderer/Opengl/Model/Standard/2D/LabelNode2D.cpp
        Renderer/Opengl/Model/Standard/2D/LabelNode2D.h
        Renderer/Opengl/Model/Standard/2D/BaseNode2D.cpp
        Renderer/Opengl/Model/Standard/2D/BaseNode2D.h
        Renderer/Opengl/Material/2D/LabelSettings.cpp
        Renderer/Opengl/Material/2D/LabelSettings.h
        Renderer/Opengl/Material/2D/Font.cpp
        Renderer/Opengl/Material/2D/Font.h
        Renderer/Opengl/Node2DRenderer.cpp
        Renderer/Opengl/Node2DRenderer.h
        Renderer/Opengl/Model/Standard/2D/MeshNode2D.cpp
        Renderer/Opengl/Model/Standard/2D/MeshNode2D.h
        Renderer/Opengl/Model/Utils/TextMesh.cpp
        Renderer/Opengl/Model/Utils/TextMesh.h
        Renderer/Opengl/Material/Uniform/FadeOutUniform.cpp
        Renderer/Opengl/Material/Uniform/FadeOutUniform.h
        Renderer/Opengl/Material/Uniform/FadeInUniform.cpp
        Renderer/Opengl/Material/Uniform/FadeInUniform.h
        Renderer/Opengl/Model/Standard/2D/QuadNode2D.cpp
        Renderer/Opengl/Model/Standard/2D/QuadNode2D.h
        Renderer/Opengl/Model/Utils/Mesh2D.cpp
        Renderer/Opengl/Model/Utils/Mesh2D.h
        Renderer/Opengl/Model/Game/RadarMeshNode2D.cpp
        Renderer/Opengl/Model/Game/RadarMeshNode2D.h
        Renderer/Opengl/Material/Uniform/TimerUniform.cpp
        Renderer/Opengl/Material/Uniform/TimerUniform.h
        Scenes/BarriersScene.cpp
        Scenes/BarriersScene.h
        Renderer/Opengl/Model/Game/RadarItem.cpp
        Renderer/Opengl/Model/Game/RadarItem.h
        Scenes/CoinScene.cpp
        Scenes/CoinScene.h
        Tools/Capabilities.h
        Tools/BlendFactor.h
        Tools/Blending.h
        Tools/ContextState.cpp
        Tools/ContextState.h
        Scenes/PreloaderScene.cpp
        Scenes/PreloaderScene.h
        Renderer/Opengl/Model/Standard/2D/TringleNode2D.cpp
        Renderer/Opengl/Model/Standard/2D/TringleNode2D.h
        Renderer/Opengl/Model/Standard/TringleMesh3D.cpp
        Renderer/Opengl/Model/Standard/TringleMesh3D.h
        Renderer/Opengl/Model/Standard/QuadMesh3D.cpp
        Renderer/Opengl/Model/Standard/QuadMesh3D.h
        Renderer/Opengl/Model/Standard/GPUParticle3D.cpp
        Renderer/Opengl/Model/Standard/GPUParticle3D.h
        Renderer/Opengl/Model/Standard/2D/GPUParticle2D.cpp
        Renderer/Opengl/Model/Standard/2D/GPUParticle2D.h
        Renderer/Opengl/Material/Particle/BaseProcessMaterial.cpp
        Renderer/Opengl/Material/Particle/BaseProcessMaterial.h
        Renderer/Opengl/Material/Particle/ParticleProcessMaterial.cpp
        Renderer/Opengl/Material/Particle/ParticleProcessMaterial.h
        Renderer/Opengl/Material/Interface/BlendingInterface.cpp
        Renderer/Opengl/Material/Interface/BlendingInterface.h
        Tools/DepthFunc.h
        Scenes/TorchScene.cpp
        Scenes/TorchScene.h
        Scenes/WeatherScene.cpp
        Scenes/WeatherScene.h
        Renderer/Opengl/Model/Standard/WireframeArrowMesh.cpp
        Renderer/Opengl/Model/Standard/WireframeArrowMesh.h
        Renderer/Opengl/Model/Debug/LightNode3D.cpp
        Renderer/Opengl/Model/Debug/LightNode3D.h
        Renderer/Opengl/Model/Debug/DirectionalLightNode3D.cpp
        Renderer/Opengl/Model/Debug/DirectionalLightNode3D.h
        Tools/DrawElement.h
        Renderer/Opengl/Model/Standard/SkyboxNode3D.cpp
        Renderer/Opengl/Model/Standard/SkyboxNode3D.h
        Renderer/Opengl/Material/Uniform/CallbackUniform.cpp
        Renderer/Opengl/Material/Uniform/CallbackUniform.h
        Renderer/Opengl/Model/Game/StreetLampNode3D.cpp
        Renderer/Opengl/Model/Game/StreetLampNode3D.h
        Physic/Shape.h
        Physic/BoxShape.cpp
        Physic/BoxShape.h
        Renderer/Opengl/Model/Game/BarrelNode3D.cpp
        Renderer/Opengl/Model/Game/BarrelNode3D.h
)
target_link_libraries(${PROJECT_NAME} glfw glut GLU GL GLEW freetype openal alut assimp nlohmann_json::nlohmann_json)

file(COPY Assets DESTINATION ${CMAKE_BINARY_DIR})

### TESTING #################################################
# Povolení testování
enable_testing()

# --- FetchContent pro Catch2 ---
include(FetchContent)
FetchContent_Declare(
        catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.4.0 # Zvažte použití novější verze, např. v3.6.0
)
FetchContent_MakeAvailable(catch2)

# --- Zdrojové soubory APLIKACE ---
# Definujte zde všechny .cpp soubory vaší hry (mimo složku Tests/)
# POZNÁMKA: Hlavičkové soubory (.h) se sem nepíšou!
set(APP_SOURCES
        ItemsDto/BaseItem.cpp
        Manager/ResourceManager.cpp
        Manager/RenderManager.cpp
        Manager/KeyboardManager.cpp
        Manager/EatManager.cpp
        Manager/ShaderManager.cpp
        Manager/LevelManager.cpp
        Manager/VboIndexer.cpp
        Manager/Camera.cpp
        Manager/TextureManager.cpp
        ItemsDto/AnimItem.cpp
        Renderer/Opengl/BaseRenderer.cpp
        Renderer/Opengl/BloomRenderer.cpp
        Renderer/Opengl/RainDropRenderer.cpp
        Renderer/Opengl/PlanarReflectionRenderer.cpp
        Renderer/Opengl/PlanarReflectionRenderer.h
        Renderer/Opengl/Model/Character.cpp
        Renderer/Opengl/Model/Utils/Vao.cpp
        Renderer/Opengl/Model/Utils/Vbo.cpp
        Renderer/Opengl/Model/Utils/Ebo.cpp
        Renderer/Opengl/Model/Utils/Mesh.cpp
        Renderer/Opengl/Model/RainModel.cpp
        Renderer/Opengl/Model/MeshModel.cpp
        Handler/SnakeMoveHandler.cpp
        Handler/BaseHandler.cpp
        Handler/EatLocationHandler.cpp
        Handler/RadarHandler.cpp
        Physic/CollisionDetector.cpp
        Resource/ObjModelLoader.cpp
        Resource/TextureLoader.cpp
        Resource/ShaderLoader.cpp
        Resource/AnimLoader.cpp
        Handler/Debug/PositionHandler.cpp
        Handler/Debug/PositionHandler.h
        Renderer/Opengl/Model/Standard/BoxMesh.cpp
        Renderer/Opengl/Model/Standard/BoxMesh.h
        Renderer/Opengl/Model/Standard/CapsuleMesh.cpp
        Renderer/Opengl/Model/Standard/CapsuleMesh.h
        Renderer/Opengl/Model/Standard/CylinderMesh.cpp
        Renderer/Opengl/Model/Standard/CylinderMesh.h
        Renderer/Opengl/Model/Standard/PlaneMesh.cpp
        Renderer/Opengl/Model/Standard/PlaneMesh.h
        Renderer/Opengl/Model/Standard/StandardMesh.cpp
        Renderer/Opengl/Model/Standard/StandardMesh.h
        Renderer/Opengl/Node3DRenderer.cpp
        Renderer/Opengl/Node3DRenderer.h
        Renderer/Opengl/Material/StandardMaterial.cpp
        Renderer/Opengl/Material/StandardMaterial.h
        Renderer/Opengl/Material/BaseMaterial.h
        Renderer/Opengl/Material/PlanarReflectionMaterial.cpp
        Renderer/Opengl/Material/PlanarReflectionMaterial.h
        Renderer/Opengl/Material/ShaderMaterial.cpp
        Renderer/Opengl/Material/ShaderMaterial.h
        Renderer/Opengl/Material/IAlbedoMaterial.h
        Tools/Timer.cpp
        Tools/Timer.h
        Tools/WorldEnvironment.cpp
        Tools/WorldEnvironment.h
        Tools/Environment.cpp
        Tools/Environment.h
        Lights/DirectionalLight.cpp
        Lights/DirectionalLight.h
        Lights/SpotLight.cpp
        Lights/SpotLight.h
        Tools/Clear.h
        ItemsDto/Transform.cpp
        ItemsDto/Transform.h
        Lights/PointLight.cpp
        Lights/PointLight.h
        Renderer/Opengl/Model/Standard/SphereMesh.cpp
        Renderer/Opengl/Model/Standard/SphereMesh.h
        Renderer/Opengl/Model/Standard/ArrayMesh.cpp
        Renderer/Opengl/Model/Standard/ArrayMesh.h
        Renderer/Opengl/Model/Standard/AnimationArrayMesh.cpp
        Renderer/Opengl/Model/Standard/AnimationArrayMesh.h
        Renderer/Opengl/Model/Standard/Animation/AnimationPlayer.cpp
        Renderer/Opengl/Model/Standard/Animation/AnimationPlayer.h
        Renderer/Opengl/Scene/Scene.cpp
        Renderer/Opengl/Scene/Scene.h
        Renderer/Opengl/Material/IUniform.cpp
        Renderer/Opengl/Material/IUniform.h
        Renderer/Opengl/Material/Uniform/TextureUniform.cpp
        Renderer/Opengl/Material/Uniform/TextureUniform.h
        Renderer/Opengl/Material/Uniform/TextureArrayUniform.cpp
        Renderer/Opengl/Material/Uniform/TextureArrayUniform.h
        Resource/ResourceLoader.cpp
        Resource/ResourceLoader.h
        Renderer/Opengl/Model/SpinnerMesh.cpp
        Renderer/Opengl/Model/SpinnerMesh.h
        Scenes/MainScene.cpp
        Scenes/MainScene.h
        Renderer/Opengl/Scene/SceneRenderer.cpp
        Renderer/Opengl/Scene/SceneRenderer.h
        Renderer/Opengl/Model/Standard/MeshNode3D.cpp
        Renderer/Opengl/Model/Standard/MeshNode3D.h
        Scenes/PlayerScene.cpp
        Scenes/PlayerScene.h
        Renderer/Opengl/Model/Game/SnakeMeshNode3D.cpp
        Renderer/Opengl/Model/Game/SnakeMeshNode3D.h
        ItemsDto/Vector3i.cpp
        ItemsDto/Vector3i.h
        Renderer/Opengl/Model/Game/CoinMeshNode3D.cpp
        Renderer/Opengl/Model/Game/CoinMeshNode3D.h
        ItemsDto/Visibility.cpp
        ItemsDto/Visibility.h
        Renderer/Opengl/Model/Standard/2D/LabelNode2D.cpp
        Renderer/Opengl/Model/Standard/2D/LabelNode2D.h
        Renderer/Opengl/Model/Standard/2D/BaseNode2D.cpp
        Renderer/Opengl/Model/Standard/2D/BaseNode2D.h
        Renderer/Opengl/Material/2D/LabelSettings.cpp
        Renderer/Opengl/Material/2D/LabelSettings.h
        Renderer/Opengl/Material/2D/Font.cpp
        Renderer/Opengl/Material/2D/Font.h
        Renderer/Opengl/Node2DRenderer.cpp
        Renderer/Opengl/Node2DRenderer.h
        Renderer/Opengl/Model/Standard/2D/MeshNode2D.cpp
        Renderer/Opengl/Model/Standard/2D/MeshNode2D.h
        Renderer/Opengl/Model/Utils/TextMesh.cpp
        Renderer/Opengl/Model/Utils/TextMesh.h
        Renderer/Opengl/Material/Uniform/FadeOutUniform.cpp
        Renderer/Opengl/Material/Uniform/FadeOutUniform.h
        Renderer/Opengl/Material/Uniform/FadeInUniform.cpp
        Renderer/Opengl/Material/Uniform/FadeInUniform.h
        Renderer/Opengl/Model/Standard/2D/QuadNode2D.cpp
        Renderer/Opengl/Model/Standard/2D/QuadNode2D.h
        Renderer/Opengl/Model/Utils/Mesh2D.cpp
        Renderer/Opengl/Model/Utils/Mesh2D.h
        Renderer/Opengl/Model/Game/RadarMeshNode2D.cpp
        Renderer/Opengl/Model/Game/RadarMeshNode2D.h
        Renderer/Opengl/Material/Uniform/TimerUniform.cpp
        Renderer/Opengl/Material/Uniform/TimerUniform.h
        Scenes/BarriersScene.cpp
        Scenes/BarriersScene.h
        Renderer/Opengl/Model/Game/RadarItem.cpp
        Renderer/Opengl/Model/Game/RadarItem.h
        Scenes/CoinScene.cpp
        Scenes/CoinScene.h
        Tools/Capabilities.h
        Tools/BlendFactor.h
        Tools/Blending.h
        Tools/ContextState.cpp
        Tools/ContextState.h
        Scenes/PreloaderScene.cpp
        Scenes/PreloaderScene.h
        Renderer/Opengl/Model/Standard/2D/TringleNode2D.cpp
        Renderer/Opengl/Model/Standard/2D/TringleNode2D.h
        Renderer/Opengl/Model/Standard/TringleMesh3D.cpp
        Renderer/Opengl/Model/Standard/TringleMesh3D.h
        Renderer/Opengl/Model/Standard/QuadMesh3D.cpp
        Renderer/Opengl/Model/Standard/QuadMesh3D.h
        Renderer/Opengl/Model/Standard/GPUParticle3D.cpp
        Renderer/Opengl/Model/Standard/GPUParticle3D.h
        Renderer/Opengl/Model/Standard/2D/GPUParticle2D.cpp
        Renderer/Opengl/Model/Standard/2D/GPUParticle2D.h
        Renderer/Opengl/Material/Particle/BaseProcessMaterial.cpp
        Renderer/Opengl/Material/Particle/BaseProcessMaterial.h
        Renderer/Opengl/Material/Particle/ParticleProcessMaterial.cpp
        Renderer/Opengl/Material/Particle/ParticleProcessMaterial.h
        Renderer/Opengl/Material/Interface/BlendingInterface.cpp
        Renderer/Opengl/Material/Interface/BlendingInterface.h
        Tools/DepthFunc.h
        Scenes/TorchScene.cpp
        Scenes/TorchScene.h
        Scenes/WeatherScene.cpp
        Scenes/WeatherScene.h
        Renderer/Opengl/Model/Standard/WireframeArrowMesh.cpp
        Renderer/Opengl/Model/Standard/WireframeArrowMesh.h
        Renderer/Opengl/Model/Debug/LightNode3D.cpp
        Renderer/Opengl/Model/Debug/LightNode3D.h
        Renderer/Opengl/Model/Debug/DirectionalLightNode3D.cpp
        Renderer/Opengl/Model/Debug/DirectionalLightNode3D.h
        Tools/DrawElement.h
        Renderer/Opengl/Model/Standard/SkyboxNode3D.cpp
        Renderer/Opengl/Model/Standard/SkyboxNode3D.h
        Renderer/Opengl/Material/Uniform/CallbackUniform.cpp
        Renderer/Opengl/Material/Uniform/CallbackUniform.h
        Renderer/Opengl/Model/Game/StreetLampNode3D.cpp
        Renderer/Opengl/Model/Game/StreetLampNode3D.h
        Physic/Shape.h
        Physic/BoxShape.cpp
        Physic/BoxShape.h
        Renderer/Opengl/Model/Game/BarrelNode3D.cpp
        Renderer/Opengl/Model/Game/BarrelNode3D.h
)

# --- Vytvoření KNIHOVNY z herních souborů ---
add_library(snake3d_lib ${APP_SOURCES}
        Renderer/Opengl/PlanarReflectionRenderer.cpp
        Renderer/Opengl/PlanarReflectionRenderer.h
        Renderer/Opengl/Material/PlanarReflectionMaterial.cpp
        Renderer/Opengl/Material/PlanarReflectionMaterial.h
        Renderer/Opengl/Model/Standard/BoxMesh.cpp
        Renderer/Opengl/Model/Standard/BoxMesh.h
        Renderer/Opengl/Model/Standard/StandardMesh.cpp
        Renderer/Opengl/Model/Standard/StandardMesh.h
        Renderer/Opengl/Node3DRenderer.cpp
        Renderer/Opengl/Node3DRenderer.h
        Renderer/Opengl/Material/StandardMaterial.cpp
        Renderer/Opengl/Material/StandardMaterial.h
        Renderer/Opengl/Material/BaseMaterial.h
        Renderer/Opengl/Material/ShaderMaterial.cpp
        Renderer/Opengl/Material/ShaderMaterial.h
        Renderer/Opengl/Material/IAlbedoMaterial.h
        Tools/Timer.cpp
        Tools/Timer.h
        Tools/WorldEnvironment.cpp
        Tools/WorldEnvironment.h
        Tools/Environment.cpp
        Tools/Environment.h
        Lights/DirectionalLight.cpp
        Lights/DirectionalLight.h
        Lights/SpotLight.cpp
        Lights/SpotLight.h
        Tools/Clear.h
        ItemsDto/Transform.cpp
        ItemsDto/Transform.h
        Lights/PointLight.cpp
        Lights/PointLight.h
        Renderer/Opengl/Model/Standard/SphereMesh.cpp
        Renderer/Opengl/Model/Standard/SphereMesh.h
        Renderer/Opengl/Model/Standard/CapsuleMesh.cpp
        Renderer/Opengl/Model/Standard/CapsuleMesh.h
        Renderer/Opengl/Model/Standard/ArrayMesh.cpp
        Renderer/Opengl/Model/Standard/ArrayMesh.h
        Renderer/Opengl/Model/Standard/AnimationArrayMesh.cpp
        Renderer/Opengl/Model/Standard/AnimationArrayMesh.h
        Renderer/Opengl/Model/Standard/Animation/AnimationPlayer.cpp
        Renderer/Opengl/Model/Standard/Animation/AnimationPlayer.h
        Renderer/Opengl/Model/Standard/CylinderMesh.cpp
        Renderer/Opengl/Model/Standard/CylinderMesh.h
        Renderer/Opengl/Scene/Scene.cpp
        Renderer/Opengl/Scene/Scene.h
        Renderer/Opengl/Material/IUniform.cpp
        Renderer/Opengl/Material/IUniform.h
        Renderer/Opengl/Material/Uniform/TextureUniform.cpp
        Renderer/Opengl/Material/Uniform/TextureUniform.h
        Renderer/Opengl/Material/Uniform/TextureArrayUniform.cpp
        Renderer/Opengl/Material/Uniform/TextureArrayUniform.h
        Resource/ResourceLoader.cpp
        Resource/ResourceLoader.h
        Renderer/Opengl/Model/SpinnerMesh.cpp
        Renderer/Opengl/Model/SpinnerMesh.h
        Scenes/MainScene.cpp
        Scenes/MainScene.h
        Renderer/Opengl/Model/Standard/MeshNode3D.cpp
        Renderer/Opengl/Model/Standard/MeshNode3D.h
        Scenes/PlayerScene.cpp
        Scenes/PlayerScene.h
        Renderer/Opengl/Model/Game/SnakeMeshNode3D.cpp
        Renderer/Opengl/Model/Game/SnakeMeshNode3D.h
        ItemsDto/Vector3i.cpp
        ItemsDto/Vector3i.h
        Renderer/Opengl/Model/Game/CoinMeshNode3D.cpp
        Renderer/Opengl/Model/Game/CoinMeshNode3D.h
        ItemsDto/Visibility.cpp
        ItemsDto/Visibility.h
        Renderer/Opengl/Model/Standard/2D/LabelNode2D.cpp
        Renderer/Opengl/Model/Standard/2D/LabelNode2D.h
        Renderer/Opengl/Model/Standard/2D/BaseNode2D.cpp
        Renderer/Opengl/Model/Standard/2D/BaseNode2D.h
        Renderer/Opengl/Material/2D/LabelSettings.cpp
        Renderer/Opengl/Material/2D/LabelSettings.h
        Renderer/Opengl/Material/2D/Font.cpp
        Renderer/Opengl/Material/2D/Font.h
        Renderer/Opengl/Node2DRenderer.cpp
        Renderer/Opengl/Node2DRenderer.h
        Renderer/Opengl/Model/Standard/2D/MeshNode2D.cpp
        Renderer/Opengl/Model/Standard/2D/MeshNode2D.h
        Renderer/Opengl/Model/Utils/TextMesh.cpp
        Renderer/Opengl/Model/Utils/TextMesh.h
        Renderer/Opengl/Material/Uniform/FadeOutUniform.cpp
        Renderer/Opengl/Material/Uniform/FadeOutUniform.h
        Renderer/Opengl/Material/Uniform/FadeInUniform.cpp
        Renderer/Opengl/Material/Uniform/FadeInUniform.h
        Renderer/Opengl/Model/Standard/2D/QuadNode2D.cpp
        Renderer/Opengl/Model/Standard/2D/QuadNode2D.h
        Renderer/Opengl/Model/Utils/Mesh2D.cpp
        Renderer/Opengl/Model/Utils/Mesh2D.h
        Renderer/Opengl/Model/Game/RadarMeshNode2D.cpp
        Renderer/Opengl/Model/Game/RadarMeshNode2D.h
        Renderer/Opengl/Material/Uniform/TimerUniform.cpp
        Renderer/Opengl/Material/Uniform/TimerUniform.h
        Scenes/BarriersScene.cpp
        Scenes/BarriersScene.h
        Renderer/Opengl/Model/Game/RadarItem.cpp
        Renderer/Opengl/Model/Game/RadarItem.h
        Scenes/CoinScene.cpp
        Scenes/CoinScene.h
        Tools/Capabilities.h
        Tools/BlendFactor.h
        Tools/Blending.h
        Tools/ContextState.cpp
        Tools/ContextState.h
        Scenes/PreloaderScene.cpp
        Scenes/PreloaderScene.h
        Renderer/Opengl/Model/Standard/2D/TringleNode2D.cpp
        Renderer/Opengl/Model/Standard/2D/TringleNode2D.h
        Renderer/Opengl/Model/Standard/TringleMesh3D.cpp
        Renderer/Opengl/Model/Standard/TringleMesh3D.h
        Renderer/Opengl/Model/Standard/QuadMesh3D.cpp
        Renderer/Opengl/Model/Standard/QuadMesh3D.h
        Renderer/Opengl/Model/Standard/GPUParticle3D.cpp
        Renderer/Opengl/Model/Standard/GPUParticle3D.h
        Renderer/Opengl/Model/Standard/2D/GPUParticle2D.cpp
        Renderer/Opengl/Model/Standard/2D/GPUParticle2D.h
        Renderer/Opengl/Material/Particle/BaseProcessMaterial.cpp
        Renderer/Opengl/Material/Particle/BaseProcessMaterial.h
        Renderer/Opengl/Material/Particle/ParticleProcessMaterial.cpp
        Renderer/Opengl/Material/Particle/ParticleProcessMaterial.h
        Renderer/Opengl/Material/Interface/BlendingInterface.cpp
        Renderer/Opengl/Material/Interface/BlendingInterface.h
        Tools/DepthFunc.h
        Scenes/TorchScene.cpp
        Scenes/TorchScene.h
        Scenes/WeatherScene.cpp
        Scenes/WeatherScene.h
        Renderer/Opengl/Model/Standard/WireframeArrowMesh.cpp
        Renderer/Opengl/Model/Standard/WireframeArrowMesh.h
        Renderer/Opengl/Model/Debug/LightNode3D.cpp
        Renderer/Opengl/Model/Debug/LightNode3D.h
        Renderer/Opengl/Model/Debug/DirectionalLightNode3D.cpp
        Renderer/Opengl/Model/Debug/DirectionalLightNode3D.h
        Tools/DrawElement.h
        Renderer/Opengl/Model/Standard/SkyboxNode3D.cpp
        Renderer/Opengl/Model/Standard/SkyboxNode3D.h
        Renderer/Opengl/Material/Uniform/CallbackUniform.cpp
        Renderer/Opengl/Material/Uniform/CallbackUniform.h
        Renderer/Opengl/Model/Game/StreetLampNode3D.cpp
        Renderer/Opengl/Model/Game/StreetLampNode3D.h
        Physic/Shape.h
        Physic/BoxShape.cpp
        Physic/BoxShape.h
        Renderer/Opengl/Model/Game/BarrelNode3D.cpp
        Renderer/Opengl/Model/Game/BarrelNode3D.h
)

# Přidání cest k hlavičkovým souborům pro vaši knihovnu
# PUBLIC znamená, že kdokoliv, kdo se nalinkuje na snake3d_lib, také dostane tyto cesty
target_include_directories(snake3d_lib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        # Můžete explicitně vyjmenovat složky, pokud jich není moc
        # např. ${CMAKE_CURRENT_SOURCE_DIR}/Manager
)

# Linkování knihovny proti jejím závislostem (GLFW, OpenGL, atd.)
target_link_libraries(snake3d_lib PUBLIC
        glfw glut GLU GL GLEW freetype assimp
)


# --- Zdrojové soubory TESTŮ ---
# Použijte GLOB pouze pro testy, pokud chcete, ale lepší je vypsat je
file(GLOB TEST_SOURCES "Tests/*.cpp")

# --- Vytvoření spustitelného souboru pro TESTY ---
add_executable(Tests ${TEST_SOURCES})

# --- Linkování TESTŮ proti herní knihovně a Catch2 ---
# Použijte Catch2::Catch2WithMain, aby Catch2 automaticky vytvořil funkci main().
# Je to jednodušší a spolehlivější.
target_link_libraries(Tests PRIVATE
        snake3d_lib
        Catch2::Catch2WithMain
)

# --- Integrace CTest ---
include(CTest)
include(Catch)
catch_discover_tests(Tests)
#target_include_directories(Tests PRIVATE ${catch2_SOURCE_DIR}/single_include)