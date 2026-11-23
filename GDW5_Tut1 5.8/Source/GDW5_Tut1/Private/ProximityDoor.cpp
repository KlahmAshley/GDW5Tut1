// Fill out your copyright notice in the Description page of Project Settings.



#include "ProximityDoor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"

// Sets default values
AProximityDoor::AProximityDoor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PrimaryActorTick.bStartWithTickEnabled = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Hinge = CreateDefaultSubobject<USceneComponent>(TEXT("Hinge"));
	Hinge->SetupAttachment(RootComponent);

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(Hinge);

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Trigger->SetupAttachment(RootComponent);

	Trigger->SetRelativeLocation(FVector(0.0f, 150.0f, 100.0f));
	Trigger->SetBoxExtent(FVector(300.0f, 250.0f, 150.0f));
	Trigger->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));


}

// Called when the game starts or when spawned
void AProximityDoor::BeginPlay()
{
	Super::BeginPlay();

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AProximityDoor::OnTriggerBegin);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &AProximityDoor::OnTriggerEnd);



}

// Called every frame
void AProximityDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentAngle = FMath::FInterpTo(CurrentAngle, TargetAngle, DeltaTime, OpenSpeed);
	Hinge->SetRelativeRotation(FRotator(0.0f, CurrentAngle, 0.0f));

	if (FMath::IsNearlyEqual(CurrentAngle, TargetAngle, 1.0f))
	{
		CurrentAngle = TargetAngle;
		Hinge->SetRelativeRotation(FRotator(0.0f, CurrentAngle, 0.0f));
		SetActorTickEnabled(false);

	}

}


void AProximityDoor::OnTriggerBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APawn* Pawn = Cast<APawn>(OtherActor);
	if (Pawn == nullptr || !Pawn->IsPlayerControlled())
	{
		return;
	}

	PlayersInside++;
	if (PlayersInside == 1)
	{
		TargetAngle = OpenAngle;
		SetActorTickEnabled(true);
		OnDoorOpening();
	}
}

void AProximityDoor::OnTriggerEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APawn* Pawn = Cast<APawn>(OtherActor);
	if (Pawn == nullptr || !Pawn->IsPlayerControlled())
	{
		return;
	}

	PlayersInside = FMath::Max(PlayersInside - 1, 0);
	if (PlayersInside == 0)
	{
		TargetAngle = 0.0f;
		SetActorTickEnabled(true);
		OnDoorClosing();
	}
}
