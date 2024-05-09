// Fill out your copyright notice in the Description page of Project Settings.


#include "RHCharacterDisplay.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "RHCharacterSelectionType.h"

// Sets default values
ARHCharacterDisplay::ARHCharacterDisplay()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>("RootComponent");

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>("MeshComp");
	MeshComp->SetupAttachment(RootComponent);	
	
	ViewCamera = CreateDefaultSubobject<UCameraComponent>("ViewCamera");
	ViewCamera->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ARHCharacterDisplay::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARHCharacterDisplay::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARHCharacterDisplay::SetCharacter(const URHCharacterSelectionType* newSelection)
{
	if (!newSelection)
		return;
	UE_LOG(LogTemp, Warning, TEXT("CHAR NAME: %s"), *newSelection->charName.ToString())
	MeshComp->SetSkeletalMesh(newSelection->GetSkelMesh());
	MeshComp->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	MeshComp->AnimClass = newSelection->charIdleAnim;
}

