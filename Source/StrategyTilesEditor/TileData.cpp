// Fill out your copyright notice in the Description page of Project Settings.


#include "TileData.h"

TileData::TileData(TileManager* tile_manager, FVector cell_center, int x, int y) : tile_manager(tile_manager), cell_center(cell_center), x(x), y(y)
{
}

TileData::~TileData()
{
}

void TileData::InitTileGeometry(float Radius, int Sides)
{
	radius = Radius;
	sides = Sides;
	float angle = PI / sides;

	Verts.Empty();
	for (int i = 0; i < sides; i++) {
		Verts.Add(
			FVector(cell_center.X + cos(2 * PI * (double)i / sides + angle) * radius, cell_center.Y + sin(2 * PI * (double)i / sides + angle) * radius, 0)
		);
	}
}

TileManager::TileManager(TArray<FVector> cells_centers, int sides, float radius, int width, int height) : sides(sides), radius(radius), width(width), height(height)
{
	int i = 0;
	for (auto cell_center : cells_centers) {
		TileDataArray.Add(new TileData(this, cell_center, i % width, (int)(i / width)));
		TileDataArray[i]->InitTileGeometry(radius, sides);
		i++;
	}

	for (int j = 0; j < cells_centers.Num(); j++) {
		RebuiltAdjacencyData(TileDataArray[j]->x, TileDataArray[j]->y);
	}
}

TileManager::~TileManager() {
	for (auto tile : TileDataArray) {
		if (tile != nullptr) delete tile;
	}
}

TileData* TileManager::operator[](int index)
{
	return TileDataArray[index];
}

TileData* TileManager::Get(int x, int y)
{
	if (x >= 0 && x < width && y >= 0 && y < height)
		return TileDataArray[x + width * y];

	return nullptr;
}

TileData* TileManager::GetByCoordinates(float PosX, float PosY)
{
	float StepX, StepY, FullWidth, FullHeight, MinLength, CurLength;
	int InterpIndexX, InterpIndexY;
	FVector p1(PosX, PosY, 0), p2;
	TileData *NearestTile, *CurTile;


	if (sides == 4) {
		StepX = StepY = 2 * radius * 0.70710678;
	}
	else if (sides == 6) {
		StepX = 2 * radius * 0.86602;
		StepY = 1.5 * radius;
	}
	else {
		return nullptr;
	}

	FullWidth = StepX * (width + 1);
	FullHeight = StepY * (height + 1);

	InterpIndexX = (int)(width * PosX / FullWidth);
	InterpIndexY = (int)(height * PosY / FullHeight);
	if (InterpIndexX >= width)
		InterpIndexX = width - 1;
	else if (InterpIndexX < 0) {
		InterpIndexX = 0;
	}
	if (InterpIndexY >= height)
		InterpIndexY = height - 1;
	else if (InterpIndexY < 0) {
		InterpIndexY = 0;
	}

	NearestTile = this->Get(InterpIndexX, InterpIndexY);

	p2 = NearestTile->cell_center;
	p2.Z = 0;
	MinLength = (p1 - p2).SizeSquared();

	for (int j = (InterpIndexY - 1 < 0 ? 0 : InterpIndexY - 1); j <= (InterpIndexY + 1 >= height ? height - 1 : InterpIndexY + 1); j++) {
		for (int i = (InterpIndexX - 1 < 0 ? 0 : InterpIndexX - 1); i <= (InterpIndexX + 1 >= width ? width - 1 : InterpIndexX + 1); i++) {
			CurTile = this->Get(i, j);
			p2 = CurTile->cell_center;
			p2.Z = 0;
			CurLength = (p1 - p2).SizeSquared();
			if (CurLength < MinLength) {
				MinLength = CurLength;
				NearestTile = CurTile;
			}
		}
	}

	return NearestTile;
}

void TileManager::RebuiltAdjacencyData(int x, int y)
{
	TileData* cur_tile = Get(x, y);
	TArray<AdjacencyData> adjacency_data;
	TileData* tile;

	if (cur_tile != nullptr) {
		switch (sides) {
		case 4:
			tile = Get(x + 1, y);
			if (tile != nullptr) adjacency_data.Add(AdjacencyData(tile, { tile->Verts[1], tile->Verts[2] }));

			tile = Get(x - 1, y);
			if (tile != nullptr) adjacency_data.Add(AdjacencyData(tile, { tile->Verts[3], tile->Verts[0] }));

			tile = Get(x, y - 1);
			if (tile != nullptr) adjacency_data.Add(AdjacencyData(tile, { tile->Verts[0], tile->Verts[1] }));

			tile = Get(x, y + 1);
			if (tile != nullptr) adjacency_data.Add(AdjacencyData(tile, { tile->Verts[2], tile->Verts[3] }));

			tile = Get(x + 1, y + 1);
			if (tile != nullptr) adjacency_data.Add(AdjacencyData(tile, { tile->Verts[2] }));

			tile = Get(x - 1, y + 1);
			if (tile != nullptr) adjacency_data.Add(AdjacencyData(tile, { tile->Verts[3] }));

			tile = Get(x - 1, y - 1);
			if (tile != nullptr) adjacency_data.Add(AdjacencyData(tile, { tile->Verts[0] }));

			tile = Get(x + 1, y - 1);
			if (tile != nullptr) adjacency_data.Add(AdjacencyData(tile, { tile->Verts[1] }));
			break;

		case 6:
			tile = Get(x + 1, y);
			if (tile != nullptr) adjacency_data.Add(AdjacencyData(tile, { tile->Verts[2], tile->Verts[3] }));

			tile = Get(x - 1, y);
			if (tile != nullptr) adjacency_data.Add(AdjacencyData(tile, { tile->Verts[5], tile->Verts[0] }));

			if (y % 2 == 0) {
				tile = Get(x - 1, y - 1);
				if (tile != nullptr) adjacency_data.Add(AdjacencyData(tile, { tile->Verts[0], tile->Verts[1] }));

				tile = Get(x, y - 1);
				if (tile != nullptr) adjacency_data.Add(AdjacencyData(tile, { tile->Verts[1], tile->Verts[2] }));

				tile = Get(x - 1, y + 1);
				if (tile != nullptr) adjacency_data.Add(AdjacencyData(tile, { tile->Verts[4], tile->Verts[5] }));

				tile = Get(x, y + 1);
				if (tile != nullptr) adjacency_data.Add(AdjacencyData(tile, { tile->Verts[3], tile->Verts[4] }));
			}
			else {
				tile = Get(x, y - 1);
				if (tile != nullptr) adjacency_data.Add(AdjacencyData(tile, { tile->Verts[0], tile->Verts[1] }));

				tile = Get(x + 1, y - 1);
				if (tile != nullptr) adjacency_data.Add(AdjacencyData(tile, { tile->Verts[1], tile->Verts[2] }));

				tile = Get(x, y + 1);
				if (tile != nullptr) adjacency_data.Add(AdjacencyData(tile, { tile->Verts[4], tile->Verts[5] }));

				tile = Get(x + 1, y + 1);
				if (tile != nullptr) adjacency_data.Add(AdjacencyData(tile, { tile->Verts[3], tile->Verts[4] }));
			}
			break;

		default:
			cur_tile->adjacency_data = TArray<AdjacencyData>();
			return;
		}

		cur_tile->adjacency_data = adjacency_data;
	}
}

void TileManager::RebuiltAdjacencyDataAround(int x, int y)
{
	TileData* cur_tile;
	for (int i = x - 1; i <= x + 1; i++) {
		for (int j = y - 1; j <= y + 1; j++) {
			cur_tile = Get(i, j);
			if (cur_tile != nullptr) {
				RebuiltAdjacencyData(i, j);
			}
		}
	}
}
