// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AControl : ModuleRules
{
	public AControl(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] 
		{ "Core", "CoreUObject", "Engine", "InputCore", "UMG","HeadMountedDisplay", "MoviePlayer" });

        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore", "EnhancedInput" });
    }
}
