// Fill out your copyright notice in the Description page of Project Settings.


#include "DS_Generator.h"
#include "ProceduralMeshComponent.h"


#define COLLISION_SPAWNING ECC_GameTraceChannel1
#define COLLISION_PROPS ECC_GameTraceChannel2

// Sets default values
ADS_Generator::ADS_Generator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>("ProceduralMesh");
	ProceduralMesh->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void ADS_Generator::BeginPlay()
{
	Super::BeginPlay();

	SpawnTerrain();
}

// Called every frame
void ADS_Generator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADS_Generator::SpawnTerrain()
{
	ProceduralMesh->ClearAllMeshSections();

	CreateVertices();
	CreateTriangles();

	//Setup procedural mesh for terrain using created vertices & triangles
	ProceduralMesh->CreateMeshSection(0, Vertices, Triangles, TArray<FVector>(), UV0, TArray<FColor>(), TArray<FProcMeshTangent>(), true);
	ProceduralMesh->SetMaterial(0, Material);
	ProceduralMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ProceduralMesh->SetCollisionProfileName("AllowSpawning");
	ProceduralMesh->SetCollisionResponseToChannel(COLLISION_SPAWNING, ECR_Block);

	//Spawn all the cool flora
	SpawnFlora(TreesClass, TreeTraceRadius, TreeIterations, MaxTrees, TreeTolerance, SpawnedTrees, TreeMaxRotation);
	SpawnFlora(FlowersClass, FlowerTraceRadius, FlowerIterations, MaxFlowers, FlowerTolerance, SpawnedFlowers, FlowerMaxRotation);
	SpawnFlora(RocksClass, RockTraceRadius, RockIterations, MaxRocks, RockTolerance, SpawnedRocks, RockMaxRotation);
	SpawnFlora(GrassClass, GrassTraceRadius, GrassIterations, MaxGrass, GrassTolerance, SpawnedGrass, GrassMaxRotation);

	ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	PlayerChar->SetActorLocation(Vertices[(Vertices.Num() / 2)]);
}

void ADS_Generator::CreateVertices()
{
	Vertices.Empty();

	//Two for loops to create a grid of vertices
	for (int X = 0; X <= XSize; ++X)
	{
		for (int Y = 0; Y <= YSize; ++Y)
		{
			//Use perlin noise to randomize height of vertices, so terrain isn't flat
			float Z = FMath::PerlinNoise2D(FVector2D(X * NoiseScale + 0.1, Y * NoiseScale + 0.1)) * ZMultiplier;
			Vertices.Add(FVector(X * Scale, Y * Scale, Z));
			UV0.Add(FVector2D(X * UVScale, Y * UVScale));
		}
	}
}

void ADS_Generator::CreateTriangles()
{
	Triangles.Empty();

	int Vertex = 0;
	for (int X = 0; X < XSize; ++X)
	{
		for (int Y = 0; Y < YSize; ++Y)
		{
			Triangles.Add(Vertex);//Bottom Left
			Triangles.Add(Vertex + 1);//Bottom Right
			Triangles.Add(Vertex + YSize + 1);//Top Left
			Triangles.Add(Vertex + 1);//Bottom Right
			Triangles.Add(Vertex + YSize + 2);//Top Right
			Triangles.Add(Vertex + YSize + 1);//Top Left

			++Vertex;
		}
		++Vertex;
	}
}

FHitResult ADS_Generator::SpawnTrace(FVector Location)
{
	FHitResult Hit; // FHitResult will hold all data returned by our line collision query

	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;


	//Line trace is from 500 units above Location to 500 units below
	FVector TraceStart = Location + FVector(0, 0, 1500);
	FVector TraceEnd = Location - FVector(0, 0, 1500);

	GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECollisionChannel(COLLISION_SPAWNING), QueryParams);

	//DrawDebugLine(GetWorld(), TraceStart, TraceEnd, Hit.bBlockingHit ? FColor::Blue : FColor::Red, false, 15.0f, 0, 10.0f);

	return Hit;
}

FHitResult ADS_Generator::SpawnSweep(FVector Location, float Radius)
{
	FHitResult Hit; // FHitResult will hold all data returned by our line collision query

	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;


	//Line trace is from 500 units above Location to 500 units below
	FVector TraceStart = Location + FVector(0, 0, 1500);
	FVector TraceEnd = Location - FVector(0, 0, 1500);

	GetWorld()->SweepSingleByChannel(Hit, TraceStart, TraceEnd, FQuat(0.0), ECollisionChannel(COLLISION_PROPS), FCollisionShape::MakeSphere(Radius), QueryParams);

	return Hit;
}

void ADS_Generator::SpawnProps(int Iterations, int MaxSpawned, int Tolerance, float TraceRadius, TSubclassOf<class AActor> PropToSpawn, TArray<AActor*> SpawnedProps, float MaxRot)
{
	FHitResult CurrentHit;
	FHitResult PropHit;
	FActorSpawnParameters SpawnInfo;

	if (PropToSpawn)
	{
		for (int F = 0; F < Iterations; ++F)
		{
			if (SpawnedProps.Num() <= MaxSpawned)
			{
				//Get a random vertex from the generated terrain
				int CurrentVertex = FMath::RandRange(0, Vertices.Num() - 1);

				//First hit test using a line to check if object will actually spawn on terrain
				CurrentHit = SpawnTrace(Vertices[CurrentVertex]);
				if (CurrentHit.bBlockingHit == true)
				{
					//Second hit test using a sphere to check for surrounding props that may block the new one
					PropHit = SpawnSweep(Vertices[CurrentVertex], TraceRadius);
					if (PropHit.bBlockingHit == false)
					{
						UE_LOG(LogTemp, Warning, TEXT("HIT!"));
						if (FMath::RandRange(1, 100) >= Tolerance)
						{
							//Setup for aligning prop to terrain
							FVector under;
							FVector newUp;
							FVector newFwd;
							FVector newRgt;

							under = CurrentHit.ImpactPoint;
							newUp = CurrentHit.ImpactNormal;

							//Some vector magic to align prop
							FVector currentRgt = GetActorRightVector();
							newFwd = FVector::CrossProduct(currentRgt, newUp);
							newRgt = FVector::CrossProduct(newUp, newFwd);


							//Final rotation to align prop
							FTransform newSpawnLoc(newFwd.GetClampedToMaxSize(MaxRot), newRgt.GetClampedToMaxSize(MaxRot), newUp.GetClampedToMaxSize(MaxRot), under);
							

							AActor* CurrentSpawned = GetWorld()->SpawnActor<AActor>(PropToSpawn, newSpawnLoc, SpawnInfo);
							
							//Some more vector magic to randomize the yaw of the spawned prop, so they aren't all facing the same way
							FQuat RRot = FVector(0.0, FMath::RandRange(-180, 180), 0.0).Rotation().Quaternion();
							CurrentSpawned->SetActorRelativeRotation(RRot);

							//Add prop to corresponding array for later reference
							if (IsValid(CurrentSpawned))
							{
								SpawnedProps.Add(CurrentSpawned);
								UE_LOG(LogTemp, Warning, TEXT("Spawned Prop!"));
							}
						}
					}
				}
				//Error checking in case the test finds an unsuitable location
				else if (CurrentHit.bBlockingHit == false)
				{
					UE_LOG(LogTemp, Warning, TEXT("NO HIT"));
				}

			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("FLORA/FAUNA ARRAY IS EMPTY OR RADII ARRAYS DON'T MATCH!"));
	}
}

void ADS_Generator::SpawnFlora(TSubclassOf<class AActor> Prop, float TraceRadius, int Iterations, int MaxSpawned, int Tolerance, TArray<AActor*> SpawnedProps, float MaxRotation)
{
	SpawnProps(Iterations, MaxSpawned, Tolerance, TraceRadius, Prop, SpawnedProps, MaxRotation);
}

void ADS_Generator::SpawnFauna(TArray<TSubclassOf<class AActor>> Fauna, float TraceRadius, int Iterations, int MaxSpawned, int Tolerance, float MaxRotation)
{
	for (int F = 0; F <= Fauna.Num(); ++F)
	{
		SpawnProps(Iterations, MaxSpawned, Tolerance, TraceRadius, Fauna[F], SpawnedFauna, MaxRotation);
	}
}