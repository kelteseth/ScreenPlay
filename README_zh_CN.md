<div>
<img width="100%" height="93" src=".gitlab/media/logo_gitlab_fullwidth.svg">
</div>
<div align="center">
<br>

[![构建状态](https://gitlab.com/kelteseth/ScreenPlay/badges/master/pipeline.svg)](https://gitlab.com/kelteseth/ScreenPlay/-/commits/master) 
[![文档构建](https://gitlab.com/kelteseth/ScreenPlayDeveloperDocs/badges/master/pipeline.svg)](https://gitlab.com/kelteseth/ScreenPlayDeveloperDocs/-/commits/master) 
[![Crowdin](https://badges.crowdin.net/screenplay/localized.svg)](https://crowdin.com/project/screenplay) 
[![论坛](https://img.shields.io/badge/Forum-Discussion-orange?style=flat-square&logo=discourse&logoColor=white)](https://forum.screen-play.app/)
[![Twitter关注](https://img.shields.io/twitter/follow/kelteseth?style=flat-square&logo=twitter)](https://twitter.com/kelteseth) 
[![Discord](https://img.shields.io/discord/516635043435773970?style=flat-square&logo=discord&logoColor=white)](https://discord.com/invite/4sY9d4f2DM)
[![Reddit](https://img.shields.io/reddit/subreddit-subscribers/screenplayapp?style=flat-square&logo=reddit)](https://www.reddit.com/r/ScreenPlayApp/)

[![Steam](https://img.shields.io/badge/Steam-Download-blue?style=for-the-badge&logo=steam&logoColor=white)](https://store.steampowered.com/app/672870/ScreenPlay/)
[![Windows](https://img.shields.io/badge/Windows-Download-0078d4?style=for-the-badge&logo=windows&logoColor=white)](http://gitlab.com/kelteseth/ScreenPlay/-/jobs/artifacts/master/download?job=build:windows_release)
[![macOS](https://img.shields.io/badge/macOS-Download-000000?style=for-the-badge&logo=apple&logoColor=white)](http://gitlab.com/kelteseth/ScreenPlay/-/jobs/artifacts/master/download?job=build:osx_release)

![](/.gitlab/media/preview.webp) 

<a href="https://www.youtube.com/watch?v=q-J2fTWDxw8"><p>观看Steam抢先体验预告片 (YouTube)</p></a> 

[English README](README.md)

ScreenPlay 是一款开源、跨平台的视频壁纸与桌面组件应用程序。它使用现代的 C++23/Qt6/QML 编写。<br>  <a href="https://screen-play.app/">主页</a> - <a href="https://forum.screen-play.app/">论坛</a>- <a href="https://discord.com/invite/4sY9d4f2DM">Discord</a> 

</div>

## 目录
- [快速开始](#快速开始)
- [内容创建](#内容创建)
- [一般贡献](#一般贡献)
- [开发](#开发)
- [平台支持](#平台支持)

## 快速开始

<h3><a href="https://kelteseth.gitlab.io/ScreenPlayDocs/building-screenplay/">开发者设置指南：如何下载和编译ScreenPlay</a> </h3>

## ✨ 主要功能

- 🎥 **视频壁纸** - 支持 WebM、MP4 和其他视频格式
- 🎥 **Godot 引擎壁纸** - 支持 Godot 4 壁纸 (🚀 专业版功能)
- ⏰ **时间线壁纸** - 根据用户定义的时间范围自动更换壁纸 (🚀 专业版功能)
- 🖼️ **静态壁纸** - 带有各种效果的精美图像壁纸
- 🎮 **交互式QML壁纸** - 使用 Qt QML 的自定义交互式壁纸
- 🌐 **网页壁纸** - HTML/CSS/JavaScript 壁纸
- 🧩 **桌面组件** - 可自定义的系统监控、天气、RSS订阅等组件
- 🔧 **跨平台** - 支持 Windows、macOS 和 Linux
- 🎨 **简单的内容创建** - 基础壁纸无需编程知识
- 🛠️ **Steam创意工坊** - 轻松分享和下载社区内容

## 📋 系统要求

### 最低要求
- **操作系统**: Windows 10 1909+, macOS 10.15+, 或带有 X11/Wayland 的 Linux
- **内存**: 4 GB
- **显卡**: 支持 DirectX 11 或 OpenGL 3.3+
- **存储空间**: 500 MB 可用空间

### 视频壁纸推荐配置
- **显卡**: 独立显卡 (NVIDIA GTX 1060 / AMD RX 580 或更好)
- **内存**: 8 GB+
- **处理器**: 四核处理器

*注意: 性能因壁纸分辨率和复杂度而异。QML/HTML 壁纸比视频壁纸更轻量。*

# 内容创建
[5分钟学会壁纸和组件的QML基础](https://screen-play.app/blog/guide_learn_the_basics_of_qml/)
<div>
<a href="https://kelteseth.gitlab.io/ScreenPlayDocs/wallpaper/"><img src=".gitlab/media/wallpaper_guide.png"></a>
</div>

* [壁纸文档](https://kelteseth.gitlab.io/ScreenPlayDocs/wallpaper/) - 通用壁纸指南和信息。
* [性能指南](https://kelteseth.gitlab.io/ScreenPlayDocs/wallpaper/) - 流畅播放需要现代显卡。如果遇到性能问题，可以使用 QML/HTML/GIF 壁纸！
* [通过Handbrake导入视频](https://forum.screen-play.app/topic/43/fast-bulk-video-conversion-with-handbrake) - Handbrake 提供简便的批量视频转换。

<div>
<a href="https://kelteseth.gitlab.io/ScreenPlayDocs/widgets/"><img src=".gitlab/media/widgets_guide.png"></a>
</div>

* [组件指南](https://kelteseth.gitlab.io/ScreenPlayDocs/widgets/) - 如何创建组件的通用指南。ScreenPlay 使用易学的 QML 语言，无需编程知识！


# 一般贡献

每个人都可以贡献代码、设计、文档以及翻译。更多信息请查看我们的[贡献指南](https://kelteseth.gitlab.io/ScreenPlayDocs/contribute/)。也请查阅我们的[协作指南](Docs/CodeOfConduct.md)。

## 🚀 快速贡献指南

**新手贡献者？** 从这里开始：
1. 🐛 [报告错误](https://gitlab.com/kelteseth/ScreenPlay/-/issues) - 帮助我们通过报告问题来改进
2. 🌐 [翻译](https://crowdin.com/project/screenplay) - 让 ScreenPlay 支持您的语言  
3. 💡 [功能请求](https://gitlab.com/kelteseth/ScreenPlay/-/issues) - 分享您的想法
4. 🎨 [创建内容](https://kelteseth.gitlab.io/ScreenPlayDocs/) - 制作壁纸和组件

**开发者:**
5. 🔧 [修复错误](https://gitlab.com/kelteseth/ScreenPlay/-/issues?label_name%5B%5D=Bug) - 帮助解决现有问题
6. ⭐ [添加功能](https://gitlab.com/kelteseth/ScreenPlay/-/issues?label_name%5B%5D=Feature) - 实现新功能
7. 📚 [改进文档](https://gitlab.com/kelteseth/ScreenPlayDocs) - 帮助其他用户和开发者

以下是一些贡献方式：
* 使用预发布版本 / master 分支或 Steam 每夜构建版本
* [添加/修复翻译](https://crowdin.com/project/screenplay)
* [设计 UI/UX](https://kelteseth.gitlab.io/ScreenPlayDocs/contribute/)
* [创建示例 HTML/QML/Javascript 内容](https://kelteseth.gitlab.io/ScreenPlayDocs/)
* [报告错误](https://gitlab.com/kelteseth/ScreenPlay/-/issues)
* [编写用户文档](https://gitlab.com/kelteseth/ScreenPlayDocs)
* [编写需要的功能](https://gitlab.com/kelteseth/ScreenPlay/-/issues?label_name%5B%5D=Feature)
* [重构代码](https://gitlab.com/kelteseth/ScreenPlay/-/issues?label_name%5B%5D=Code+Quality)
* [审查合并请求](https://gitlab.com/kelteseth/ScreenPlay/-/merge_requests)
* [验证问题](https://gitlab.com/kelteseth/ScreenPlay/-/issues?label_name%5B%5D=Unverified)

# 开发
* [**开发者设置指南：如何下载和编译ScreenPlay**](https://kelteseth.gitlab.io/ScreenPlayDocs/building-screenplay/)
 * If you want to contribute but don't know how to start, take a look at our open issues and WIP merge request.
 * If you need help don't hesitate to ask me (Kelteseth) via:
     * [Create a forum topic with a detailed description](https://forum.screen-play.app/category/2/general-discussion)
     * [discord channel general](https://discord.gg/3RygPHZ)
 * [ScreenPlay project overview](Docs/ProjectOverview.md)
 * [Developer C++ Classes Documentation](https://kelteseth.gitlab.io/ScreenPlayDeveloperDocs/)

# 平台支持

* ✅ 完全支持
* ❌ 不工作/尚未实现
* ❓ 部分实现/尚未测试（需要帮助）

<div align="center">

| 功能                	    | Windows 	    | Linux (X11/Wayland) 	        | macOS 	|
|------------------------	    |---------	    |-------	        |-------	|
| **ScreenPlay 主程序**       | ✅       	    | ✅     	        | ✅     	|
| **Steam 二进制文件**            | ✅       	    | ❌     	        | ✅    	|
| **视频壁纸**          | ✅       	    | ✅   | ✅    |
| **QML/HTML 壁纸**       | ✅       	    | ✅   | ✅    |
| **桌面组件**           | ✅       	    | ✅   	| ✅    	|
| **多显示器支持**     | ✅       	    | ✅   	| ✅    	|
| **创意工坊集成**      | ✅       	    | ❌   	| ✅    	|
| **多语言支持**<br/>*(EN,DE,RU,FR,ES,KO,VI,ZH_CN,PT_BR)*     | ✅            	| ✅     	        |  ✅     	|

</div>

## ❓ 常见问题

<details>
<summary><strong>如何安装 ScreenPlay？</strong></summary>

- **Steam（推荐）**: [从Steam下载](https://store.steampowered.com/app/672870/ScreenPlay/) 获得自动更新和创意工坊支持
- **直接下载**: 
  - [Windows](http://gitlab.com/kelteseth/ScreenPlay/-/jobs/artifacts/master/download?job=build:windows_release)
  - [macOS](http://gitlab.com/kelteseth/ScreenPlay/-/jobs/artifacts/master/download?job=build:osx_release) (未签名)
</details>

<details>
<summary><strong>为什么我的视频壁纸性能很差？</strong></summary>

视频壁纸需要现代显卡。如果遇到性能问题：
- 尝试降低视频分辨率或比特率
- 切换到 QML 或 HTML 壁纸以获得更好的性能
- 查看我们的[性能指南](https://kelteseth.gitlab.io/ScreenPlayDocs/wallpaper/)
</details>

<details>
<summary><strong>如何创建自己的壁纸或组件？</strong></summary>

- 查看我们的[5分钟QML教程](https://screen-play.app/blog/guide_learn_the_basics_of_qml/)
- 阅读[壁纸文档](https://kelteseth.gitlab.io/ScreenPlayDocs/wallpaper/)
- 浏览[组件指南](https://kelteseth.gitlab.io/ScreenPlayDocs/widgets/)
- 查看 `Content/` 文件夹中的示例内容
</details>

<details>
<summary><strong>我可以在多显示器上使用 ScreenPlay 吗？</strong></summary>

可以！ScreenPlay 支持多显示器设置。您可以在每个显示器上设置不同的壁纸，并在显示器上任意位置放置组件。
</details>

