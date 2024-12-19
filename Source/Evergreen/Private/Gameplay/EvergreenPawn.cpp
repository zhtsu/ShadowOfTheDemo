// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/EvergreenPawn.h"

#include "CineCameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Common/CommonMacro.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"
#include "Gameplay/EvergreenGameInstance.h"

AEvergreenPawn::AEvergreenPawn()
{
	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootScene->Mobility = EComponentMobility::Movable;
	RootComponent = RootScene;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 2000;
	SpringArm->SetRelativeRotation(FRotator(-25, 0, 0));
	SpringArm->SetRelativeLocation(FVector(0, 0, 40));
	SpringArm->bDoCollisionTest = false;
	
	Camera = CreateDefaultSubobject<UCineCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	
	FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Movement"));

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	SpringArm->bUsePawnControlRotation = false;
	SpringArm->bInheritYaw = false;
	Camera->bUsePawnControlRotation = false;
}

void AEvergreenPawn::BeginPlay()
{
	Super::BeginPlay();

	if (Controller && Controller->GetControlRotation() != SpringArm->GetRelativeRotation())
		Controller->SetControlRotation(SpringArm->GetRelativeRotation());
}

void AEvergreenPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEvergreenGameInstance::OwnedPlayerInputComponent = PlayerInputComponent;
}

void AEvergreenPawn::AddMappingContext()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MappingContext, 0);
		}
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(UEvergreenGameInstance::OwnedPlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AEvergreenPawn::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AEvergreenPawn::Look);
	}
}

void AEvergreenPawn::RemoveMappingContext()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();
		}
	}
}

void AEvergreenPawn::Move(const FInputActionValue& InputActionValue)
{
	UEvergreenGameInstance* EGI = UEvergreenGameInstance::GetEvergreenGameInstance();
	if (!EGI->IsTestModeEnabled()) return;

	const FVector MovementVector = InputActionValue.Get<FVector>();

	if (Controller != nullptr)
	{
		const FRotator ControlRotation = SpringArm->GetRelativeRotation();
		
		const FVector ForwardDirection = FRotationMatrix(ControlRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(ControlRotation).GetUnitAxis(EAxis::Y);
		const FVector UpVector = FRotationMatrix(ControlRotation).GetUnitAxis(EAxis::Z);
		
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
		AddMovementInput(UpVector, MovementVector.Z);
	}
}

void AEvergreenPawn::Look(const FInputActionValue& InputActionValue)
{
	UEvergreenGameInstance* EGI = UEvergreenGameInstance::GetEvergreenGameInstance();
	if (!EGI->IsTestModeEnabled()) return;

	const FVector LookVector = InputActionValue.Get<FVector>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookVector.X);
		AddControllerPitchInput(LookVector.Y);
		AddControllerRollInput(LookVector.Z);
		SpringArm->SetRelativeRotation(Controller->GetControlRotation());
	}
}

void AEvergreenPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (!bCameraOffsetFollowCursorEnabled) return;
	if (!UEvergreenGameInstance::GetEvergreenGameInstance()->IsInteractionMode()) return;

	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (!PlayerController) return;
	
	FVector2D MousePosition;
	PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y);
	
	FIntPoint ViewportSize;
	PlayerController->GetViewportSize(ViewportSize.X, ViewportSize.Y);

	FVector2D ViewportCenter = FVector2D(ViewportSize) * 0.5;

	bool bIsMouseInViewport = (MousePosition.X >= 0 && MousePosition.X <= ViewportSize.X)
		&& (MousePosition.Y >= 0 && MousePosition.Y <= ViewportSize.Y);
	
	if (!bIsMouseInViewport)
	{
		MouseOffset = FMath::Vector2DInterpTo(MouseOffset,
			FVector2D::ZeroVector, DeltaSeconds, InterpSpeed);
	}
	else
	{
		MouseOffset = MousePosition - ViewportCenter;
	}
	
	FVector TargetOffset = FVector(0.f,
		MouseOffset.X * CameraOffsetScale_X,
		MouseOffset.Y * CameraOffsetScale_Y);

	SpringArm->SocketOffset = TargetOffset;

	FAST_PRINT(TargetOffset.ToString())
}
