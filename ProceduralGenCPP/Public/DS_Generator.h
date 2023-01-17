// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DS_Generator.generated.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

class UProceduralMeshComponent;
class UMaterialInterface;

UCLASS()
class PROCEDURALGENCPP_API ADS_Generator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADS_Generator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* Material;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Meta = (ClampMin = 1), Category = "Terrain")
	int XSize = 1;//Num squares along X axis
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 1), Category = "Terrain")
	int YSize = 1;//Num squares along Y axis
	
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0), Category = "Terrain")
	float ZMultiplier = 100.0f;
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0), Category = "Terrain")
	float NoiseScale = 1.0f;

	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0.000001), Category = "Terrain")
	float Scale = 1;
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0.000001), Category = "Terrain")
	float UVScale = 1;
	
	UPROPERTY(EditAnywhere, Category = "Trees")
	TSubclassOf<class AActor> TreesClass;
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0), Category = "Trees")
	int TreeIterations = 50;
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0), Category = "Trees")
	int MaxTrees = 10;
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 1, ClampMax = 100), Category = "Trees")
	int TreeTolerance = 50;
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0), Category = "Trees")
	float TreeTraceRadius = 0;
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0), Category = "Trees")
	float TreeMaxRotation = 10;

	UPROPERTY(EditAnywhere, Category = "Flowers")
	TSubclassOf<class AActor> FlowersClass;
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0), Category = "Flowers")
	int FlowerIterations = 50;
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0), Category = "Flowers")
	int MaxFlowers = 10;
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 1, ClampMax = 100), Category = "Flowers")
	int FlowerTolerance = 50;
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0), Category = "Flowers")
	float FlowerTraceRadius = 0;
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0), Category = "Flowers")
	float FlowerMaxRotation = 20;

	UPROPERTY(EditAnywhere, Category = "Rocks")
	TSubclassOf<class AActor> RocksClass;
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0), Category = "Rocks")
	int RockIterations = 50;
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0), Category = "Rocks")
	int MaxRocks = 10;
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 1, ClampMax = 100), Category = "Rocks")
	int RockTolerance = 50;
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0), Category = "Rocks")
	float RockTraceRadius = 0;
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0), Category = "Rocks")
	float RockMaxRotation = 45;

	UPROPERTY(EditAnywhere, Category = "Grass")
	TSubclassOf<class AActor> GrassClass;
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0), Category = "Grass")
	int GrassIterations = 50;
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0), Category = "Grass")
	int MaxGrass = 10;
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 1, ClampMax = 100), Category = "Grass")
	int GrassTolerance = 50;
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0), Category = "Grass")
	float GrassTraceRadius = 0;
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0), Category = "Grass")
	float GrassMaxRotation = 20;

	UPROPERTY(EditAnywhere, Category = "Fauna")
	TArray<TSubclassOf<class AActor>> GrassClasses;
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0), Category = "Fauna")
	int FaunaIterations = 50;
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0), Category = "Fauna")
	int MaxFauna = 10;
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 1, ClampMax = 100), Category = "Fauna")
	int FaunaTolerance = 50;
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0), Category = "Fauna")
	float FaunaTraceRadius;
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0), Category = "Fauna")
	float FaunaMaxRotation;

private:
	UProceduralMeshComponent* ProceduralMesh;
	
	TArray<FVector> Vertices;
	TArray<int> Triangles;
	TArray<AActor*> SpawnedTrees;
	TArray<AActor*> SpawnedFlowers;
	TArray<AActor*> SpawnedRocks;
	TArray<AActor*> SpawnedGrass;
	TArray<AActor*> SpawnedFauna;
	TArray<FVector2D> UV0;

	void SpawnTerrain();
	void CreateVertices();
	void CreateTriangles();
	FHitResult SpawnTrace(FVector Location);
	FHitResult SpawnSweep(FVector Location, float Radius);
	void SpawnProps(int Iterations, int MaxSpawned, int Tolerance, float TraceRadius, TSubclassOf<class AActor> PropToSpawn, TArray<AActor*> SpawnedProps, float MaxRot);
	void SpawnFlora(TSubclassOf<class AActor> Prop, float TraceRadius, int Iterations, int MaxSpawned, int Tolerance, TArray<AActor*> SpawnedProps, float MaxRotation);
	void SpawnFauna(TArray<TSubclassOf<class AActor>> Fauna, float TraceRadius, int Iterations, int MaxSpawned, int Tolerance, float MaxRotation);
};
