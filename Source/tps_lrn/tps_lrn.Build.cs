// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class tps_lrn : ModuleRules
{
	public tps_lrn(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });

		PublicIncludePaths.AddRange(new string[] { "tps_lrn" });
	}
}
