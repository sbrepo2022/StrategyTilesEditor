// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FastNoise/VoxelFastNoise.h"
#include "VoxelGenerators/VoxelGeneratorHelpers.h"
#include "NetController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "NetVoxelGenerator.generated.h"

UCLASS(Blueprintable)
class UVoxelGeneratorExample : public UVoxelGenerator
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Field")
		float XLength = 30000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Field")
		float YLength = 30000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Field")
		float ZLength = 5000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IslandsBase")
		float IslandsHeight = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IslandsBase")
		float Fallof = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
		float IslandBeachNoiseDense = 0.05f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
		float IslandBeachAltNoiseDense = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
		float NoiseFallof = 1200.0f;

	//~ Begin UVoxelGenerator Interface
	virtual TVoxelSharedRef<FVoxelGeneratorInstance> GetInstance() override;
	//~ End UVoxelGenerator Interface
};

class FVoxelGeneratorExampleInstance : public TVoxelGeneratorInstanceHelper<FVoxelGeneratorExampleInstance, UVoxelGeneratorExample>
{
public:
	using Super = TVoxelGeneratorInstanceHelper<FVoxelGeneratorExampleInstance, UVoxelGeneratorExample>;

	explicit FVoxelGeneratorExampleInstance(const UVoxelGeneratorExample& MyGenerator);

	//~ Begin FVoxelGeneratorInstance Interface
	virtual void Init(const FVoxelGeneratorInit& InitStruct) override;

	v_flt GetValueImpl(v_flt X, v_flt Y, v_flt Z, int32 LOD, const FVoxelItemStack& Items) const;
	FVoxelMaterial GetMaterialImpl(v_flt X, v_flt Y, v_flt Z, int32 LOD, const FVoxelItemStack& Items) const;

	TVoxelRange<v_flt> GetValueRangeImpl(const FVoxelIntBox& Bounds, int32 LOD, const FVoxelItemStack& Items) const;

	virtual FVector GetUpVector(v_flt X, v_flt Y, v_flt Z) const override final;
	//~ End FVoxelGeneratorInstance Interface

private:
	const float XLength;
	const float YLength;
	const float ZLength;

	const float IslandsHeight;
	const float Fallof;

	const float IslandBeachNoiseDense;
	const float IslandBeachAltNoiseDense;
	const float NoiseFallof;

	float VoxelSize;
	ANetController* NetController;

	FVoxelFastNoise Noise;
};
