<div>
<img width="100%" height="93" src=".gitlab/media/logo_gitlab_fullwidth.svg">
</div>

<div align="center">

<br>

ScreenPlay [![pipeline status](https://gitlab.com/kelteseth/ScreenPlay/badges/master/pipeline.svg)](https://gitlab.com/kelteseth/ScreenPlay/-/commits/master) <br>
Dev. Docs   [![pipeline status](https://gitlab.com/kelteseth/ScreenPlayDeveloperDocs/badges/master/pipeline.svg)](https://gitlab.com/kelteseth/ScreenPlayDeveloperDocs/-/commits/master) <br><br>
<a href="https://twitter.com/kelteseth">![Twitter Follow](https://img.shields.io/twitter/follow/kelteseth?style=for-the-badge)</a>
<a href="https://www.reddit.com/r/ScreenPlayApp/">![Subreddit subscribers](https://img.shields.io/reddit/subreddit-subscribers/screenplayapp?style=for-the-badge)</a>
<a href="https://discord.com/invite/rUvjNSV?utm_source=Discord%20Widget&utm_medium=Connect">![Discord](https://img.shields.io/discord/516635043435773970?style=for-the-badge)</a>



<br>
ScreenPlay is an open source cross platform app for displaying Video Wallpaper, Widgets and AppDrawer. It is written in modern C++20/Qt5/QML. Binaries with workshop support are available for Windows and soon Linux & MacOSX via <a href="https://store.steampowered.com/about/">Steam</a>.  Join our community: 

<h3><a href="https://screen-play.app/">Homepage</a> - <a href="https://forum.screen-play.app/">Forum</a> - <a href="https://discord.gg/3RygPHZ">🎉 Discord (Free Steam Key Here!)</a>  -  <a href="https://www.reddit.com/r/ScreenPlayApp/">Reddit</a> - <a href="https://store.steampowered.com/app/672870/ScreenPlay/">Steam</a></h3>
<br>

<a href="https://www.youtube.com/watch?v=q-J2fTWDxw8">

![Preview](.gitlab/media/preview.gif)

</a> 
<p>Click gif to see the Steam Early Access Trailer (YouTube)</p>
</div>



# Contributing

Everyone can contribute with code, design, documentation or translation. Visit our [contributing guide](https://kelteseth.gitlab.io/ScreenPlayDocs/contribute/contribute) for more information. Also checkout our [Collaboration Guidelines](Docs/CodeOfConduct.md).

Here are some ways you can contribute:
* by using prerelease versions / master branch or the Steam nightly builds
* by [adding/fixing translation](https://kelteseth.gitlab.io/ScreenPlayDocs/contribute/translations/)
* by [designing UI/UX](https://kelteseth.gitlab.io/ScreenPlayDocs/contribute/contribute/#design)
* by [creating example HTML/QML/Javascript content](https://kelteseth.gitlab.io/ScreenPlayDocs/)
* by [reporting bugs](https://gitlab.com/kelteseth/ScreenPlay/-/issues)
* by [writing user documentation](https://gitlab.com/kelteseth/ScreenPlayDocs)
* by [coding needed features](https://gitlab.com/kelteseth/ScreenPlay/-/issues?label_name%5B%5D=Feature)
* by [refactoring code](https://gitlab.com/kelteseth/ScreenPlay/-/issues?label_name%5B%5D=Code+Quality)
* by [reviewing merge requests](https://gitlab.com/kelteseth/ScreenPlay/-/merge_requests)
* by [verifying issues](https://gitlab.com/kelteseth/ScreenPlay/-/issues?label_name%5B%5D=Unverified)

# Coding
* [**Developer setup guide on how to download and compile ScreenPlay yourself.**](Docs/DeveloperSetup.md)
 * If you want to contribute but don't know how to start, take a look at our open issues and WIP merge request.
 * If you need help don't hesitate to ask me (Kelteseth) via:
     * [Create a forum topic with a detailed description](https://forum.screen-play.app/category/2/general-discussion)
     * [discord channel general](https://discord.gg/3RygPHZ)
 * [ScreenPlay project overview](Docs/ProjectOverview.md)
 * [Developer C++ Classes Documentation](https://kelteseth.gitlab.io/ScreenPlayDeveloperDocs/)

# Content Creation
* [Wallpaper Guide](https://kelteseth.gitlab.io/ScreenPlayDocs/wallpaper/wallpaper/)
* [Widgets Guide](https://kelteseth.gitlab.io/ScreenPlayDocs/widgets/widgets/)

# Platform support

* ❌ Not working/Not implemented
* ❓ Only partially implemented/Not tested, help needed

<div align="center">

| Feature                	    | Windows 	    | Linux 	        | MacOS 	|
|------------------------	    |---------	    |-------	        |-------	|
| __ScreenPlay Main App__       | ✔       	    | ✔     	        | ✔     	|
| __Steam Binaries__            | ✔       	    | ❌     	        | ❌     	|
| __Wallpaper__                 | ✔       	    | ❓ Help Needed for Gnome/KDE/etc!               | ✔    |
| __Widgets__                   | ✔       	    | ✔   	| ✔    	|
| __Multilanguage (EN,DE,RU,FR,ES,KO,VI,CH🆕)__              | ✔            	| ✔     	        |  ✔     	|

</div>

Because every operating system has its own version of desktop environment we need to adapt the ScreenPlayWindow for every platform 
separately. The most feature complete for now is Windows 10. Windows 7 works but the wallpaper have the [wrong coordinates](https://gitlab.com/kelteseth/ScreenPlay/issues/34). MacOS has some basic wallpaper functionality but no maintainer. For Linux we sadly have no support for any desktop environments at the moment, except basic KDE support.

__If you want to help and add new desktop environments look at ScreenPlayWallpaper/src folder__

* [BaseWindow](https://gitlab.com/kelteseth/ScreenPlay/blob/dev/ScreenPlayWallpaper/src/basewindow.h) baseclass for:
    * [LinuxWindow](https://gitlab.com/kelteseth/ScreenPlay/blob/dev/ScreenPlayWallpaper/src/linuxwindow.h)
    * [WinWindow](https://gitlab.com/kelteseth/ScreenPlay/blob/dev/ScreenPlayWallpaper/src/winwindow.h)
    * [MacWindow](https://gitlab.com/kelteseth/ScreenPlay/blob/dev/ScreenPlayWallpaper/src/macwindow.h)

<div align="center">

| Plattform                	     | Windows 10  |   Gnome	         | KDE 	           	        | MacOS  	|
|------------------------	     |-------	   |---------	     |-------	                | -------	|
| __Wallpaper__                  | ✔           |❌ Help Needed!  | ❓ [Basic implementation](https://gitlab.com/kelteseth/ScreenPlay/-/tree/master/ScreenPlayWallpaper/kde/ScreenPlay)   | ❓ [Basic implementation](https://gitlab.com/kelteseth/ScreenPlay/-/blob/master/ScreenPlayWallpaper/src/MacBridge.mm)     	|


</div>

