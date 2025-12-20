// KJY

#include "SVFlipbookEnemy.h"
#include "PaperFlipbookComponent.h"

ASVFlipbookEnemy::ASVFlipbookEnemy(const FObjectInitializer& ObjectInitializer)
{
	PaperFlipbookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("PaperFlipbookComponent"));
	PaperFlipbookComponent->SetupAttachment(RootComponent);
}
