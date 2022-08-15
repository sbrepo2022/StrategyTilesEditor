// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/LineBatchComponent.h"
#include "Kismet/GameplayStatics.h"
#include "VoxelTools/VoxelBlueprintLibrary.h"
#include "VoxelWorld.h"
#include "TileData.h"
#include "NetController.generated.h"

UCLASS()
class STRATEGYTILESEDITOR_API ANetController : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	ANetController();
	~ANetController();

	void SetTileBiome(TileData *tile, TileBiomeType tile_biome_type);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void InitCells();

	int GetNumberOfTilePoints();
	TArray<FVector> GetCentersOfSquares();
	TArray<FVector> GetCentersOfHexagons();

	TArray<FVector> InitNetMesh();
	TArray<FVector> GetTileGeometry(FVector pos, int number_of_points);
	AVoxelWorld* GetVoxelWorld();

public:

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	TileManager *tile_manager;

	UPROPERTY()
	USceneComponent *Scene;

	UPROPERTY()
	ULineBatchComponent* LineBatcher;

	UPROPERTY()
	ULineBatchComponent* TileLineBatcher;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Net Settings", meta = (GetOptions = "GetNetTypeOptions"))
	FName NetType = TEXT("Squares");

	UFUNCTION(CallInEditor)
	TArray<FString> GetNetTypeOptions() const
	{
		return { TEXT("Squares"), TEXT("Hexagons") };
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Net Settings")
	float CellRadius = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Net Settings")
	float NetHeight = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Net Settings")
	float NetWidth = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Net Settings")
	float LineThickness = 100;

	UFUNCTION()
	TArray<FVector> GetCentersOfCells();

	UFUNCTION()
	void DrawCircle(FVector pos, float radius, float angle, int number_of_points);

	UFUNCTION()
	void DrawFilledTile(FVector pos, int number_of_points);
};
