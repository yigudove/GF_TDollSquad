#include "BaseAnimInstance.h"
#include "BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

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
        bAiming = AnimCharacter->IsAiming();


        FRotator AimRotation = AnimCharacter->GetBaseAimRotation();
        FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(AnimCharacter->GetVelocity());
        YawOffset = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

        CharacterRotationLastFrame = CharacterRotation;
        CharacterRotation = AnimCharacter->GetActorRotation();
        const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
        const float Target = Delta.Yaw / DeltaTime;
        const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.f);
        Lean = FMath::Clamp(Interp, -90.f, 90.f);

        UE_LOG(LogTemp, Log, TEXT("Yaw: %f, Lean: %f"), YawOffset, Lean);
}
