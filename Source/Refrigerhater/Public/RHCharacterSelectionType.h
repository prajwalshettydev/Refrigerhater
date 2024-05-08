#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RHCharacterSelectionType.generated.h"

/**
 * 
 */
UCLASS()
class REFRIGERHATER_API URHCharacterSelectionType : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	FName charName;
	UPROPERTY(EditDefaultsOnly)
	UTexture* charIcon;	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ACharacter> charClass;	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UAnimInstance> charIdleAnim;

	USkeletalMesh* GetSkelMesh() const;
};
