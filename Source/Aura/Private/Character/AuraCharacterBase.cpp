// Fill out your copyright notice in the Description page of Project Settings.
#include "Character/AuraCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Aura/Aura.h"
#include "Kismet/GameplayStatics.h"
// Sets default values
AAuraCharacterBase::AAuraCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);
}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

// Called when the game starts or when spawned
void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAuraCharacterBase::InitAbilityActorInfo()
{
}

void AAuraCharacterBase::AddGameplayAbilities() const
{
	// 자신이 서버인지 확인함
	if (!HasAuthority())	return;

	UAuraAbilitySystemComponent* AuraASC = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	AuraASC->AddGameplayAbilities(StartGameplayAbilities);
	AuraASC->AddPassiveGameplayAbilities(StartPassiveGameplayAbilities);
}

FVector AAuraCharacterBase::GetCombatSocketLocation_Implementation(const FGameplayTag& Tag)
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	if (IsValid(Weapon) && Tag.MatchesTagExact(GameplayTags.Socket_Weapon))
	{
		return Weapon->GetSocketLocation(WeaponSocketName);
	}
	if (Tag.MatchesTagExact(GameplayTags.Socket_LeftHand))
	{
		return GetMesh()->GetSocketLocation(LeftHandSocketName);
	}
	if (Tag.MatchesTagExact(GameplayTags.Socket_RightHand))
	{
		return GetMesh()->GetSocketLocation(RightHandSocketName);
	}
	if (Tag.MatchesTagExact(GameplayTags.Socket_Tail))
	{
		return GetMesh()->GetSocketLocation(TailSocketName);
	}
	return FVector();
}



void AAuraCharacterBase::ApplyEffectToSelfInit(TSubclassOf<UGameplayEffect> EffectClass, float level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(EffectClass);

	FGameplayEffectContextHandle EffectContext = GetAbilitySystemComponent()->MakeEffectContext();
	EffectContext.AddSourceObject(this);
	const FGameplayEffectSpecHandle EffectSpec = GetAbilitySystemComponent()->MakeOutgoingSpec(EffectClass, level, EffectContext);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*EffectSpec.Data.Get(), GetAbilitySystemComponent());
	
}

bool AAuraCharacterBase::IsDead_Implementation() const
{
	return bDead;
}

AActor* AAuraCharacterBase::GetActor_Implementation()
{
	return this;
}

void AAuraCharacterBase::Die()
{
	Weapon->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	MulticastHandleDeath();
}

TArray<FTaggedMontage> AAuraCharacterBase::GetTaggedMontages_Implementation()
{
	return  TaggedMontages;
}

UNiagaraSystem* AAuraCharacterBase::GetBloodEffect_Implementation()
{
	return BloodEffect;
}

FTaggedMontage AAuraCharacterBase::GetTaggedMontageByTag_Implementation(const FGameplayTag& Tag)
{
	for (FTaggedMontage& Montage : TaggedMontages)
	{
		if (Montage.AttackTag.MatchesTag(Tag))
		{
			return Montage;
		}
	}
	return FTaggedMontage();
}

int32 AAuraCharacterBase::GetSummonCount_Implementation()
{
	return SummonCount;
}

void AAuraCharacterBase::IncreaseSummonCount_Implementation(int32 AddValue)
{
	SummonCount += AddValue;
}

ECharacterClass AAuraCharacterBase::GetCharacterClass_Implementation()
{
	return CharacterClass;
}

void AAuraCharacterBase::MulticastHandleDeath_Implementation()
{
	UGameplayStatics::PlaySoundAtLocation(this, DeathSoundBase,GetActorLocation(), GetActorRotation());
	Weapon->SetSimulatePhysics(true);
	Weapon->SetEnableGravity(true);
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	Dissolve();

	bDead = true;
}

void AAuraCharacterBase::Dissolve()
{
	if (IsValid(CharacterMaterial))
	{
		UMaterialInstanceDynamic* CharacterDynamicMaterial = UMaterialInstanceDynamic::Create(CharacterMaterial, this);
		GetMesh()->SetMaterial(0, CharacterDynamicMaterial);
		StartCharacterDisolveTimeLine(CharacterDynamicMaterial);

	}
	if (IsValid(WeaponMaterial))
	{
		UMaterialInstanceDynamic* WeaponDynamicMaterial = UMaterialInstanceDynamic::Create(WeaponMaterial, this);
		Weapon->SetMaterial(0, WeaponDynamicMaterial);
		StartWeaponDisolveTimeLine(WeaponDynamicMaterial);
	}
}

UAnimMontage* AAuraCharacterBase::GetHitReactMontage_Implementation()
{
	return HitReaction;
}

void AAuraCharacterBase::InitializeDefaultAttributes() const
{
	ApplyEffectToSelfInit(DefaultPrimaryAttributes, 1.f);
	ApplyEffectToSelfInit(DefaultSecondaryAttributes, 1.f);
	ApplyEffectToSelfInit(DefaultVitalAttributes, 1.f);
}


