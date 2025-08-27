// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Actor.h"
#include "Interface/CHInteractInterface.h"
#include "CHShip.generated.h"

class UInputAction;
class UInputMappingContext;
class UFloatingPawnMovement;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class CHSURVIVE_API ACHShip : public APawn, public ICHInteractInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACHShip();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    // APawn 표준 입력 파이프라인 연결 (AddMovementInput → Movement)
    virtual UPawnMovementComponent* GetMovementComponent() const override;

protected:
    virtual void BeginPlay() override;
    virtual void PossessedBy(AController* NewController) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    // 입력 액션/컨텍스트 (에디터에서 할당)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess="true"))
    UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess="true"))
    UInputAction* MoveAction;

    // 컴포넌트
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Mesh")
    TObjectPtr<UCameraComponent> Camera;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Mesh")
    TObjectPtr<USpringArmComponent> SpringArm;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Mesh")
    TObjectPtr<UStaticMeshComponent> Mesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Placement")
    TObjectPtr<USceneComponent> AttachPoint;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement")
    TObjectPtr<UFloatingPawnMovement> Movement;

    // 서버가 사용하는 입력(디버그용, 선택)
    UPROPERTY(Replicated)
    FVector2D ServerMoveInput = FVector2D::ZeroVector;

    // 파라미터
    UPROPERTY(EditAnywhere, Category="Ship")
    float YawTurnRateDegPerSec = 60.f;
    
    UPROPERTY(EditAnywhere, Category="Ship")
    float SideFriction = 0.5f;

    // 고정 고도
    float FixedZ = 0.f;

private:
    // 입력 처리 (로컬 클라에서 호출)
    void OnMove(const FInputActionValue& Value);
    void OnMoveCompleted(const FInputActionValue& Value);

    // 서버 RPC: 클라 → 서버로 입력 전달 → 서버가 즉시 이동/회전 처리
    UFUNCTION(Server, Unreliable)
    void Server_SetMoveInput(FVector2D InMove);

public:
    // 인터랙트 (탑승)
    virtual void Interact(AActor* Interactor) override;

    UPROPERTY(EditAnywhere, Category="Ship|Input")
    bool bSwizzleYX = false;     // IMC에서 Up/Down이 X로, Left/Right가 Y로 들어오면 true

    UPROPERTY(EditAnywhere, Category="Ship|Input")
    bool bInvertForward = false; // 위(Up)가 +Y가 아니라 -Y로 들어오면 true (부호 뒤집기)
};
