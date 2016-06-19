// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
using UnrealBuildTool;

public class GameEd : ModuleRules
{
	public GameEd(TargetInfo Target)
	{
		PrivateDependencyModuleNames.AddRange( new string[] {
				"Core",
				"CoreUObject",
                "Engine",
				"UnrealEd",
				"AssetTools",
				"ContentBrowser",
                "ClassViewer",
                "KismetCompiler",
				"Game"
			} );
	}
}