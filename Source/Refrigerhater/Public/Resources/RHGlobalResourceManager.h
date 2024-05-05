// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RHGlobalResourceManager.generated.h"

UCLASS()
class REFRIGERHATER_API ARHGlobalResourceManager : public AActor
{
	GENERATED_BODY()

	

protected:

	FTimerHandle TimerHandle_Resource1;
	FTimerHandle TimerHandle_Resource2;

	UPROPERTY(EditAnywhere, Category = "Resources")
	TArray<TSubclassOf<class ARHResourceBase>> ResourceClassOrdinary;

	UPROPERTY(EditAnywhere, Category = "Resources")
	TArray<TSubclassOf<class ARHResourceBase>> ResourceClassEpic;

	// Box component to define the spawn area
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawn")
	class UBoxComponent* SpawnVolume;

	UPROPERTY(EditAnywhere, Category = "Resources")
	int32 MaxResource1Count = 10;

	UPROPERTY(EditAnywhere, Category = "Resources")
	int32 MaxResource2Count = 5;
	
	// // Number of resources to spawn
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	// int32 ResourceCount;

	TArray<AActor*> SpawnedResources1;
	TArray<AActor*> SpawnedResources2;


private:
	
	void SpawnResource1();
	void SpawnResource2();
	FVector GenerateRandomPointInBox();
	void SpawnResourceAtLocation(const TArray<TSubclassOf<ARHResourceBase>>& Resources, const FVector& Location);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:
	// Sets default values for this actor's properties
	ARHGlobalResourceManager();
};