#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BaseAnimInstance.generated.h"

UCLASS()
class GF_TDOLLSQUAD_API UBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
        virtual void NativeInitializeAnimation() override;
        virtual void NativeUpdateAnimation(float DeltaTime) override;

private:
        UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
        class ABaseCharacter* AnimCharacter;
        UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
        float CharacterSpeed;
        UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
        bool bIsInAir;
        UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
        bool bIsAccelerating;
        UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
        bool bAiming;
        UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
        float YawOffset;
        UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
        float Lean;

        FRotator CharacterRotationLastFrame;
        FRotator CharacterRotation;
};
