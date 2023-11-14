// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProcDungeonCrawler : ModuleRules
{
	public ProcDungeonCrawler(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] 
			{ "Core", "CoreUObject", "Engine", "InputCore", "UMG", "EnhancedInput", "Niagara", "PCG", "ActorSequence" }
		);
		
		PrivateDependencyModuleNames.AddRange( new string[] { "Slate", "SlateCore", "PCG" });
	}
}
