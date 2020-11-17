// Fill out your copyright notice in the Description page of Project Settings.


#include "SwipeComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
USwipeComponent::USwipeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;


	bTouched = false;
	SwipeAmount = 50.0f;
	SwipeTime = 0.0f;
	bSwipeTriggered = false;
	bSwipeEnded = true;
	TouchStartLocation = TouchEndLocation = FVector2D{ 0.0f };
}


// Called when the game starts
void USwipeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	UE_LOG(LogTemp, Warning, TEXT("Swipe component!"));

}


// Called every frame
void USwipeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	this->CheckSwipe();
}

void USwipeComponent::TouchBegin()
{
	UE_LOG(LogTemp, Warning, TEXT("Touch Begin"));

	bTouched = true;
	TouchStartLocation = GetTouchCoordinates();
	SwipeStartTime = GetWorld()->GetTimeSeconds();
}

void USwipeComponent::TouchEnd()
{
	UE_LOG(LogTemp, Warning, TEXT("Touch End"));
	
	bTouched = false;
	bSwipeTriggered = false;
	bSwipeEnded = true;
	SwipeEndTime = 0.0f;
	SwipeStartTime = 0.0f;
}

FVector2D USwipeComponent::GetTouchCoordinates()
{
	if (!GetWorld()) return FVector2D{ 0.0f };

	APlayerController* PController = GetWorld()->GetFirstPlayerController();
	float LocX, LocY;
	bool IsCurrentlyPressed;
	PController->GetInputTouchState(ETouchIndex::Touch1, LocX, LocY, IsCurrentlyPressed);

	return FVector2D{ LocX, LocY };
}

void USwipeComponent::CheckSwipe()
{
	if (!GetWorld()) return;

	if (bTouched)
	{

		SwipeEndTime = GetWorld()->GetTimeSeconds();
		SwipeTime = SwipeEndTime - SwipeStartTime;

		float TouchDistance = FVector2D::Distance(GetTouchCoordinates(), TouchStartLocation);
		if (TouchDistance > SwipeAmount)
		{
			bSwipeTriggered = true;
		}

		if (bSwipeTriggered && bSwipeEnded && SwipeTime < 0.3f)
		{
			bSwipeEnded = false;
			TouchEndLocation = GetTouchCoordinates();

			FVector2D Offset = UKismetMathLibrary::Subtract_Vector2DVector2D(TouchEndLocation, TouchStartLocation);

			if (FMath::Abs(Offset.X) > FMath::Abs(Offset.Y))
			{
				if (Offset.X > 0.0f)
				{
					SwipeRightDelegate.Broadcast();
				}
				else
				{
					SwipeLeftDelegate.Broadcast();
				}
			}
			else
			{
				if (Offset.Y > 0.0f)
				{
					SwipeDownDelegate.Broadcast();
				}
				else
				{
					SwipeUpDelegate.Broadcast();
				}
			}
		}
	}
}
