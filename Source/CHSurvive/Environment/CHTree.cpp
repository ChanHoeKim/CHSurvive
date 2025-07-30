// Fill out your copyright notice in the Description page of Project Settings.


#include "Environment/CHTree.h"

#include "CHLog.h"
#include "Animation/CHAnimInstance.h"
#include "Character/CHPlayerCharacter.h"
#include "Components/BoxComponent.h"

// Sets default values
ACHTree::ACHTree()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.3f;
}

// Called when the game starts or when spawned
void ACHTree::BeginPlay()
{
	Super::BeginPlay();

	
}

void ACHTree::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ACHTree::OnOverlapBegin);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &ACHTree::OnOverlapEnd);
}

void ACHTree::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult)
{

	
	ACHPlayerCharacter* OverlappingPawn = Cast<ACHPlayerCharacter>(OtherActor);
	if (OverlappingPawn)
	{
		OverlappingPawn->bBeReadyToAttack = true;
		OverlappingPawn->bShouldMove = false;
		OverlappingPawn->bTargetAttack = true;
		
		OverlappingPawn->OnOffTagetAutoAttack(true);
		//OverlappingPawn->bTargetAttack = true;
		
	}
}

void ACHTree::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	ACHPlayerCharacter* OverlappingPawn = Cast<ACHPlayerCharacter>(OtherActor);
	if (OverlappingPawn)
	{
		OverlappingPawn->bBeReadyToAttack = false;
		//OverlappingPawn->bShouldMove = false;
		OverlappingPawn->bTargetAttack = false;
		
		OverlappingPawn->OnOffTagetAutoAttack(false);
		
		//OverlappingPawn->bTargetAttack = true;
	}
}

void ACHTree::ServerRPC_DestroyTree_Implementation()
{
	SpawnLog();
	//Destroy();
}

void ACHTree::SpawnLog()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	FVector SpawnLocation = GetActorLocation();
	FRotator SpawnRotator = GetActorRotation();

	// ACHLog* SpawnedActor = GetWorld()->SpawnActor<ACHLog>(ACHLog::StaticClass(), SpawnLocation, SpawnRotator);
	ACHLog* SpawnedActor = GetWorld()->SpawnActorDeferred<ACHLog>(LogClassToSpawn, GetActorTransform(), nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	if (SpawnedActor)
	{
		SpawnedActor->FinishSpawning(GetActorTransform());

		Destroy();
	}
}

void ACHTree::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// if (GEngine)
	// {
	// 	GEngine->AddOnScreenDebugMessage(
	// 			-1, // Key (고유 ID, -1이면 자동으로 갱신됨)
	// 				5.0f, // Duration (화면에 표시될 시간, 초 단위)
	// 					FColor::Green, // 텍스트 색상
	// 						TEXT("Tick") // 출력할 메시지
	// 						);
	// }

	
	if (TreeHp <= 0)
	{
		if (HasAuthority())
		{
			//Destroy();
			SpawnLog();
			//FActorSpawnParameters SpawnParameters;
		}
		else
		{
			ServerRPC_DestroyTree();
		}
	}
}

float ACHTree::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
                          class AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	//실수 : 포맷을 d로 지정하면 0이 출력 됨
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
				-1, // Key (고유 ID, -1이면 자동으로 갱신됨)
					5.0f, // Duration (화면에 표시될 시간, 초 단위)
						FColor::Green, // 텍스트 색상
							FString::Printf(TEXT("%f"), ActualDamage) // 출력할 메시지
							);
	}

	TreeHp -= ActualDamage;
	
	return ActualDamage;
}






