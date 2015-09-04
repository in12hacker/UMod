// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "WeaponBase.h"
#include "UModCharacter.generated.h"

class UInputComponent;

UCLASS(config=Game)
class AUModCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;
public:
	AUModCharacter(const FObjectInitializer& ObjectInitializer);

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AUModProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	class USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation;

protected:
	
	/** Fires a projectile. */
	void OnFire();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface
public:
	//@ServerSide Gives a weapon to the player
	virtual void GiveWeapon(FString base);
	//@ServerSide Removes the current weapon the player holds
	virtual void RemoveActiveWeapon();
	//@ServerSide Removes all weapons this player is holding
	virtual void StripWeapons();
	//@Shared Returns all weapons
	virtual AWeaponBase** GetWeapons();
	//@Shared returns the weapon the player holds
	virtual AWeaponBase* GetActiveWeapon();
	//@ServerSide Switchs to another weapon slot
	virtual void SwitchWeapon(uint8 id);

	virtual void AUModCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty, FDefaultAllocator> & OutLifetimeProps) const;	
private:
	//The weapon matrix which stores all pointers to all AActors corresponding to those weapons
	UPROPERTY(ReplicatedUsing = UpdateClientSideData)
	AWeaponBase *weapons[16];
	//The current weapon slot id which corresponds to the weapon the player holds
	UPROPERTY(ReplicatedUsing = UpdateClientSideData)
	uint8 curWeapon;
	//This function is used to create an attachement between player and weapon
	virtual void UpdateAttachement();
	//This function is used to sync weapons with clients
	virtual void OnSwitchChanged();

	//Function to be called right after some vars have been replicated.
	UFUNCTION()
	void UpdateClientSideData();

	UFUNCTION(Server, UnReliable, WithValidation)
	void OnPlayerClick(uint8 but);
	void OnPlayerClick_Implementation(uint8 but);
	bool OnPlayerClick_Validate(uint8 but);
};

