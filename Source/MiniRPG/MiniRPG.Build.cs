// 어떤 기능 덩어리들을 가져다 쓸지 적어두는 목록

using UnrealBuildTool;

public class MiniRPG : ModuleRules
{
	public MiniRPG(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core", "CoreUObject", "Engine", "InputCore",
			"EnhancedInput", "AIModule", "NavigationSystem",
			"UMG", "Slate", "SlateCore"});
		
		PublicIncludePaths.Add(ModuleDirectory);
	}
}
