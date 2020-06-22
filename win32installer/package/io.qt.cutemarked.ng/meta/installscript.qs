function Component()
{
}

Component.prototype.createOperations = function()
{
    component.createOperations();
     if (systemInfo.productType === "windows") {
        component.addOperation("CreateShortcut", "@TargetDir@/cutemarked.exe", "@StartMenuDir@/CuteMarkEd-NG.lnk",
            "workingDirectory=@HomeDir@", "description=Open CuteMarkEd-NG");
    }
}