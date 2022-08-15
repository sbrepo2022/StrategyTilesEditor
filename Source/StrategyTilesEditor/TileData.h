// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WaterBodyActor.h"
#include "WaterBodyIslandActor.h"

/**
 * 
 */

enum TileBiomeType
{
	OCEAN,
	PLAIN,
};

class AdjacencyData;
class TileManager;

class TileData
{
public:
	TileManager* tile_manager;

	TileBiomeType tile_biome_type = TileBiomeType::OCEAN;
	FVector cell_center;
	int x, y;

	TArray<FVector> Verts;
	int sides;
	float radius;

	TArray<AdjacencyData> adjacency_data;

public:
	TileData() {};
	TileData(TileManager *tile_manager, FVector cell_center, int x, int y);
	~TileData();

	void InitTileGeometry(float radius, int number_of_points);
};

class AdjacencyData {
public:
	TileData* tile;
	TArray<FVector> adjacency_line;

public:
	AdjacencyData() {};
	AdjacencyData(TileData* tile, TArray<FVector> adjacency_line) : tile(tile), adjacency_line(adjacency_line) {};
};

class TileManager {
public:
	TArray<TileData*> TileDataArray;
	int sides;
	float radius;
	int width, height;

public:
	TileManager(TArray<FVector> cells_centers, int sides, float radius, int width, int height);
	~TileManager();

	TileData* operator[] (int index);
	TileData* Get(int x, int y);
	TileData* GetByCoordinates(float PosX, float PosY);
	void RebuiltAdjacencyData(int x, int y);
	void RebuiltAdjacencyDataAround(int x, int y);
};
