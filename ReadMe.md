# Atrium

## About the project

The intent of this project is to create a graphics library and game engine that I can use for other projects of mine and to improve my understanding of how game engines are built and structured.

## Project goals

* Keep it simple to use. Every part of the code should be relatively self-explanatory.
* Allow room for expansion to other platforms. Getting it to run on a PS Vita would be really cool.
* Modular and split in concrete blocks of functionality.
* Performance, performance, performance.

## Getting started

### Prerequisites

 * Windows 11
 * Visual Studio 2022 with CMake

### Adding and setting up the repository

In your desired project directory, either:

1. Clone into your own directory with `git clone https://github.com/StripeRose/atrium` **OR**
2. Add it as a sub-module with `git submodule add https://github.com/StripeRose/atrium`

This will set up the engine in the `./atrium/` directory.

Navigate into the directory and execute `git submodule update --init --recursive` to clone all dependencies.

### Create a new project

Copy and rename the `example` directory so it lies next to the `atrium` directory.
The directory contains a basic program that sets up the engine to open a blank window, and exits when you close it.

Once copied, run `generate_solution.bat` from within the directory. If it’s the first run, it will build [Sharpmake](https://github.com/ubisoft/Sharpmake/) and use it to generate a Visual Studio 2022 solution.

## Example code

Further example projects are located in [atrium-examples](https://github.com/StripeRose/atrium-examples).

## Performance profiling

The codebase uses [Tracy](https://github.com/wolfpld/tracy) for profiling on supported platforms.

In order to run it, use the script located at `./tools/run-profiler.bat`.
If it doesn't exist, it will attempt to build from the source-code and start after.

## Acknowledgements

Atrium currently utilizes the following third-party open-source libraries and tools.

- [Sharpmake](https://github.com/ubisoft/Sharpmake/) to generate VS solutions, by Ubisoft
- [Dear ImGui](https://github.com/ocornut/imgui/) for editor and debug UI, by Omar Cornut and all contributors.
- [DirectXTex](https://github.com/Microsoft/DirectXTex/) to handle DDS texture files, by Microsoft
- [Tracy](https://github.com/wolfpld/tracy/) for profiling code, by Bartosz Taudul and all contributors.

Additionally it uses the following third-party projects:

- [Microsoft GameInput](https://aka.ms/gameinput) as an input-device source, by Microsoft
