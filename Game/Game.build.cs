// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer

using UnrealBuildTool;

public class Game : ModuleRules
{
	public Game(TargetInfo Target)
	{
		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"CoreUObject",
				"Engine",
				"AssetRegistry",
                "ImageWrapper"
			}
		);

        if (Target.Type == TargetRules.TargetType.Editor)
        {
            PrivateDependencyModuleNames.AddRange(new string[] {
                "UnrealEd"
            });
        }

    }
}