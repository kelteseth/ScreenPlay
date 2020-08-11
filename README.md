<div>
<img width="100%" height="93" src=".gitlab/media/logo_gitlab_fullwidth.svg">
</div>

<div align="center">

[![pipeline status](https://gitlab.com/kelteseth/ScreenPlay/badges/master/pipeline.svg)](https://gitlab.com/kelteseth/ScreenPlay/-/commits/master)

ScreenPlay is an open source cross platform app for displaying Wallpaper, Widgets and AppDrawer. It is written in modern C++17/Qt5/QML. Binaries with workshop support are available for Windows and soon Linux & MacOSX via <a href="https://store.steampowered.com/about/">Steam</a>.  Join our community: 

<h3><a href="https://screen-play.app/">Homepage</a> - <a href="https://forum.screen-play.app/">Forum</a> - <a href="https://discord.gg/3RygPHZ">🎉 Discord (Free Steam Key Here!)</a>  -  <a href="https://www.reddit.com/r/ScreenPlayApp/">Reddit</a></h3>
<!--<h4><a href="https://steamcommunity.com/app/672870/">Download ScreenPlay!</a></h4>-->
<br>

![Preview](.gitlab/media/preview.mp4)

<br> <h4><a href="https://kelteseth.gitlab.io/ScreenPlayDocs/wallpaper/wallpaper/">Wallpaper </a>  and  <a href="https://kelteseth.gitlab.io/ScreenPlayDocs/widgets/widgets/">  Widgets </a>Guide</h4>
</div>


# Contributing for none programmer

Everyone can contribute with code, design, documentation or translation. Visit our [contributing guide](https://kelteseth.gitlab.io/ScreenPlayDocs/contribute/contribute) for more informations. Also check our [Collaboration Guidelines](Docs/CodeOfConduct.md).

* If you want to help [translate](https://kelteseth.gitlab.io/ScreenPlayDocs/contribute/translations/)
* If you are a [designer](https://kelteseth.gitlab.io/ScreenPlayDocs/contribute/contribute/#design)
* Help create example app via [HTML/QML/Javascript](https://kelteseth.gitlab.io/ScreenPlayDocs/)


# Contributing for programmer
 * If you want to contribute but don't know how to start, take a look at our open issues and WIP merge request.
 * If you need help don't hesitate to ask me (Keltseth) via:
     * [Create a forum topic with a detailed description](https://forum.screen-play.app/category/2/general-discussion)
     * [discord channel general](https://discord.gg/3RygPHZ)
 * [Developer setup guide on how to download and compile ScreenPlay yourself.](Docs/DeveloperSetup.md)
 * [ScreenPlay project overview](Docs/ProjectOverview.md)
 * [Developer C++ Classes Documentation](https://kelteseth.gitlab.io/ScreenPlayDeveloperDocs/)

# Platform support

* ❌ Not working/Not implemented
* ❓ Only partical implemented/Not tested

<div align="center">

| Feature                	    | Windows 	    | Linux 	        | MacOS 	|
|------------------------	    |---------	    |-------	        |-------	|
| __ScreenPlay Main App__       | ✔       	    | ✔     	        | ✔     	|
| __Steam Binaries__            | ✔       	    | ❌     	        | ❌     	|
| __Wallpaper__                 | ✔       	    | ❌ Help Needed for Gnome/KDE/etc!               | ✔    |
| __Widgets__                   | ✔       	    | ❓   	| ✔    	|
| __Multilanguage (EN,DE,RU,FR,ES,KO🆕,VI🆕)__              | ✔            	| ✔     	        |  ✔     	|

</div>

Because every operating system has his own version of desktop environment we need to adapt the ScreenPlayWindow for every platform seperate. The most feature complete for now is Windows 10. Windows 7 works but the wallpaper have the [worng coordinates](https://gitlab.com/kelteseth/ScreenPlay/issues/34). MacOS has some basic wallpaper functionality but no maintainer. For Linux we sadly have no support for any desktops environments at the moment.

__If you want to help and add new desktop environments look at ScreenPlayWallpaper/src folder__

* [BaseWindow](https://gitlab.com/kelteseth/ScreenPlay/blob/dev/ScreenPlayWallpaper/src/basewindow.h) baseclass for:
    * [LinuxWindow](https://gitlab.com/kelteseth/ScreenPlay/blob/dev/ScreenPlayWallpaper/src/linuxwindow.h)
    * [WinWindow](https://gitlab.com/kelteseth/ScreenPlay/blob/dev/ScreenPlayWallpaper/src/winwindow.h)
    * [MacWindow](https://gitlab.com/kelteseth/ScreenPlay/blob/dev/ScreenPlayWallpaper/src/macwindow.h)

<div align="center">

| Plattform                	    | Windows 7	       | Windows 8/8.1 	    | Windows 10  |   Gnome	         | KDE 	            | XFCE  	        | MacOS  	|
|------------------------	    |---------	       |-------	            |-------	  |---------	     |-------	        |-------	        | -------	|
| __Wallpaper__                 | ❓ Help Needed!   | ❓ Help Needed!    | ✔      	| ❌ Help Needed!  | ❌ Help Needed!  |❌ Help Needed!   | ❓ (Basic implementation) Help Needed!     	|


</div>

