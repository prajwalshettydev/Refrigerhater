#include "EosSetup/RHCharacterSelectionType.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"

USkeletalMesh* URHCharacterSelectionType::GetSkelMesh() const
{
	if (!charClass)
		return nullptr;

	ACharacter* character = Cast<ACharacter>(charClass->GetDefaultObject());
	if (character)
		return character->GetMesh()->GetSkeletalMeshAsset();

	return nullptr;
}
