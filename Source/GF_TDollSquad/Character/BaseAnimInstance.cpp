#include "BaseAnimInstance.h"
#include "BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UBaseAnimInstance::NativeInitializeAnimation()
{
        Super::NativeInitializeAnimation();

        AnimCharacter = Cast<ABaseCharacter>(TryGetPawnOwner());
}

void UBaseAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
        Super::NativeUpdateAnimation(DeltaTime);

        if (AnimCharacter == nullptr)
        {
                AnimCharacter = Cast<ABaseCharacter>(TryGetPawnOwner());
        }
        if (AnimCharacter == nullptr) return;

        FVector Velocity = AnimCharacter->GetVelocity();
        Velocity.Z = 0.f;
        CharacterSpeed = Velocity.Size();

        bIsInAir = AnimCharacter->GetCharacterMovement()->IsFalling();
        bIsAccelerating = AnimCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f;
}
