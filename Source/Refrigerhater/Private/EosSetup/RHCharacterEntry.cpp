#include "EosSetup/RHCharacterEntry.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "EosSetup/RHCharacterSelectionType.h"

void URHCharacterEntry::SetChosen(bool value)
{
	SetIsEnabled(!value);
	charIconDisplay->GetDynamicMaterial()->SetScalarParameterValue(iconSatName, value ? 0 : 1);
}

void URHCharacterEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	URHCharacterSelectionType* charSelection = Cast<URHCharacterSelectionType>(ListItemObject);

	if (!charSelection)
		return;

	charNameDisplay->SetText(FText::FromName(charSelection->charName));
	charIconDisplay->GetDynamicMaterial()->SetTextureParameterValue(iconMatName, charSelection->charIcon);
	
}
