// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NetController.h"
#include "TileData.h"
#include "TopDownCameraPawn.generated.h"

UCLASS()
class STRATEGYTILESEDITOR_API ATopDownCameraPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATopDownCameraPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	TileData* LastSelectedTile;

	UPROPERTY()
	USceneComponent* Scene;
	
	UPROPERTY()
	USpringArmComponent* SpringArm;

	UPROPERTY()
	UCameraComponent* Camera;

	UPROPERTY()
	APlayerController* PC;

	UFUNCTION()
	FVector GetCameraPanDirection();

	UFUNCTION()
	void PanMoveCamera(FVector PanDirection);

	UFUNCTION()
	FVector GetHitCoords();

	UFUNCTION()
	void SelectTileByMouse();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float Margin = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float Speed = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float ZoomSpeed = 300;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float StartZoom = 5000;

	UPROPERTY()
	int ScreenSizeX;

	UPROPERTY()
	int ScreenSizeY;

	UFUNCTION()
	void ZoomCamera(float AxisValue);

	UFUNCTION()
	void MouseMove(float AxisValue);

	UFUNCTION()
	void MouseClick();
};
