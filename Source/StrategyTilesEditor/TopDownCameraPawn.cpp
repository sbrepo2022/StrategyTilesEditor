// Fill out your copyright notice in the Description page of Project Settings.


#include "TopDownCameraPawn.h"

// Sets default values
ATopDownCameraPawn::ATopDownCameraPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("ComponentScene"));
	RootComponent = Scene;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(Scene);
	SpringArm->bDoCollisionTest = false;
	SpringArm->SetRelativeRotation(FRotator(-50, 0, 0));
	SpringArm->TargetArmLength += StartZoom;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
}

// Called when the game starts or when spawned
void ATopDownCameraPawn::BeginPlay()
{
	Super::BeginPlay();
	
	PC = Cast<APlayerController>(GetController());
	PC->GetViewportSize(ScreenSizeX, ScreenSizeY);
}

// Called every frame
void ATopDownCameraPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector PanDirection = GetCameraPanDirection();
	PanMoveCamera(PanDirection);
}

// Called to bind functionality to input
void ATopDownCameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


	if (PlayerInputComponent) {
		PlayerInputComponent->BindAxis("ZoomCamera", this, &ATopDownCameraPawn::ZoomCamera);
		PlayerInputComponent->BindAxis("MouseMove", this, &ATopDownCameraPawn::MouseMove);
		PlayerInputComponent->BindAction("MouseClick", EInputEvent::IE_Pressed, this, &ATopDownCameraPawn::MouseClick);
	}
}


FVector ATopDownCameraPawn::GetCameraPanDirection()
{
	float MousePosX;
	float MousePosY;
	float CamDirectionX = 0;
	float CamDirectionY = 0;

	PC->GetMousePosition(MousePosX, MousePosY);

	if (MousePosX < Margin) {
		CamDirectionY = -Speed;
	}

	if (MousePosX > ScreenSizeX - Margin) {
		CamDirectionY = Speed;
	}

	if (MousePosY < Margin) {
		CamDirectionX = Speed;
	}

	if (MousePosY > ScreenSizeY - Margin) {
		CamDirectionX = -Speed;
	}

	return FVector(CamDirectionX, CamDirectionY, 0);
}

void ATopDownCameraPawn::PanMoveCamera(FVector PanDirection)
{
	if (PanDirection == FVector::ZeroVector) {
		return;
	}
	else {
		AddActorWorldOffset(GetCameraPanDirection());
	}
}


FVector ATopDownCameraPawn::GetHitCoords()
{
	FHitResult hit;
	FVector hit_vector;
	float HitX, HitY;

	TArray<AActor*> NetActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANetController::StaticClass(), NetActors);
	if (NetActors.Num() <= 0) return FVector(0, 0, 0);

	PC->GetHitResultUnderCursor(ECollisionChannel::ECC_Camera, false, hit);
	hit_vector = hit.TraceEnd - hit.TraceStart;
	float Zp = NetActors[0]->GetActorLocation().Z;

	float t = (Zp - hit.TraceStart.Z) / hit_vector.Z;
	HitX = hit_vector.X * t + hit.TraceStart.X;
	HitY = hit_vector.Y * t + hit.TraceStart.Y;

	return FVector(HitX, HitY, Zp);
}

void ATopDownCameraPawn::SelectTileByMouse()
{
	TArray<AActor*> NetActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANetController::StaticClass(), NetActors);
	if (NetActors.Num() <= 0) return;
	ANetController *NetController = Cast<ANetController>(NetActors[0]);

	FVector hit_coords = GetHitCoords() - NetController->GetActorLocation();

	TileManager *tile_manager = NetController->tile_manager;
	TileData *tile = tile_manager->GetByCoordinates(hit_coords.X, hit_coords.Y);

	NetController->DrawFilledTile(tile->cell_center, tile_manager->sides);
	LastSelectedTile = tile;
}

void ATopDownCameraPawn::ZoomCamera(float AxisValue)
{

	SpringArm->TargetArmLength -= AxisValue * ZoomSpeed;
}

void ATopDownCameraPawn::MouseMove(float AxisValue)
{
	SelectTileByMouse();
}

void ATopDownCameraPawn::MouseClick()
{
	TArray<AActor*> NetActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANetController::StaticClass(), NetActors);
	if (NetActors.Num() <= 0) return;
	ANetController* NetController = Cast<ANetController>(NetActors[0]);

	NetController->SetTileBiome(LastSelectedTile, TileBiomeType::PLAIN);
}
