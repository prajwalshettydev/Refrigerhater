#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "RHCharacterEntry.generated.h"

/**
 * 
 */
UCLASS()
class REFRIGERHATER_API URHCharacterEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
public:
	void SetChosen(bool value);
protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
private:
	UPROPERTY(meta=(BindWidget))
	class UImage* charIconDisplay;	
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* charNameDisplay;
	UPROPERTY(EditDefaultsOnly)
	FName iconMatName{"Icon"};	
	UPROPERTY(EditDefaultsOnly)
	FName iconSatName{"Saturation"};	
};
