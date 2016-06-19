/** @file Easing.h
* An interpolation / curve functions for any kind of usage.
* They're not as advanced as FMath interpolation and curve functions, but they're blueprint friendly and easy to use.
*
* @Note: Loosely based on Robert Penner easing in action script.

* @author Fathurahman <ipat.bogor@gmail.com>
*/
 
#pragma once

#include "Easing.generated.h"

typedef float (*FEaseFunction)(float);

UENUM(BlueprintType)
enum class EEaseType: uint8
{
	Linear,
	QuadraticIn,
	QuadraticOut,
	QuadraticInOut,
	CubicIn,
	CubicOut,
	CubicInOut,
	QuarticIn,
	QuarticOut,
	QuarticInOut,
	QuinticIn,
	QuinticOut,
	QuinticInOut,
	SineIn,
	SineOut,
	SineInOut,
	CircularIn,
	CircularOut,
	CircularInOut,
	ElasticIn,
	ElasticOut,
	ElasticInOut,
	BackIn,
	BackOut,
	BackInOut,
	BounceIn,
	BounceOut,
	BounceInOut
};

//////////////////////////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct GAME_API FEaseCurve
{	
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Easing")
	EEaseType EaseType;

	/** If this is set to a valid curve float, then the ease curve will use it instead of the EaseType. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Easing")
	class UCurveFloat* CurveFloat;

public:

	FEaseCurve()
		: EaseType( EEaseType::Linear )
		, CurveFloat( nullptr )
	{
	}

	FEaseCurve( EEaseType InEaseType )
		: EaseType( InEaseType )
		, CurveFloat( nullptr )
	{
	}

	FEaseCurve( UCurveFloat* InCurveFloat )
		: CurveFloat( InCurveFloat )
	{
	}

	float Ease( float Alpha ) const;

	FORCEINLINE float Ease( float ElapsedTime, float Duration ) const
	{
		return Duration > 0.f ? Ease( ElapsedTime / Duration ) : 0.f;
	}

	template<class T>
	FORCEINLINE T Interp( const T& A, const T& B, float Alpha ) const
	{
		return FMath::LerpStable( A, B, Ease( Alpha ) );
	}
};

//////////////////////////////////////////////////////////////////////////

UCLASS()
class GAME_API UEasing : public UBlueprintFunctionLibrary
{	
	GENERATED_BODY()

public:

	static FEaseFunction GetFunction(EEaseType Type);

	UFUNCTION(BlueprintPure, Category="Easing")
	static float Ease( EEaseType Type, float Alpha ) 
	{
		return (*GetFunction( Type ))( Alpha );
	}

	UFUNCTION(BlueprintPure, Category="Easing")
	static float Ease2( EEaseType Type, float ElapsedTime, float Duration )
	{
		return Duration > 0.f ? ( (*GetFunction( Type )) ( ElapsedTime / Duration ) ) : 0.f;
	}
};
