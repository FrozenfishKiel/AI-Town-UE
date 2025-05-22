// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class MassAITest : ModuleRules
{
	public MassAITest(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "InputCore" , "HeadMountedDisplay" ,
			"MassEntity" , "MassCommon" , "MassNavigation" , "MassMovement"
			, "NavigationSystem" , "AIModule" , "MassAIBehavior" ,  "GameplayTags" , "MassSmartObjects"
			, "MassSignals" , "MassRepresentation" , "SmartObjectsModule" , "HTTP" , "Json" , "JsonUtilities"
			, "VaRest"
		});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
