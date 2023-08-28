# Engine²
Engine² is a 2D sprite engine, named after the fact it's an engine with 2-dimensions.

For PNG support with sdl_image the package additionally requires: "libpng", "zlib",

## Source/Core
Contains multiple fundamental classes, primarily around rendering, inputs, and wrapping SDL functions and objects.

## Source/SceneManagement
Contains the scenes and their manager. These are all derived from a "BaseScene" so that they can be managed by a map and accessed indiscriminately. Most virtual overhead should be neglible as they are typically called once per scene update, but some of the getters and conversion methods might have an impact if called too much.

## Source/Data
Contains all the data that needs to be loaded at runtime; textures, fonts, etc.

This folder is copied on build to the executable's directory.

## Source/EntityComponentSystem
Contains fundamental classes for ECS, including components and systems which are all derived from a "BaseSystem" so that they can be managed by a vector and iterated over indiscriminately. Any virtual overhead should be negligible as they are called once per update.

## Source/Commands
Contains classes derived from the Command base class, and the Undo Manager. 
Perhaps unsurprisingly, the command pattern is used to enable undoing and redoing. 

<hr>
<sup>Engine² = (Engine)(Engine) = EEnnggiinnee != eennggiinnee</sup>
