// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer

using UnrealBuildTool;

public class GameUI : ModuleRules
{
    public GameUI(TargetInfo target)
    {
        PrivateIncludePaths.AddRange(new string[] { "GameUI/Private" });

        PublicDependencyModuleNames.AddRange(new string[] { 
        	"InputCore", 
        	"Slate", 
        	"SlateCore", 
        	"UMG",
        	"Game" 
        });

        PrivateDependencyModuleNames.AddRange(new string[] { 
        	"Engine", 
        	"Core", 
        	"EditorStyle", 
        	"CoreUObject"
        });
    }
}