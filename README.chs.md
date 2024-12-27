[English](README.md) | 中文

<p align="center"><img alt="CherryGrove Logo" width="120" src="assets/icons/CherryGrove-trs-2048.png" /></p>

<h1 align="center">CherryGrove</h1>

为未来打造的高度可自定义、高性能、脚本驱动的方块游戏引擎。

## 目标

1. 无内置游戏功能，因此几乎所有游戏元素都可以自定义。
2. 可以使用 JSON 和 JavaScript 高度自定义（模组化），且大部分属性和定义都可以在游戏过程中被动态修改。
   - 开发你自己的 C++ 模组框架也不难。
3. 基于 V8 和大量内置组件的高性能。内容包开发者不必重复发明轮子，可以直接使用高性能的内置组件。
4. 为动画创作集成相机预设和离线渲染功能。
5. 拥有兼容许多平台（Windows、Linux、Mac、Android、iOS、Web 浏览器 + emscripten）的能力和计划。
6. 完全免费开源，并使用 [GPL-3.0-or-later](LICENSE) 协议授权。
7. 对于某些人和商家来说，可以用来信息差变现和打商业广告而不用担心法律问题。:)

## 构建

### 构建依赖

以下是 CherryGrove 的依赖列表。请注意：CherryGrove 仍在活跃开发中，该列表可能会有变动。

CherryGrove @ 0.0.1 (*未发布*)

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
- [boost](https://boost.org) @ 1.87.0 (c89e626)
- [SoLoud](https://solhsa.com/soloud/index.html) @ 20200207 (c8e339f)

## 贡献

目前本项目和组织仅由 [LJM12914](https://github.com/ljm12914) 维护。如果您想要贡献，请通过我主页上的联系方式联系我。

## 许可

© 2024 LJM12914。以 [GPL-3.0-or-later](LICENSE) 协议授予许可。
