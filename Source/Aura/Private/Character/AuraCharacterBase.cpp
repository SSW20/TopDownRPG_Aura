// Fill out your copyright notice in the Description page of Project Settings.
#include "Character/AuraCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Aura/Aura.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void AAuraCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAuraCharacterBase, bIsStunned);
	DOREPLIFETIME(AAuraCharacterBase, bIsBurned);
	DOREPLIFETIME(AAuraCharacterBase, bIsShocking);
}

// Sets default values
AAuraCharacterBase::AAuraCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BurnNiagaraComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>(TEXT("BurnDebuffNiagaraComponent"));
	BurnNiagaraComponent->SetupAttachment(GetRootComponent());
	BurnNiagaraComponent->DebuffTag = FAuraGameplayTags::Get().Debuff_Burn;

	StunNiagaraComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>(TEXT("StunDebuffNiagaraComponent"));
	StunNiagaraComponent->SetupAttachment(GetRootComponent());
	StunNiagaraComponent->DebuffTag = FAuraGameplayTags::Get().Debuff_Stun;

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

void AAuraCharacterBase::OnRep_Burned()
{
}

void AAuraCharacterBase::StunTagChanged(const FGameplayTag Tag, int32 Count)
{
	bIsStunned = Count > 0;
	GetCharacterMovement()->MaxWalkSpeed = bIsStunned ? 0.f : MoveSpeed;
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

void AAuraCharacterBase::Die(const FVector& DeathImpulseVector)
{
	Weapon->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	MulticastHandleDeath(DeathImpulseVector);
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

FOnASCRegistered& AAuraCharacterBase::GetASCRegistered()
{
	return ASCRegisteredDelegate;
}

void AAuraCharacterBase::MulticastHandleDeath_Implementation(const FVector& DeathImpuseVector)
{
	UGameplayStatics::PlaySoundAtLocation(this, DeathSoundBase,GetActorLocation(), GetActorRotation());
	Weapon->SetSimulatePhysics(true);
	Weapon->SetEnableGravity(true);
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	Weapon->AddImpulse(DeathImpuseVector * 0.1f,NAME_None, true);
	
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	GetMesh()->AddImpulse(DeathImpuseVector,NAME_None, true);
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	Dissolve();

	bDead = true;
	BurnNiagaraComponent->Deactivate();
	StunNiagaraComponent->Deactivate();
	OnDeathDelegate.Broadcast(this);
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


USkeletalMeshComponent* AAuraCharacterBase::GetWeapon_Implementation()
{
	return Weapon; 
}

void AAuraCharacterBase::SetIsShocking_Implementation(bool IsShocking)
{
	bIsShocking = IsShocking;
}

bool AAuraCharacterBase::IsShocking_Implementation() const
{
	return bIsShocking;
}

FOnDeathSignature& AAuraCharacterBase::GetOnDeathDelegate()
{
	return OnDeathDelegate;
}
