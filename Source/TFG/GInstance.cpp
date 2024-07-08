// Fill out your copyright notice in the Description page of Project Settings.


#include "GInstance.h"

#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"

void UGInstance::Init()
{
	Super::Init();

	if (GEngine)
	{
		UGameUserSettings* UserSettings = GEngine->GameUserSettings;
		if (UserSettings)
		{
			UserSettings->SetScreenResolution(UserSettings->GetDesktopResolution());
			UserSettings->SetFullscreenMode(EWindowMode::WindowedFullscreen);

			UserSettings->LoadSettings(false);
			UserSettings->ApplySettings(false);
		}
	}

	// Se inicializan los parametros de la instancia
	MapTemperature = EMapTemperature::Temperate;
	MapSeaLevel = EMapSeaLevel::Standard;

	WaterTileChance = 0.4;
}
