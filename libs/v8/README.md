# Put prebuilt v8 library files here

Go to [dep_v8](https://github.com/cherryridge/dep_v8/releases) and download the file corresponding to your operating system and your development requirements. Then put the archive's content (without root folder) into corresponding `debug` or `release` folders in this directory.

V8 has a huge repository, so we don't put submodule into here. If you want to access source code for debugging, follow [their instructions](https://v8.dev/docs/source-code) to check out source code.

For more information, please visit https://docs.cherrygrove.dev/cg/releasing/building.