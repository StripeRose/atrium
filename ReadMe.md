# Rose Gold

## About the project

The intent of this project is to create a graphics library and game engine that I can use for other projects of mine and to get a better understanding in how game engines are built.

## Project goals

* Modular and split in concrete blocks of functionality.
* Keep it simple to use. Every part of the code should be relatively self-explanatory.
* Allow room for expansion to console systems. Getting it to run on a PS Vita would be really cool.
* Performance, performance, performance.

## Folder structure
* `build`
* `code`
    * `apis` - *Non-platform specific API implementations.*
        * `dx12`
        * `xinput`
    * `client`
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
* `data`
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
