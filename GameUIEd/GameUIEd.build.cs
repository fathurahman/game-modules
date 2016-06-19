// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
using UnrealBuildTool;

public class GameUIEd : ModuleRules
{
    public GameUIEd(TargetInfo target)
    {
        PrivateIncludePaths.AddRange(new string[] { "GameUIEd/Private" });

        PublicDependencyModuleNames.AddRange(new string[] { "InputCore", "Slate", "SlateCore", "UMG", "UnrealEd", "GameUI" });

        PrivateDependencyModuleNames.AddRange(new string[] { "Engine", "Core", "EditorStyle", "CoreUObject", "ClassViewer", "UMGEditor", "AssetTools", "GameUI"});
    }
}