# Put prebuilt wasmtime library files here

Go to [dep_wasmtime](https://github.com/cherryridge/dep_wasmtime/releases) and download the file corresponding to your operating system and your development requirements. Then put the archive's content (without root folder) into corresponding `debug` or `release` folders in this directory.

Wasmtime is written in Rust, and most C++ debuggers can't refer to its source code, so we don't put submodule into here.

For more information, please visit https://docs.cherrygrove.dev/cg/releasing/building.