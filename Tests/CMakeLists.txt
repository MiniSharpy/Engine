cmake_minimum_required (VERSION 3.12)

find_package(GTest CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)
find_package(sdl2-image CONFIG REQUIRED)

add_executable(EngineTests 
"Maths/Vector2Tests.cpp"  
"Maths/RectangleTests.cpp"
"Collision/CollisionTests.cpp" 
"SceneManagement/IsometricSceneTests.cpp" 
"Commands/CreateEntityCommandTests.cpp" 
)

set_property(TARGET EngineTests PROPERTY CXX_STANDARD 20)


# Link GTest main, is not performed by default in case user needs to implement their own version.
target_link_libraries(EngineTests PRIVATE GTest::gtest_main GTest::gtest)
# Use everything the main project does just incase include paths get confused.
target_link_libraries(EngineTests PRIVATE SDL2::SDL2 SDL2::SDL2main)
target_link_libraries(EngineTests PRIVATE SDL2::SDL2_image)
target_link_libraries(EngineTests PRIVATE imgui::imgui)
target_link_libraries(EngineTests PRIVATE $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>)


# Link against engine.
target_link_libraries(EngineTests PRIVATE ${PROJECT_NAME}_static)

# For VS Test Explorer (And CTest) to work.
add_test(EngineTests EngineTests)