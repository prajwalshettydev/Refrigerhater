#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <Camera/CameraComponent.h>
#include "RHCharacterDisplay.generated.h"

UCLASS()
class REFRIGERHATER_API ARHCharacterDisplay : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ARHCharacterDisplay();
	UPROPERTY(EditDefaultsOnly)
	USkeletalMeshComponent* MeshComp;
	UPROPERTY(EditDefaultsOnly)
	UCameraComponent* ViewCamera;

	UFUNCTION()
	void SetCharacter(const URHCharacterSelectionType* newSelection);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
