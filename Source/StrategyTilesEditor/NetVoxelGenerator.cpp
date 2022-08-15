#include "NetVoxelGenerator.h"
#include "FastNoise/VoxelFastNoise.inl"
#include "VoxelMaterialBuilder.h"

TVoxelSharedRef<FVoxelGeneratorInstance> UVoxelGeneratorExample::GetInstance()
{
	return MakeVoxelShared<FVoxelGeneratorExampleInstance>(*this);
}

///////////////////////////////////////////////////////////////////////////////

FVoxelGeneratorExampleInstance::FVoxelGeneratorExampleInstance(const UVoxelGeneratorExample& MyGenerator)
	: Super(&MyGenerator)
	, XLength(MyGenerator.XLength)
	, YLength(MyGenerator.YLength)
	, ZLength(MyGenerator.ZLength)
	, IslandsHeight(MyGenerator.IslandsHeight)
	, Fallof(MyGenerator.Fallof)
	, IslandBeachNoiseDense(MyGenerator.IslandBeachNoiseDense)
	, IslandBeachAltNoiseDense(MyGenerator.IslandBeachAltNoiseDense)
	, NoiseFallof(MyGenerator.NoiseFallof)
{
}

void FVoxelGeneratorExampleInstance::Init(const FVoxelGeneratorInit& InitStruct)
{
	VoxelSize = InitStruct.VoxelSize;

	TArray<AActor*> NetActors;
	if (InitStruct.World != nullptr) {
		UGameplayStatics::GetAllActorsOfClass(InitStruct.World->GetWorld(), ANetController::StaticClass(), NetActors);
		if (NetActors.Num() > 0)
			NetController = Cast<ANetController>(NetActors[0]);
		else
			NetController = nullptr;
	}

	Noise.SetSeed(1337);
}

v_flt FVoxelGeneratorExampleInstance::GetValueImpl(v_flt X, v_flt Y, v_flt Z, int32 LOD, const FVoxelItemStack& Items) const
{
	float WorldX = X * VoxelSize;
	float WorldY = Y * VoxelSize;
	float WorldZ = Z * VoxelSize;

	// Positive value -> empty voxel
	// Negative value -> full voxel
	// Value positive when Z > Height, and negative Z < Height
	if (WorldX < -XLength || WorldY < -YLength || WorldZ < -ZLength || WorldX > XLength || WorldY > YLength || WorldZ > ZLength) return 1;
	if (WorldZ < 0) return -1;

	const float NoiseHeight = (Noise.GetPerlin_2D(X, Y, IslandBeachNoiseDense) + 1.0f) * 0.5f;
	const float AlternativeNoiseHeight = (UKismetMathLibrary::Sin(X * IslandBeachAltNoiseDense) + UKismetMathLibrary::Sin(Y * IslandBeachAltNoiseDense)) * 0.5;
	const float FullNoiseHeight = NoiseHeight * 0.7 + AlternativeNoiseHeight * 0.3;
	const float RealFallof = (NoiseFallof - Fallof) * FullNoiseHeight + Fallof;

	float Height = 0;
	TileData* tile_data;
	TArray<AdjacencyData> adjacency_data;
	FVector net_pos;
	FVector RelativePos;

	if (NetController != nullptr) {
		net_pos = NetController->GetActorLocation();
		RelativePos = FVector(WorldX - net_pos.X, WorldY - net_pos.Y, 0);
		if (NetController->tile_manager != nullptr) {
			tile_data = NetController->tile_manager->GetByCoordinates(RelativePos.X, RelativePos.Y);
			if (tile_data != nullptr) {
				if (tile_data->tile_biome_type != TileBiomeType::OCEAN) {
					Height = IslandsHeight / VoxelSize;
				}
				else if (tile_data->tile_biome_type == TileBiomeType::OCEAN) {
					adjacency_data = tile_data->adjacency_data;
					float CurHeight;
					float Dist;
					FVector P1, P2;
					for (auto adj_data : adjacency_data) {
						if (adj_data.tile->tile_biome_type != TileBiomeType::OCEAN) {
							switch (adj_data.adjacency_line.Num()) {
								case 1:
									CurHeight = IslandsHeight / VoxelSize * UKismetMathLibrary::FMax((RealFallof * RealFallof - (RelativePos - adj_data.adjacency_line[0]).SizeSquared()) / (RealFallof * RealFallof), 0);
									//UE_LOG(LogTemp, Warning, TEXT("x: %d, y: %d, lx: %lf, ly: %lf, rel_x: %lf, rel_y: % lf"), tile_data->x, tile_data->y, adj_data.adjacency_line[0].X, adj_data.adjacency_line[0].Y, RelativePos.X, RelativePos.Y);
									break;

								case 2:
									P1 = adj_data.adjacency_line[0];
									P2 = adj_data.adjacency_line[1];
									Dist = (P2.Y - P1.Y) * RelativePos.X - (P2.X - P1.X) * RelativePos.Y + P2.X * P1.Y - P2.Y * P1.X;
									Dist *= Dist;
									Dist /= (P2 - P1).SizeSquared();
									CurHeight = IslandsHeight / VoxelSize * UKismetMathLibrary::FMax((RealFallof * RealFallof - Dist) / (RealFallof * RealFallof), 0);
									break;
							}
							CurHeight = CurHeight > IslandsHeight / VoxelSize ? IslandsHeight / VoxelSize : CurHeight;
							Height = CurHeight > Height ? CurHeight : Height;
						}
					}
				}
			}
		}
	}

	float Value = Z - Height;

	// The voxel value is clamped between -1 and 1. That can result in a bad gradient/normal. To solve that we divide it
	Value /= 5;

	return Value;
}

FVoxelMaterial FVoxelGeneratorExampleInstance::GetMaterialImpl(v_flt X, v_flt Y, v_flt Z, int32 LOD, const FVoxelItemStack& Items) const
{
	FVoxelMaterialBuilder Builder;

	// RGB
	Builder.SetMaterialConfig(EVoxelMaterialConfig::RGB);
	Builder.SetColor(FColor::Red);

	// Single index
	//Builder.SetMaterialConfig(EVoxelMaterialConfig::SingleIndex);
	//Builder.SetSingleIndex(0); 

	// Multi index
	//Builder.SetMaterialConfig(EVoxelMaterialConfig::MultiIndex);
	//Builder.AddMultiIndex(0, 0.5f);
	//Builder.AddMultiIndex(1, 0.5f);

	return Builder.Build();
}

TVoxelRange<v_flt> FVoxelGeneratorExampleInstance::GetValueRangeImpl(const FVoxelIntBox& Bounds, int32 LOD, const FVoxelItemStack& Items) const
{
	// Return the values that GetValueImpl can return in Bounds
	// Used to skip chunks where the value does not change
	// Be careful, if wrong your world will have holes!
	// By default return infinite range to be safe
	return TVoxelRange<v_flt>::Infinite();
}

FVector FVoxelGeneratorExampleInstance::GetUpVector(v_flt X, v_flt Y, v_flt Z) const
{
	// Used by spawners
	return FVector::UpVector;
}