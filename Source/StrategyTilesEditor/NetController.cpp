// Fill out your copyright notice in the Description page of Project Settings.


#include "NetController.h"

// Sets default values
ANetController::ANetController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("ComponentScene"));
	RootComponent = Scene;
	
	LineBatcher = CreateDefaultSubobject<ULineBatchComponent>(TEXT("LineBatcher"));
	LineBatcher->SetupAttachment(Scene);

	TileLineBatcher = CreateDefaultSubobject<ULineBatchComponent>(TEXT("TileLineBatcher"));
	TileLineBatcher->SetupAttachment(Scene);

	tile_manager = nullptr;
}

ANetController::~ANetController()
{
	if (tile_manager != nullptr)
	{
		delete tile_manager;
	}
}

void ANetController::SetTileBiome(TileData* tile, TileBiomeType tile_biome_type)
{
	if (tile == nullptr) return;

	tile->tile_biome_type = tile_biome_type;

	if (tile_biome_type == TileBiomeType::OCEAN) {

	}
	else {
		switch (tile_biome_type) {
			case TileBiomeType::PLAIN:
				
				break;
		}
	}

	AVoxelWorld* VoxelWorld = GetVoxelWorld();
	UVoxelBlueprintLibrary::Recreate(VoxelWorld, true);
}

// Called when the game starts or when spawned
void ANetController::BeginPlay()
{
	Super::BeginPlay();

	InitCells();
}

void ANetController::InitCells()
{
	TArray<FVector> cells_centers = InitNetMesh();
	int sides = GetNumberOfTilePoints();

	tile_manager = new TileManager(cells_centers, sides, CellRadius, NetWidth, NetHeight);
}

// Called every frame
void ANetController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

TArray<FVector> ANetController::GetCentersOfCells()
{
	if (NetType == "Squares") {
		return GetCentersOfSquares();
	}
	else if (NetType == "Hexagons") {
		return GetCentersOfHexagons();
	}

	return TArray<FVector>();
}

int ANetController::GetNumberOfTilePoints()
{
	int sides = 4;

	if (NetType == "Squares") {
		sides = 4;
	}
	else if (NetType == "Hexagons") {
		sides = 6;
	}

	return sides;
}

TArray<FVector> ANetController::GetCentersOfSquares() {
	TArray<FVector> result;
	float CurrX, CurrY;
	int x, y;

	y = 0;
	CurrY = CellRadius;
	while (y < NetHeight) {
		x = 0;
		CurrX = CellRadius;
		while (x < NetWidth) {
			result.Add(FVector(CurrX, CurrY, 0));

			CurrX += 2 * CellRadius * 0.70710678;
			x++;
		}
		CurrY += 2 * CellRadius * 0.70710678;
		y++;
	}

	return result;
}

TArray<FVector> ANetController::GetCentersOfHexagons() {
	TArray<FVector> result;
	float CurrX, CurrY;
	int x, y;

	y = 0;
	CurrY = CellRadius;
	while (y < NetHeight) {
		x = 0;
		CurrX = CellRadius * 0.86602 * (y % 2 == 0 ? 1 : 2);
		while (x < NetWidth) {
			result.Add(FVector(CurrX, CurrY, 0));

			CurrX += 2 * CellRadius * 0.86602;
			x++;
		}
		CurrY += 1.5 * CellRadius;
		y++;
	}

	return result;
}

TArray<FVector> ANetController::InitNetMesh()
{
	TArray<FVector> cells = GetCentersOfCells();
	for (auto cell_pos : cells) {
		if (NetType == "Squares") {
			DrawCircle(cell_pos, CellRadius, PI / 4, 4);
		}
		else if (NetType == "Hexagons") {
			DrawCircle(cell_pos, CellRadius, PI / 6, 6);
		}
	}

	return cells;
}

TArray<FVector> ANetController::GetTileGeometry(FVector pos, int number_of_points)
{
	float radius = CellRadius;
	float angle = PI / number_of_points;

	FVector actor_pos = GetActorLocation();

	TArray< FVector > Verts;
	for (int i = 0; i < number_of_points; i++) {
		Verts.Add(
			FVector(actor_pos.X + pos.X + cos(2 * PI * (double)i / number_of_points + angle) * radius, actor_pos.Y + pos.Y + sin(2 * PI * (double)i / number_of_points + angle) * radius, actor_pos.Z + 1)
		);
	}

	return Verts;
}

AVoxelWorld* ANetController::GetVoxelWorld()
{
	TArray<AActor*> VoxelWorlds;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AVoxelWorld::StaticClass(), VoxelWorlds);
	if (VoxelWorlds.Num() <= 0) return nullptr;
	AVoxelWorld* VoxelWorld = Cast<AVoxelWorld>(VoxelWorlds[0]);

	return VoxelWorld;
}

void ANetController::DrawFilledTile(FVector pos, int number_of_points)
{
	TileLineBatcher->Flush();

	float radius = CellRadius;
	float angle = PI / number_of_points;

	FVector actor_pos = GetActorLocation();

	TArray< FVector > Verts;
	TArray< int32 > Indices;

	Verts.Add(actor_pos + pos);
	Verts += GetTileGeometry(pos, number_of_points);

	for (int i = 1; i < number_of_points; i++) {
		Indices.Add(0);
		Indices.Add(i);
		Indices.Add(i + 1);
	}
	Indices.Add(0);
	Indices.Add(number_of_points);
	Indices.Add(1);

	TileLineBatcher->DrawMesh(Verts, Indices, FColor(0, 255, 0, 50), 2, 1000000);
}

void ANetController::DrawCircle(FVector pos, float radius, float angle, int number_of_points)
{
	FVector actor_pos = GetActorLocation();

	TArray<FBatchedLine> lines;

	for (int i = 0; i < number_of_points; i++) {
		LineBatcher->DrawLine(
			FVector(actor_pos.X + pos.X + cos(2 * PI * (double)i / number_of_points + angle) * radius, actor_pos.Y + pos.Y + sin(2 * PI * (double)i / number_of_points + angle) * radius, actor_pos.Z),
			FVector(actor_pos.X + pos.X + cos(2 * PI * (double)(i + 1) / number_of_points + angle) * radius, actor_pos.Y + pos.Y + sin(2 * PI * (double)(i + 1) / number_of_points + angle) * radius, actor_pos.Z),
			FLinearColor(1.0, 0.0, 0.0),
			1, LineThickness, 1000000
		);
	}
}
