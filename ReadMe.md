# Rose Gold

## About the project

The intent of this project is to create a graphics library and game engine that I can use for other projects of mine and to get a better understanding in how game engines are built.

## Project goals

* Modular and split in concrete blocks of functionality.
* Keep it simple to use. Every part of the code should be relatively self-explanatory.
* Allow room for expansion to console systems. Getting it to run on a PS Vita would be really cool.
* Performance, performance, performance.

## Getting started

### Prerequisites

 * Windows 11
 * Visual Studio 2022

### Setting up the repository

Once cloned on its own or as part of a larger project, run `setup_repo.bat`.
This script will automatically do the following.

1. Clone all submodules the in use.
2. Build the tools in use by the project; [Sharpmake](https://github.com/ubisoft/Sharpmake/) and [Tracy](https://github.com/wolfpld/tracy).

### Open the example project

Run `./code/generate_example_solution.bat` to use Sharpmake to generate the example game solution.

This will create all the current example solutions in the same directory.

## Performance profiling

The codebase uses [Tracy](https://github.com/wolfpld/tracy) for profiling on supported platforms. After it's been built successfully as part of the `setup_repo.bat` script, there is a shortcut to run it in `./code/run_tracy_profiler.bat`, which you can run before or after you start the example project.

## Structure
* `build`
* `code`
    * `apis` - *Non-platform specific API implementations.*
        * `dx12`
        * `xinput`
    * `client` - *Individual platform clients and any platform-specific code.*
        * `bootstrapper`
        * `windows`
    * `core` - *Core code in use by the engine and other projects.*
        * `audio` - *Future API for creating audio playback.*
        * `graphics` - *API for creating graphics renderers.*
        * `input` - *API for creating input handlers.*
        * `platform` - *API for creating platform handlers that tie things together.*
    * `engine` - *Basics of the engine itself. Scene and component management, amongst other things.*
	* `example`
    * `libraries` - *External code in use by the project.*
	* `modules` - *Add-on modules for the engine.*
        * `node-editor`
        * `user-interface`
        * `...`
* `config`
* `data` - *Example assets and structure.*
    * `assets`
        * `audio`
        * `fonts`
        * `images`
        * `models`
        * `shaders`
        * `video`
    * `docs`
    * `localization`
    * `scripts`
* `tools`
