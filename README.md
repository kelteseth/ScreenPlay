<div>
<img width="100%" height="93" src=".gitlab/media/logo_gitlab_fullwidth.svg">
</div>
<div align="center">
<br>

[![Pipeline Status](https://gitlab.com/kelteseth/ScreenPlay/badges/master/pipeline.svg)](https://gitlab.com/kelteseth/ScreenPlay/-/commits/master) 
[![Docs Pipeline](https://gitlab.com/kelteseth/ScreenPlayDeveloperDocs/badges/master/pipeline.svg)](https://gitlab.com/kelteseth/ScreenPlayDeveloperDocs/-/commits/master) 
[![Crowdin](https://badges.crowdin.net/screenplay/localized.svg)](https://crowdin.com/project/screenplay) 
[![Twitter Follow](https://img.shields.io/twitter/follow/kelteseth?style=flat-square&logo=twitter)](https://twitter.com/kelteseth) 
[![Discord](https://img.shields.io/discord/516635043435773970?style=flat-square&logo=discord&logoColor=white)](https://discord.com/invite/4sY9d4f2DM)
[![Reddit](https://img.shields.io/reddit/subreddit-subscribers/screenplayapp?style=flat-square&logo=reddit)](https://www.reddit.com/r/ScreenPlayApp/)

[![Forum](https://img.shields.io/badge/Forum-Join%20Discussion-orange?style=for-the-badge&logo=discourse&logoColor=white)](https://forum.screen-play.app/)

[![Steam](https://img.shields.io/badge/Steam-Download-blue?style=for-the-badge&logo=steam&logoColor=white)](https://store.steampowered.com/app/672870/ScreenPlay/)
[![Download](https://img.shields.io/badge/None%20Steam%20Version-Windows%20%26%20macOS-28a745?style=for-the-badge&logo=download&logoColor=white)](https://gitlab.com/kelteseth/ScreenPlay/-/releases)

![](/.gitlab/media/preview.webp) 

<a href="https://www.youtube.com/watch?v=q-J2fTWDxw8"><p>Watch the Steam Early Access Trailer (YouTube)</p></a> 

[中文总览](README_zh_CN.md)

ScreenPlay is an Open Source cross-platform app for displaying Video Wallpaper & Widgets. It is written in modern C++23/Qt6/QML. <br>  <a href="https://screen-play.app/">Homepage</a> - <a href="https://forum.screen-play.app/">Forums</a>- <a href="https://discord.com/invite/4sY9d4f2DM">Discord</a> 

</div>

## Table of Contents
- [Quick Start](#quick-start)
- [Content Creation](#content-creation)
- [General Contributing](#general-contributing)
- [Development](#development)
- [Platform Support](#platform-support)

## Quick Start

<h3><a href="https://kelteseth.gitlab.io/ScreenPlayDocs/building-screenplay/">Developer setup guide on how to download and compile ScreenPlay yourself.</a> </h3>

## ✨ Key Features

- 🎥 **Video Wallpapers** - Support for WebM, MP4 and other video formats
- 🎥 **Godot Engine Wallpapers** - Support for Godot 4 Wallpaper (🚀 Pro Feature)
- ⏰ **Timeline Wallpapers** - Automatically change wallpaper based on a user defined range (🚀 Pro Feature)
- 🖼️ **Static Wallpapers** - Beautiful image wallpapers with various effects
- 🎮 **Interactive QML Wallpapers** - Custom interactive wallpapers using Qt QML
- 🌐 **Web-based Wallpapers** - HTML/CSS/JavaScript wallpapers
- 🧩 **Desktop Widgets** - Customizable widgets for system monitoring, weather, RSS feeds, and more
- 🔧 **Cross-platform** - Works on Windows, macOS, and Linux
- 🎨 **Easy Content Creation** - No coding knowledge required for basic wallpapers
- 🛠️ **Steam Workshop** - Easy sharing and downloading of community content

## 📋 System Requirements

### Minimum Requirements
- **OS**: Windows 10 1909+, macOS 10.15+, or Linux with X11/Wayland
- **RAM**: 4 GB
- **GPU**: DirectX 11 compatible or OpenGL 3.3+ support
- **Storage**: 500 MB free space

### Recommended for Video Wallpapers
- **GPU**: Dedicated graphics card (NVIDIA GTX 1060 / AMD RX 580 or better)
- **RAM**: 8 GB+
- **CPU**: Quad-core processor

*Note: Performance varies based on wallpaper resolution and complexity. QML/HTML wallpapers are more lightweight than video wallpapers.*

# Content Creation
[Learn the basics of QML for Wallpapers and Widgets in 5 minutes](https://screen-play.app/blog/guide_learn_the_basics_of_qml/)
<div>
<a href="https://kelteseth.gitlab.io/ScreenPlayDocs/wallpaper/"><img src=".gitlab/media/wallpaper_guide.png"></a>
</div>

* [Wallpaper Documentation](https://kelteseth.gitlab.io/ScreenPlayDocs/wallpaper/). General wallpaper guidelines and information.
* [Performance Guidelines](https://kelteseth.gitlab.io/ScreenPlayDocs/wallpaper/). You need a modern GPU for smooth playback. You can always use QML/HTML/GIF wallpaper if you encounter performance issues!

<div>
<a href="https://kelteseth.gitlab.io/ScreenPlayDocs/widgets/"><img src=".gitlab/media/widgets_guide.png"></a>
</div>


* [Widgets Guide](https://kelteseth.gitlab.io/ScreenPlayDocs/widgets/). General guidelines how to create widgets. ScreenPlay uses the easy to learn QML language. No coding knowledge required!


# General Contributing

Everyone can contribute with code, design, documentation or translation. Visit our [contributing guide](https://kelteseth.gitlab.io/ScreenPlayDocs/contribute/) for more information. Also checkout our [Collaboration Guidelines](Docs/CodeOfConduct.md).

## 🚀 Quick Contributing Guide

**New to contributing?** Start here:
1. 🐛 [Report bugs](https://gitlab.com/kelteseth/ScreenPlay/-/issues) - Help us improve by reporting issues
2. 🌐 [Translate](https://crowdin.com/project/screenplay) - Make ScreenPlay available in your language  
3. 💡 [Request features](https://gitlab.com/kelteseth/ScreenPlay/-/issues) - Share your ideas
4. 🎨 [Create content](https://kelteseth.gitlab.io/ScreenPlayDocs/) - Make wallpapers and widgets

**For developers:**
5. 🔧 [Fix bugs](https://gitlab.com/kelteseth/ScreenPlay/-/issues?label_name%5B%5D=Bug) - Help resolve existing issues
6. ⭐ [Add features](https://gitlab.com/kelteseth/ScreenPlay/-/issues?label_name%5B%5D=Feature) - Implement new functionality
7. 📚 [Improve docs](https://gitlab.com/kelteseth/ScreenPlayDocs) - Help other users and developers

Here are some ways you can contribute:
* by using prerelease versions / master branch or the Steam nightly builds
* by [adding/fixing translation](https://crowdin.com/project/screenplay)
* by [designing UI/UX](https://kelteseth.gitlab.io/ScreenPlayDocs/contribute/)
* by [creating example HTML/QML/Javascript content](https://kelteseth.gitlab.io/ScreenPlayDocs/)
* by [reporting bugs](https://gitlab.com/kelteseth/ScreenPlay/-/issues)
* by [writing user documentation](https://gitlab.com/kelteseth/ScreenPlayDocs)
* by [coding needed features](https://gitlab.com/kelteseth/ScreenPlay/-/issues?label_name%5B%5D=Feature)
* by [refactoring code](https://gitlab.com/kelteseth/ScreenPlay/-/issues?label_name%5B%5D=Code+Quality)
* by [reviewing merge requests](https://gitlab.com/kelteseth/ScreenPlay/-/merge_requests)
* by [verifying issues](https://gitlab.com/kelteseth/ScreenPlay/-/issues?label_name%5B%5D=Unverified)

# Development
* [**Developer setup guide on how to download and compile ScreenPlay yourself.**](https://kelteseth.gitlab.io/ScreenPlayDocs/building-screenplay/)
 * If you want to contribute but don't know how to start, take a look at our open issues and WIP merge request.
 * If you need help don't hesitate to ask me (Kelteseth) via:
     * [Create a forum topic with a detailed description](https://forum.screen-play.app/)
     * [discord channel general](https://discord.gg/3RygPHZ)
 * [ScreenPlay project overview](Docs/ProjectOverview.md)
 * [Developer C++ Classes Documentation](https://kelteseth.gitlab.io/ScreenPlayDeveloperDocs/)

# Platform support

* ✅ Fully supported
* ❌ Not working/Not implemented
* ❓ Partially implemented/Not tested (help needed)

<div align="center">

| Feature                	    | Windows 	    | Linux (X11/Wayland) 	        | macOS 	|
|------------------------	    |---------	    |-------	        |-------	|
| **ScreenPlay Main App**       | ✅       	    | ✅     	        | ✅     	|
| **Steam Binaries**            | ✅       	    | ❌     	        | ✅    	|
| **Video Wallpapers**          | ✅       	    | ✅   | ✅    |
| **QML/HTML Wallpapers**       | ✅       	    | ✅   | ✅    |
| **Desktop Widgets**           | ✅       	    | ✅   	| ✅    	|
| **Multi-monitor Support**     | ✅       	    | ✅   	| ✅    	|
| **Workshop Integration**      | ✅       	    | ❌   	| ✅    	|
| **Multilanguage Support**<br/>*(EN,DE,RU,FR,ES,KO,VI,ZH_CN,PT_BR)*     | ✅            	| ✅     	        |  ✅     	|

</div>

## ❓ Frequently Asked Questions

<details>
<summary><strong>How do I install ScreenPlay?</strong></summary>

- **Steam (Recommended)**: [Download from Steam](https://store.steampowered.com/app/672870/ScreenPlay/) for automatic updates and Workshop support
- **Direct Download (Non-Steam Version)**: 
  - [Latest Release](https://gitlab.com/kelteseth/ScreenPlay/-/releases) - Windows and macOS builds without Steam Workshop integration
  - [getsp.de](https://getsp.de/) - Alternative download source
  
*Note: Linux builds are coming soon! For community content downloads, visit our [forums](https://forum.screen-play.app/) where users share wallpapers and widgets.*
</details>

<details>
<summary><strong>Why is my video wallpaper performance poor?</strong></summary>

Video wallpapers require a modern GPU. If you experience performance issues:
- Try reducing the video resolution or bitrate
- Switch to QML or HTML wallpapers for better performance
- Check our [Performance Guidelines](https://kelteseth.gitlab.io/ScreenPlayDocs/wallpaper/)
</details>

<details>
<summary><strong>How do I create my own wallpaper or widget?</strong></summary>

- Check our [5-minute QML tutorial](https://screen-play.app/blog/guide_learn_the_basics_of_qml/)
- Read the [Wallpaper Documentation](https://kelteseth.gitlab.io/ScreenPlayDocs/wallpaper/)
- Browse the [Widgets Guide](https://kelteseth.gitlab.io/ScreenPlayDocs/widgets/)
- Look at example content in the `Content/` folder
</details>

<details>
<summary><strong>Can I use ScreenPlay on multiple monitors?</strong></summary>

Yes! ScreenPlay supports multi-monitor setups. You can set different wallpapers on each monitor and position widgets anywhere across your displays.
</details>

<details>
<summary><strong>Where can I download wallpapers and widgets?</strong></summary>

- **Steam Workshop**: If you use the Steam version, browse and download content directly through Steam Workshop
- **Community Forums**: Visit our [forums](https://forum.screen-play.app/) for user-shared content - many users prefer this for the direct download versions
- **Create Your Own**: Check our [content creation guides](https://kelteseth.gitlab.io/ScreenPlayDocs/) to make your own wallpapers and widgets
</details>

