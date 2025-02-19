English | [中文](README.chs.md)

<p align="center"><img alt="CherryGrove Logo" width="120" src="assets/icons/CherryGrove-trs-2048.png" /></p>

<h1 align="center">CherryGrove</h1>

An easily customizable, high performance scripting-based block game engine made for the next generation.

## Goals

1. None of actual gameplay content is built-in, so basically every aspect of gameplay is changeable.
2. Highly customizable via JSON or JavaScript. Most of the gameplay definitions can be changed dynamically.
   - Also it's not difficult to develop your own modding API in C++.
3. High performance due to V8 engine and various intrinsic components that can be quickly accessed by packs.
4. Integrate camera presets and offline-rendering to provide tools for animation creation.
5. Abilities and plans to support various platforms: Windows, Linux, Mac, Android, iOS, Web (emscripten).
6. Completely free and open source, licensed under [GPL-3.0-or-later](LICENSE).

## Building

### Build Dependencies

Below is a list of dependencies of CherryGrove. Please note that CherryGrove is still in active development and this list is subject to change.

CherryGrove @ 0.0.1 (*Not released yet*)

- [glfw](https://www.glfw.org/) @ 3.4 (7b6aead)
- [bgfx](https://github.com/bkaradzic/bgfx) @ 69acf28 ([bimg](https://github.com/bkaradzic/bimg) @ 0d1c78e, [bx](https://github.com/bkaradzic/bx) @ 7014882)
- [stbi](https://github.com/nothings/stb) @ 5c20573
- [Dear ImGui](https://github.com/ocornut/imgui) @ docking @ 14d213c
- [v8](https://v8.dev/) @ 6142bd1
- [FreeType](https://freetype.org/) @ 2.13.3 (42608f77)
- [pr0g's imgui_impl_bgfx](https://gist.github.com/pr0g/aff79b71bf9804ddb03f39ca7c0c3bbb) @ a8dce22
- [glm](https://github.com/g-truc/glm) @ 1.0.1 (0af55cc)
- [EnTT](https://github.com/skypjack/entt) @ 42d9628
- [JSON for Modern C++](https://github.com/nlohmann/json) @ 3.11.3 (9cca280)
- [JSON schema validator for JSON for Modern C++](https://github.com/pboettch/json-schema-validator) @ fbd72de
- [boost](https://boost.org) @ 1.87.0 (c89e626)
- [SoLoud](https://solhsa.com/soloud/index.html) @ 20200207 (c8e339f)

Some important notes:

1. CherryGrove uses dynamic runtime library on Windows (`/MD` and `/MDd`).
2. Reach out [`args-*.gn`](lib_archive) for arguments to build V8.

## Contributing

Currently the repository and organization is maintained solely by [LJM12914](https://github.com/ljm12914). Feel free to contact me via the contact information on my profile if you want to contribute.

## License

©2025 LJM12914. Licensed under [GPL-3.0-or-later](LICENSE).
