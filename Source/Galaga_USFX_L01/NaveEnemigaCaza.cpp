// Fill out your copyright notice in the Description page of Project Settings.


#include "NaveEnemigaCaza.h"
#include "Kismet/GameplayStatics.h"

#include "Galaga_USFX_L01Projectile.h"

ANaveEnemigaCaza::ANaveEnemigaCaza()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShipMesh(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_TriPyramid.Shape_TriPyramid'"));
	//// Create the mesh component
	//mallaNaveEnemiga = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	mallaNaveEnemiga->SetStaticMesh(ShipMesh.Object);

    
    bCanFire = true;
    FireRate = 1.0f; // Ajusta según sea necesario
    GunOffset = FVector(100.0f, 0.0f, 0.0f); // Ajusta según sea necesario
    static ConstructorHelpers::FObjectFinder<USoundBase> FireAudio(TEXT("/Game/TwinStick/Audio/TwinStickFire.TwinStickFire"));
    FireSound = FireAudio.Object;
	
}

void ANaveEnemigaCaza::Mover(float DeltaTime)
{
	// Obtiene la posición actual del actor
	FVector PosicionActual = GetActorLocation();

	// Genera nuevas coordenadas X e Y aleatorias
	float NuevaX = FMath::RandRange(-1000.0f, 1000.0f) * DeltaTime * velocidad;
	float NuevaY = FMath::RandRange(-1000.0f, 1000.0f) * DeltaTime * velocidad;

	// Crea un nuevo vector de posición con las coordenadas aleatorias y la misma Z que la posición actual
	FVector NuevaPosicion = FVector(PosicionActual.X + NuevaX, PosicionActual.Y + NuevaY, PosicionActual.Z);

	// Establece la nueva posición del actor
	SetActorLocation(NuevaPosicion);
}

void ANaveEnemigaCaza::Disparar()
{
    // Si es posible disparar
    if (bCanFire)
    {
        // Obtiene la rotación y la ubicación del propietario del componente
        FRotator FireRotation = GetActorRotation() + FRotator(0.0f, -180.0f, 0.0f);
        FVector SpawnLocation = GetActorLocation() + FireRotation.RotateVector(GunOffset);

        UWorld* const World = GetWorld();
        if (World)
        {
            // Spawn del proyectil
			for (int i = 0; i < 3; ++i)
			{
				//World->SpawnActor<AGalaga_USFX_L01Projectile>(SpawnLocation, FireRotation);
				FRotator ModifiedRotation = FireRotation;
				// Modificando la rotacion para que los disparos sean en forma de abanico
				ModifiedRotation.Yaw += (i - 1) * 20.0f; // Offset rotation by 10 degrees

				const FVector ModifiedSpawnLocation = GetActorLocation() + ModifiedRotation.RotateVector(GunOffset);

				//// Spawn the projectile
				World->SpawnActor<AGalaga_USFX_L01Projectile>(ModifiedSpawnLocation, ModifiedRotation);
			}


            // Configuración del temporizador para controlar la velocidad de disparo
            bCanFire = false;
            World->GetTimerManager().SetTimer(TimerHandle_ShotTimerExpired, this, &ANaveEnemigaCaza::ShotTimerExpired, FireRate);

            // Reproducir el sonido de disparo
            if (FireSound)
            {
                UGameplayStatics::PlaySoundAtLocation(World, FireSound, GetActorLocation());
            }
        }
    }
}

void ANaveEnemigaCaza::ShotTimerExpired()
{
	bCanFire = true;
}

void ANaveEnemigaCaza::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Mover(DeltaTime);
    Disparar();
}
