function Component()
{
    // Install to @RootDir@ instead of @HomeDir@ on Windows
    if (installer.value("os") === "win") {
        var homeDir = installer.value("HomeDir");
        var targetDir = installer.value("TargetDir").replace(homeDir, "@RootDir@");
        installer.setValue("TargetDir", targetDir);
    }

    // do not show component selection page
    installer.setDefaultPageVisible(QInstaller.ComponentSelection, false);
    // no startmenu entry so no need to ask where to create it
    installer.setDefaultPageVisible(QInstaller.StartMenuSelection, false);
}
