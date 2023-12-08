cmake_minimum_required (VERSION 3.12)

set(ALL_FILES
    "Core/Game.cpp"
    "Core/Game.h" 
    "Core/Events.cpp"
    "Core/Events.h"
    "Core/Renderer.cpp"
    "Core/Renderer.h"
    "Core/Settings.cpp"
    "Core/Settings.h"
    "Core/Window.cpp"
    "Core/Window.h"

    "SceneManagement/SceneManager.h"
    "SceneManagement/SceneManager.cpp"
    "SceneManagement/BaseScene.h"
    "SceneManagement/IGrid.h"
    "SceneManagement/IsometricScene.cpp"
    "SceneManagement/IsometricScene.h"

    "Core/Surface.cpp"
    "Core/Surface.h"
    "Core/Texture.cpp"
    "Core/Texture.h"
    "Core/Font.cpp"
    "Core/Font.h"

    "Core/Timer.h"

    "EntityComponentSystem/Components.h"
    "EntityComponentSystem/Entity.h"
    "EntityComponentSystem/EntityManager.h"
    "EntityComponentSystem/EntityMemoryPool.h"
    "EntityComponentSystem/TupleHelper.h"
    "EntityComponentSystem/ComponentHelper.h"
    "EntityComponentSystem/Systems/BaseSystem.h" 
    "EntityComponentSystem/Systems/EditorSystem.h" 
    "EntityComponentSystem/Systems/EditorSystem.cpp" 
    "EntityComponentSystem/Systems/MovementSystem.cpp"
    "EntityComponentSystem/Systems/MovementSystem.h"

    "Commands/Command.h"
    "Commands/CreateEntityCommand.h" 
    "Commands/CreateEntityCommand.cpp" 
    "Commands/DeleteEntityCommand.cpp" 
    "Commands/DeleteEntityCommand.h" 
    "Commands/UndoManager.h"
    "Commands/UndoManager.cpp" 
     
    "Pathfinding/NavigationGraph.h" 
     
    "Pathfinding/NavigationGraph.cpp"

    "Collision/Intersections.h"
    "Collision/Intersections.cpp" 

    "Maths/Vector2.h" 
    "Maths/Rectangle.h" 
    
    "Editor/TileAtlas.h"
    "Editor/TileAtlas.cpp" 
    "Editor/Tile.h"
    "Editor/Tile.cpp"
    "Editor/ComponentEditor.h"
    "Input/Action.h"
    "Input/Action.cpp" 
    "Input/Input.h" 
      
    "Input/InputMisc.h"
    "Input/Modifiers/Modifier.h"  
    "Input/Modifiers/NegateModifier.h" 
    "Input/Modifiers/SwizzleModifier.h" 
    "Input/Modifiers/DeadZoneModifier.h" 
    "Input/Modifiers/ScalarModifier.h" 
    "Input/Modifiers/DeltaTimeModifier.h" 
    "Input/Conditions/Condition.h" 
    "Input/Conditions/PressedCondition.h" 
    "Input/Conditions/ReleasedCondition.h" 
    "Input/Conditions/TapCondition.h" )

# Copy data folder to build directory. Hard links might be better, then there's less duplicate data. CMAKE_CURRENT_SOURCE_DIR might be better instead of LIST.
add_custom_target(copy_assets COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/Data ${CMAKE_CURRENT_BINARY_DIR}/Data)

# Includes
find_package(imgui CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)

# Create project as static library to link against in testing.
add_library(${PROJECT_NAME}_static STATIC ${ALL_FILES})

# Set up custom imconfig.h.
set_directory_properties(PROPERTIES COMPILE_DEFINITIONS IMGUI_USER_CONFIG="imconfig_custom.h")
target_include_directories(${PROJECT_NAME}_static PRIVATE ${CMAKE_CURRENT_LIST_DIR}) # This is neccesary for imgui, (and the testing includes), to find the path.

# Linking against static library.
target_link_libraries(${PROJECT_NAME}_static PRIVATE SDL2::SDL2 SDL2::SDL2main)
target_link_libraries(${PROJECT_NAME}_static PRIVATE $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>)
target_link_libraries(${PROJECT_NAME}_static PRIVATE imgui::imgui)
target_link_libraries(${PROJECT_NAME}_static PRIVATE $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>)

# Add source to this project's executable.
add_executable(${PROJECT_NAME} "main.cpp")
add_dependencies(${PROJECT_NAME} copy_assets)

# Linking against executable, strictly speaking only what's included in main is neccesary.
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_static)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2::SDL2main)