#include "BaseCharacter.h"

#include  "OnlineSessionSettings.h"

#include "Framework/Application/SlateApplication.h"
#include "GameFramework/GameUserSettings.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "OnlineSubsystem.h"

ABaseCharacter::ABaseCharacter():
CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete ))
{
	PrimaryActorTick.bCanEverTick = true;

	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CharacterCameraSpringArm"));
	CameraSpringArm->SetupAttachment(GetMesh());
	CameraSpringArm->TargetArmLength = 300.0f;
	CameraSpringArm->bUsePawnControlRotation = true;

	CharacterCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("CharacterFollowCamera"));
	CharacterCamera->SetupAttachment(CameraSpringArm);
	CharacterCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	IOnlineSubsystem *OSS = IOnlineSubsystem::Get();
	if(OSS)
	{
		OSSInterface = OSS->GetSessionInterface();
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.0f,
				FColor::Blue,
				FString::Printf(TEXT("Found OSS: %s"), *OSS->GetSubsystemName().ToString()));
		}
	}
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMapping, 0);
		}
	}
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* PEI = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		PEI->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABaseCharacter::Move);
		PEI->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABaseCharacter::Look);
		PEI->BindAction(InteractAction, ETriggerEvent::Started, this, &ABaseCharacter::Interact);
		PEI->BindAction(QuitGameAction, ETriggerEvent::Started, this, &ABaseCharacter::QuitGame);
	}
}

void ABaseCharacter::Move(const FInputActionValue& Value)
{
	// 转为2d向量
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// 找出是哪个方向向前 获取控制旋转
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);//inPich和inRoll都为0但是yaw 也就是方向取控制旋转的方向
		//ue坐标系 前面 是x 垂直是z水平 是y
		// 获取向前向量
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

    
		//获取向右向量
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ABaseCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);//围绕x轴 [ue-x代表向前的水平线]偏航叫 控制左右旋转 z代表高度垂直线
		AddControllerPitchInput(LookAxisVector.Y);//围绕y轴 [ue是水平线]俯仰角 roll  在unreal中，pitch 对应于y轴，yaw对应于z轴，roll对应于x轴。
	}
}

void ABaseCharacter::Interact(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("Trigger Interact Action."));

	// UGameUserSettings* MyGameSettings = GEngine->GetGameUserSettings();
	// // MyGameSettings->SetFullscreenMode(EWindowMode::Fullscreen);
	// MyGameSettings->SetFullscreenMode(EWindowMode::Windowed);
	// MyGameSettings->ApplySettings(true);
	if(GEngine->GetGameUserSettings()->GetFullscreenMode()==EWindowMode::Fullscreen)
	{
		GEngine->GetGameUserSettings()->SetFullscreenMode(EWindowMode::Windowed);
		GEngine->GetGameUserSettings()->SetScreenResolution(FIntPoint(1800, 1000));
		GEngine->GetGameUserSettings()->ApplySettings(true);
	}
	else
	{
		GEngine->GetGameUserSettings()->SetFullscreenMode(EWindowMode::Fullscreen);
		GEngine->GetGameUserSettings()->ApplySettings(true);
	}

}

void ABaseCharacter::QuitGame(const FInputActionValue& Value)
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		PlayerController->ConsoleCommand("quit");
	}
}

void ABaseCharacter::CreateGameSession()
{
	if(!OSSInterface.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("!OSSInterface.IsValid()"));
		return;
	}
	
	GEngine->AddOnScreenDebugMessage(
	-1,
	15.0f,
	FColor::Blue,
	FString::Printf(TEXT("Try Create Session"))
	);
	
	auto ExistingSession = OSSInterface->GetNamedSession(NAME_GameSession);
	if(ExistingSession != nullptr)
	{
		OSSInterface->DestroySession(NAME_GameSession);
	}

	OSSInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);
	TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings() );

	SessionSettings->bIsLANMatch = false;
	SessionSettings->NumPublicConnections = 8;
	SessionSettings->bAllowJoinInProgress = true;
	SessionSettings->bAllowJoinViaPresence = true;
	SessionSettings->bShouldAdvertise = true;
	SessionSettings->bUsesPresence = true;
	SessionSettings->bUseLobbiesIfAvailable = true;

	SessionSettings->Set(FName("MatchType"), FString("FreeForAll"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	
	const ULocalPlayer *LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	OSSInterface->CreateSession(
		*LocalPlayer->GetPreferredUniqueNetId(),
		NAME_GameSession,
		*SessionSettings
		);
}

void ABaseCharacter::JoinGameSession()
{
	if(!OSSInterface.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("!OSSInterface.IsValid()"));
		return;
	}

	GEngine->AddOnScreenDebugMessage(
	-1,
	15.0f,
	FColor::Blue,
	FString::Printf(TEXT("Try Join Session"))
	);

	OSSInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);
	
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->MaxSearchResults = 10000;
	SessionSearch->bIsLanQuery = false;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	const ULocalPlayer *LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	OSSInterface->FindSessions(
		*LocalPlayer->GetPreferredUniqueNetId(),
		SessionSearch.ToSharedRef()
	);
}

void ABaseCharacter::OnCreateSessionComplete(FName SessionName, bool bSuccess)
{
	auto ExistingSession = OSSInterface->GetNamedSession(NAME_GameSession);
	
	if(bSuccess)
	{
		GEngine->AddOnScreenDebugMessage(
				-1,
				15.0f,
				FColor::Blue,
				FString::Printf(TEXT("Create Session: %s"), *SessionName.ToString())
				);
        GEngine->AddOnScreenDebugMessage(
            -1,
            15.0f,
            FColor::Blue,
            FString::Printf(TEXT("Create Session Id: %s"), *ExistingSession->GetSessionIdStr())
        );
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(
						-1,
						15.0f,
						FColor::Blue,
						FString::Printf(TEXT("Failed to create session"))
						);
	}
}

void ABaseCharacter::OnFindSessionsComplete(bool bSuccess)
{
	if(!OSSInterface.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("!OSSInterface.IsValid()"));
		return;
	}
	
	for(auto Result : 	SessionSearch->SearchResults)
	{
		FString Id = Result.GetSessionIdStr();
		FString OwningUser = Result.Session.OwningUserName;
		FString MatchType;
		Result.Session.SessionSettings.Get(FName("MatchType"), MatchType);
		GEngine->AddOnScreenDebugMessage(
		-1,
		15.0f,
		FColor::Cyan,
		FString::Printf(TEXT("Session Id: %s, Owning User: %s"), *Id, *OwningUser)
		);

		if (MatchType == FString("FreeForAll"))
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(
					-1,
					15.f,
					FColor::Cyan,
					FString::Printf(TEXT("Joining Match Type: %s"), *MatchType)
				);
			}

			// OSSInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);

			// const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
			// OSSInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, Result);
		}
	}

}
