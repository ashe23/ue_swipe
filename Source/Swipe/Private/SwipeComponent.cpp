// Fill out your copyright notice in the Description page of Project Settings.


#include "SwipeComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

USwipeComponent::USwipeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;


	bTouched = false;
	SwipeDistance = 50.0f;
	SwipeTriggerTime = 0.2f;
	SwipeTime = 0.0f;
	bSwipeTriggered = false;
	bSwipeEnded = true;
	TouchStartLocation = TouchEndLocation = FVector2D{ 0.0f };
}


void USwipeComponent::BeginPlay()
{
	Super::BeginPlay();

}


void USwipeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	this->CheckSwipe();
}

void USwipeComponent::TouchBegin()
{
	bTouched = true;
	TouchStartLocation = GetTouchCoordinates();
	SwipeStartTime = GetWorld()->GetTimeSeconds();
}

void USwipeComponent::TouchEnd()
{
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
		if (TouchDistance > SwipeDistance)
		{
			bSwipeTriggered = true;
		}

		if (bSwipeTriggered && bSwipeEnded && SwipeTime < SwipeTriggerTime)
		{
			bSwipeEnded = false;
			TouchEndLocation = GetTouchCoordinates();

			FVector2D Offset = UKismetMathLibrary::Subtract_Vector2DVector2D(TouchEndLocation, TouchStartLocation);

			if (FMath::Abs(Offset.X) > FMath::Abs(Offset.Y))
			{
				if (Offset.X > 0.0f)
				{
					OnSwipeRight.Broadcast();
				}
				else
				{
					OnSwipeLeft.Broadcast();
				}
			}
			else
			{
				if (Offset.Y > 0.0f)
				{
					OnSwipeDown.Broadcast();
				}
				else
				{
					OnSwipeUp.Broadcast();
				}
			}
		}
	}
}
