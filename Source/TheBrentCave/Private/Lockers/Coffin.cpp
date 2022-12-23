// Fill out your copyright notice in the Description page of Project Settings.


#include "Lockers/Coffin.h"



void ACoffin::BeginPlay()
{
	Super::BeginPlay();

	bIsPeaking = true;

	SetupPeakCurve();

	//FSoftClassPath peakClassRef(TEXT("/Game/_Main/Locker/Coffin/PeakWidget.PeakWidget_C"));

	//if (UClass* widgetClass = peakClassRef.TryLoadClass<UUserWidget>()) {
	//	PeakWidget = CreateWidget<UUserWidget>(GetWorld(), widgetClass);
	//}
}


void ACoffin::EnterLocker()
{
	//Super::EnterLocker();

	if (!IsHiding) {
		IsHiding = true;

		if (HitBoxPlayer) {
			GetWorldTimerManager().ClearAllTimersForObject(this);

			HitBoxPlayer->isHiding = true;
			InteractComponent->SetVisibility(false);

			HitBoxPlayer->SetActorEnableCollision(false);
			DoorTimeline.PlayFromStart();

			OuterLockerLight->SetVisibility(true);
			InnerLockerLight->SetVisibility(true);
			HitBoxPlayer->SetActorHiddenInGame(true);
			GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(this, 0.4f);
			InputComponent->BindAxis("Turn", this, &ALocker::AddCameraYaw);
			InputComponent->BindAxis("LookUp", this, &ALocker::AddCameraPitch);

			GetWorldTimerManager().SetTimer(PeakHandle, this, &ACoffin::EnablePeak, 1.0f, false, 1.0f);

			EnableInput(GetWorld()->GetFirstPlayerController());
		}
	}
}


void ACoffin::LeaveLocker()
{
	if (IsHiding) {
		IsHiding = false;

		if (HitBoxPlayer) {
			GetWorldTimerManager().ClearAllTimersForObject(this);

			HitBoxPlayer->isHiding = false;
			InteractComponent->SetVisibility(true);

			DoorTimeline.PlayFromStart();

			OuterLockerLight->SetVisibility(false);
			InnerLockerLight->SetVisibility(false);
			HitBoxPlayer->SetActorHiddenInGame(false);
			GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(HitBoxPlayer, 0.6f);
			InputComponent->AxisBindings.Empty();

			// Swap Binding
			InputComponent->RemoveActionBinding("Interact", IE_Pressed);
			InputComponent->BindAction("Interact", IE_Pressed, this, &ACoffin::EnterLocker);

			InputComponent->RemoveActionBinding("Interact", IE_Released);

			if (PeakWidget && PeakWidget->IsInViewport())
			{
				PeakWidget->RemoveFromParent();
				//CollectGarbage(RF_NoFlags, true);
			}

			GetWorldTimerManager().SetTimer(PeakHandle, this, &ACoffin::EnableCollision, 0.7f, false, 0.7f);
		}
		////DisableInput(GetWorld()->GetFirstPlayerController());
	}
}


void ACoffin::StartPeak()
{

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Peaked.")));
	PeakTimeline.Play();
}


void ACoffin::EndPeak()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Unpeaked.")));
	PeakTimeline.Reverse();
}


void ACoffin::EnablePeak()
{
	// Swap Binding

	if (IsHiding)
	{
		InputComponent->RemoveActionBinding("Interact", IE_Pressed);
		InputComponent->BindAction("Interact", IE_Pressed, this, &ACoffin::StartPeak);

		InputComponent->BindAction("Interact", IE_Released, this, &ACoffin::EndPeak);

		if (PeakWidgetClass)
		{
			PeakWidget = CreateWidget<UUserWidget>(GetWorld(), PeakWidgetClass);
			PeakWidget->AddToViewport();
		}
	}
}


void ACoffin::EnableCollision()
{
	HitBoxPlayer->SetActorEnableCollision(true);
}

void ACoffin::SetupPeakCurve()
{
	if (PeakCurve) {
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindUFunction(this, FName("UpdateTimelineProgressPeak"));
		PeakTimeline.AddInterpFloat(PeakCurve, TimelineProgress);
	}
}


void ACoffin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PeakTimeline.TickTimeline(DeltaTime);
	//MyTimeline->TickComponent(DeltaTime, ELevelTick::LEVELTICK_TimeOnly, NULL);
}


void ACoffin::UpdateTimelineProgressPeak(float Value)
{
	float angle = FMath::Lerp(ClosedDoorAngle, OpenPeakAngle, Value);
	Pivot->SetRelativeRotation((FRotator(0.0f, angle, 0.0f)));
}