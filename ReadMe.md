# Atrium

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
 * Visual Studio 2022 (Tracy profiler requires CMake tools to build)

### Adding and setting up the repository

In your desired project directory, either:

1. Clone into your own directory with `git clone https://github.com/StripeRose/atrium` **OR**
2. Add it as a sub-module with `git submodule add https://github.com/StripeRose/atrium`

This will set up the engine in the `./atrium/` directory.

Navigate into the directory, and run `./setup_repo.bat` to clone all submodules in use, and build [Sharpmake](https://github.com/ubisoft/Sharpmake/), the Visual Studio solution generator.

### Create a new project

Copy and rename the `example` directory so it lies next to `atrium`.
The directory contains a basic program that sets up the engine to open a blank window, and exits when you close it.

Once copied, run `generate_solution.bat` from within the directory. This should create a solution to open and run from Visual Studio.

## Example code

Further example projects are located in [atrium-examples](https://github.com/StripeRose/atrium-examples).

## Performance profiling

The codebase uses [Tracy](https://github.com/wolfpld/tracy) for profiling on supported platforms.

The executable is located at `./tools/tracy-profiler.exe`, which you can run before or while the project is running.

### Building Tracy
To build the Tracy profiler program, ensure Visual Studio 2022 has been installed with CMake tools, and run `./code/libraries/build_tracy_server.bat`.

## Structure
Rough project structure, both current and potential future data.
* `build` — *The target build directory for everything.*
* `code`
    * `apis` — *Non-platform specific API implementations.*
    * `client` — *Individual platform clients and any platform-specific code.*
    * `core` — *Smaller common interfaces to allow the engine to use any API. Primarily to simplify porting.*
        * `audio` — *API for interfacing with audio playback and recording devices.*
        * `graphics` — *API for interfacing with GPUs.*
        * `input` — *API for interfacing with keyboards, mice, controllers, joysticks and other input devices.*
        * `platform` — *API for interfacing with platform-specific features such as windows.*
    * `engine` — *Basics of the engine itself, such as scene and component management.*
	* `example` — *An empty example project.*
    * `libraries` — *External code in use by the project.*
* `config`
* `tools` — *Any programs or scripts to help build executables or data.*
