// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BuildSystem : ModuleRules
{
    public BuildSystem(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "AIModule",
            "StateTreeModule",
            "GameplayStateTreeModule",
            "UMG",
            "GameplayTags"
        });

        PrivateDependencyModuleNames.AddRange(new string[] { });

        PublicIncludePaths.AddRange(new string[] {
            "BuildSystem",
            "BuildSystem/Variant_Platforming",
            "BuildSystem/Variant_Combat",
            "BuildSystem/Variant_Combat/AI",
            "BuildSystem/Variant_SideScrolling",
            "BuildSystem/Variant_SideScrolling/Gameplay",
            "BuildSystem/Variant_SideScrolling/AI"
        });

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
