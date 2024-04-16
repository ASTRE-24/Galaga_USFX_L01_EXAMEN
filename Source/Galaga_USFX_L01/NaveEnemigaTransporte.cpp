// Fill out your copyright notice in the Description page of Project Settings.


#include "NaveEnemigaTransporte.h"
#include "Kismet/GameplayStatics.h"
#include "Galaga_USFX_L01Projectile.h"

ANaveEnemigaTransporte::ANaveEnemigaTransporte()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShipMesh(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_NarrowCapsule.Shape_NarrowCapsule'"));
	mallaNaveEnemiga->SetStaticMesh(ShipMesh.Object);

	bCanFire = true;
	FireRate = 2.0f; // Ajusta según sea necesario
	GunOffset = FVector(100.0f, 0.0f, 0.0f); // Ajusta según sea necesario
	static ConstructorHelpers::FObjectFinder<USoundBase> FireAudio(TEXT("/Game/TwinStick/Audio/TwinStickFire.TwinStickFire"));
	FireSound = FireAudio.Object;

}

void ANaveEnemigaTransporte::Mover(float DeltaTime)
{
	// Obtiene la posici�n actual del actor
	FVector PosicionActual = GetActorLocation();

	// Genera nuevas coordenadas X e Y aleatorias
	float NuevaX = FMath::RandRange(-1000.0f, 1000.0f) * (DeltaTime / 1000.0f);
	float NuevaY = FMath::RandRange(-1000.0f, 1000.0f) * (DeltaTime / 1000.0f);
	float NuevaZ = FMath::RandRange(-1000.0f, 1000.0f) * DeltaTime;

	// Crea un nuevo vector de posici�n con las coordenadas aleatorias y la misma Z que la posici�n actual
	FVector NuevaPosicion = FVector(PosicionActual.X + NuevaX, PosicionActual.Y + NuevaY, PosicionActual.Z );

	// Establece la nueva posici�n del actor
	SetActorLocation(NuevaPosicion);
}

void ANaveEnemigaTransporte::Disparar()
{
	// Si es posible disparar
	if (bCanFire)
	{
		// Obtiene la rotación y la ubicación del propietario del componente
		FRotator FireRotation = GetActorRotation() + FRotator(0.0f, 00.0f, 0.0f);
		FVector SpawnLocation = GetActorLocation() + FireRotation.RotateVector(GunOffset);

		UWorld* const World = GetWorld();
		if (World)
		{
			// Calcula la posición de spawn para el primer proyectil (a la izquierda de la nave)
			const FVector SpawnLocationLeft = SpawnLocation - FireRotation.RotateVector(FVector(0.f, 60.f, 0.f));

			// Spawnea el primer proyectil
			World->SpawnActor<AGalaga_USFX_L01Projectile>(SpawnLocationLeft, FireRotation);

			// Calcula la posición de spawn para el segundo proyectil (a la derecha de la nave)
			const FVector SpawnLocationRight = SpawnLocation + FireRotation.RotateVector(FVector(0.f, 60.f, 0.f));

			// Spawnea el segundo proyectil
			World->SpawnActor<AGalaga_USFX_L01Projectile>(SpawnLocationRight, FireRotation);

			// Configuración del temporizador para controlar la velocidad de disparo
			bCanFire = false;
			World->GetTimerManager().SetTimer(TimerHandle_ShotTimerExpired, this, &ANaveEnemigaTransporte::ShotTimerExpired, FireRate);

			// Reproducir el sonido de disparo
			if (FireSound)
			{
				UGameplayStatics::PlaySoundAtLocation(World, FireSound, GetActorLocation());
			}
		}
	}
}

void ANaveEnemigaTransporte::ShotTimerExpired()
{
	bCanFire = true;
}

void ANaveEnemigaTransporte::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Mover(DeltaTime);
	Disparar();
}
