# Contributing to ScreenPlay

### Basic

Every changes to ScreenPlay are done via Gitlab issues and merge requests (pull request for github people). These follow the normal contributing pattern: press the big blue fork button at the top of the repository, make changes, open a merge request.

### Development

1.  Accept the Contributing Agreement:
    *  [For regular contributors](https://gitlab.com/kelteseth/ScreenPlay/blob/dev/Legal/individual_contributor_license_agreement.md)
    *  [For members of a company](https://gitlab.com/kelteseth/ScreenPlay/blob/dev/Legal/corporate_contributor_license_agreement.md)
2.  Download and set up ScreenPlay as described in the main [Readme](https://gitlab.com/kelteseth/ScreenPlay).
2.  Choose an issue to work on:
    *   Your own ideas or thinks you want to improve. Alternatively you can tackle existing open issues:
    *   You can find simple problems by [searching for problems labeled "Accepting merge requests" sorted by weight.](https://gitlab.com/kelteseth/ScreenPlay/issues?state=opened&label_name[]=Accepting+merge+requests&assignee_id=0&sort=weight). Low-weight issues will be the easiest to accomplish.
    *   Make sure that no one else is working on the problem and make sure that we are still interested in a particular post.
    *   You can also make comments and ask for help if you are new or stuck. We will be happy to help you!
3.  Add the feature or fix the bug you’ve chosen to work on.
4.  Open a merge request to merge your code and its documentation (preferably via [draw.io]( http://draw.io/) embedded graphs). The sooner you open a merge request, the sooner you can get feedback. You can mark it as Work in Progress to indicate that you are not finished.
6.  Make sure the CI builds are passing.
7.  Make sure to update the translations if you've added any text via the qsTr("text) macro. It is ok to not translate your text for all the supported languages. You can always ask other contributors from other countries to verify your translations.
7.  Wait for a reviewer. You’ll likely need to change some things once the reviewer has completed a code review for your merge request. You may also need multiple reviews depending on the size of the change.
8.  Get your changes merged!


### Programming rules

1.  Use the Qt naming conventions for [C++](https://wiki.qt.io/Coding_Conventions) and [QML](https://doc.qt.io/qt-5/qml-codingconventions.html)
2.  Use all the available Qt container and classes for things like (Q)Strings and (Q)Vector.
3.  Use the c++17 std smart pointer.
4.  Use the webkit formatting style. You can automatically format your code via the Beautifier QtCreator plugin:
    *   To activate this plugin you have to navigate inside your QtCreator: Help -> About Plugins -> Beautifier. 
    *   [Download and install LLVM](http://releases.llvm.org/download.html) to format the code
    *   In QtCreator navigate to: Tools -> Options -> Beautifier -> Clang format -> Options -> Use predefined style: Webkit
    *   Add a formatting shortcut via: Tools -> Options -> Environment -> Keyboard -> Search for "format" and add your shortcut to ClangFormat, FormatFile
3.  Use this [git commit message style](https://gist.github.com/robertpainsi/b632364184e70900af4ab688decf6f53)

### Documentation

To contribute to the [documentation](https://kelteseth.gitlab.io/ScreenPlayDocs/) you only have to press the pen icon on the top right of every page. We mostly use [draw.io]( http://draw.io/) for every diagram because it is free and you can embed the code inside a png when exporting the document. 

### Translation

We use the [Qt Linguist] for translating ScreenPlay. This tool lets you open the ScreenPlay_<language>.ts files and edit these easily. If is included in the open source Qt SDK and normally inside your Qt bin folder: "C:\Qt\5.XX.0\msvc2017_64\bin\linguist.exe" 

### Design

To design ScreenPlay we use [Affinity Designer](https://affinity.serif.com/en-gb/designer/) as superior alternative to Photoshop. But you can use any tool you like. If you have any ideas on how to improve any design aspect of ScreenPlay just open a issue with the "Design" tag and paste you mockups into the description (preferably as png or/and the source *.afdesign file) 