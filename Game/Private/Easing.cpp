// @Author Fathurahman <ipat.bogor@gmail.com> @ipatizer
// Copyright (c) 2015 Fathur Rahman

#include "GamePrivatePCH.h"
#include "Easing.h"

#define M_PI	3.1415926535897932f
#define M_PI_2	6.2831853071795864f

//////////////////////////////////////////////////////////////////////////
// Ease Functions
//////////////////////////////////////////////////////////////////////////

#pragma region EaseFunctions

// Modeled after the line y = x
static float EaseLinear(float p)
{
	return p;
}

// Modeled after the parabola y = x^2
static float EaseQuadraticIn(float p)
{
	return p * p;
}

// Modeled after the parabola y = -x^2 + 2x
static float EaseQuadraticOut(float p)
{
	return -(p * (p - 2));
}

// Modeled after the piecewise quadratic
// y = (1/2)((2x)^2)             ; [0, 0.5)
// y = -(1/2)((2x-1)*(2x-3) - 1) ; [0.5, 1]
static float EaseQuadraticInOut(float p)
{
	if(p < 0.5)
	{
		return 2 * p * p;
	}
	else
	{
		return (-2 * p * p) + (4 * p) - 1;
	}
}

// Modeled after the cubic y = x^3
static float EaseCubicIn(float p)
{
	return p * p * p;
}

// Modeled after the cubic y = (x - 1)^3 + 1
static float EaseCubicOut(float p)
{
	float f = (p - 1);
	return f * f * f + 1;
}

// Modeled after the piecewise cubic
// y = (1/2)((2x)^3)       ; [0, 0.5)
// y = (1/2)((2x-2)^3 + 2) ; [0.5, 1]
static float EaseCubicInOut(float p)
{
	if(p < 0.5)
	{
		return 4 * p * p * p;
	}
	else
	{
		float f = ((2 * p) - 2);
		return 0.5 * f * f * f + 1;
	}
}

// Modeled after the quartic x^4
static float EaseQuarticIn(float p)
{
	return p * p * p * p;
}

// Modeled after the quartic y = 1 - (x - 1)^4
static float EaseQuarticOut(float p)
{
	float f = (p - 1);
	return f * f * f * (1 - p) + 1;
}

// Modeled after the piecewise quartic
// y = (1/2)((2x)^4)        ; [0, 0.5)
// y = -(1/2)((2x-2)^4 - 2) ; [0.5, 1]
static float EaseQuarticInOut(float p) 
{
	if(p < 0.5)
	{
		return 8 * p * p * p * p;
	}
	else
	{
		float f = (p - 1);
		return -8 * f * f * f * f + 1;
	}
}

// Modeled after the quintic y = x^5
static float EaseQuinticIn(float p) 
{
	return p * p * p * p * p;
}

// Modeled after the quintic y = (x - 1)^5 + 1
static float EaseQuinticOut(float p) 
{
	float f = (p - 1);
	return f * f * f * f * f + 1;
}

// Modeled after the piecewise quintic
// y = (1/2)((2x)^5)       ; [0, 0.5)
// y = (1/2)((2x-2)^5 + 2) ; [0.5, 1]
static float EaseQuinticInOut(float p) 
{
	if(p < 0.5)
	{
		return 16 * p * p * p * p * p;
	}
	else
	{
		float f = ((2 * p) - 2);
		return  0.5 * f * f * f * f * f + 1;
	}
}

// Modeled after quarter-cycle of sine wave
static float EaseSineIn(float p)
{
	return sin((p - 1) * M_PI_2) + 1;
}

// Modeled after quarter-cycle of sine wave (different phase)
static float EaseSineOut(float p)
{
	return sin(p * M_PI_2);
}

// Modeled after half sine wave
static float EaseSineInOut(float p)
{
	return 0.5 * (1 - cos(p * M_PI));
}

// Modeled after shifted quadrant IV of unit circle
static float EaseCircularIn(float p)
{
	return 1 - sqrt(1 - (p * p));
}

// Modeled after shifted quadrant II of unit circle
static float EaseCircularOut(float p)
{
	return sqrt((2 - p) * p);
}

// Modeled after the piecewise circular function
// y = (1/2)(1 - sqrt(1 - 4x^2))           ; [0, 0.5)
// y = (1/2)(sqrt(-(2x - 3)*(2x - 1)) + 1) ; [0.5, 1]
static float EaseCircularInOut(float p)
{
	if(p < 0.5)
	{
		return 0.5 * (1 - sqrt(1 - 4 * (p * p)));
	}
	else
	{
		return 0.5 * (sqrt(-((2 * p) - 3) * ((2 * p) - 1)) + 1);
	}
}

// Modeled after the exponential function y = 2^(10(x - 1))
static float EaseExponentialIn(float p)
{
	return (p == 0.0) ? p : pow(2, 10 * (p - 1));
}

// Modeled after the exponential function y = -2^(-10x) + 1
static float EaseExponentialOut(float p)
{
	return (p == 1.0) ? p : 1 - pow(2, -10 * p);
}

// Modeled after the piecewise exponential
// y = (1/2)2^(10(2x - 1))         ; [0,0.5)
// y = -(1/2)*2^(-10(2x - 1))) + 1 ; [0.5,1]
static float EaseExponentialInOut(float p)
{
	if(p == 0.0 || p == 1.0) return p;
	
	if(p < 0.5)
	{
		return 0.5 * pow(2, (20 * p) - 10);
	}
	else
	{
		return -0.5 * pow(2, (-20 * p) + 10) + 1;
	}
}

// Modeled after the damped sine wave y = sin(13pi/2*x)*pow(2, 10 * (x - 1))
static float EaseElasticIn(float p)
{
	return sin(13 * M_PI_2 * p) * pow(2, 10 * (p - 1));
}

// Modeled after the damped sine wave y = sin(-13pi/2*(x + 1))*pow(2, -10x) + 1
static float EaseElasticOut(float p)
{
	return sin(-13 * M_PI_2 * (p + 1)) * pow(2, -10 * p) + 1;
}

// Modeled after the piecewise exponentially-damped sine wave:
// y = (1/2)*sin(13pi/2*(2*x))*pow(2, 10 * ((2*x) - 1))      ; [0,0.5)
// y = (1/2)*(sin(-13pi/2*((2x-1)+1))*pow(2,-10(2*x-1)) + 2) ; [0.5, 1]
static float EaseElasticInOut(float p)
{
	if(p < 0.5)
	{
		return 0.5 * sin(13 * M_PI_2 * (2 * p)) * pow(2, 10 * ((2 * p) - 1));
	}
	else
	{
		return 0.5 * (sin(-13 * M_PI_2 * ((2 * p - 1) + 1)) * pow(2, -10 * (2 * p - 1)) + 2);
	}
}

// Modeled after the overshooting cubic y = x^3-x*sin(x*pi)
static float EaseBackIn(float p)
{
	return p * p * p - p * sin(p * M_PI);
}

// Modeled after overshooting cubic y = 1-((1-x)^3-(1-x)*sin((1-x)*pi))
static float EaseBackOut(float p)
{
	float f = (1 - p);
	return 1 - (f * f * f - f * sin(f * M_PI));
}

// Modeled after the piecewise overshooting cubic function:
// y = (1/2)*((2x)^3-(2x)*sin(2*x*pi))           ; [0, 0.5)
// y = (1/2)*(1-((1-x)^3-(1-x)*sin((1-x)*pi))+1) ; [0.5, 1]
static float EaseBackInOut(float p)
{
	if(p < 0.5)
	{
		float f = 2 * p;
		return 0.5 * (f * f * f - f * sin(f * M_PI));
	}
	else
	{
		float f = (1 - (2*p - 1));
		return 0.5 * (1 - (f * f * f - f * sin(f * M_PI))) + 0.5;
	}
}


static float EaseBounceOut(float p)
{
	if(p < 4/11.0)
	{
		return (121 * p * p)/16.0;
	}
	else if(p < 8/11.0)
	{
		return (363/40.0 * p * p) - (99/10.0 * p) + 17/5.0;
	}
	else if(p < 9/10.0)
	{
		return (4356/361.0 * p * p) - (35442/1805.0 * p) + 16061/1805.0;
	}
	else
	{
		return (54/5.0 * p * p) - (513/25.0 * p) + 268/25.0;
	}
}

static float EaseBounceIn(float p)
{
	return 1 - EaseBounceOut(1 - p);
}


static float EaseBounceInOut(float p)
{
	if(p < 0.5)
	{
		return 0.5 * EaseBounceIn(p*2);
	}
	else
	{
		return 0.5 * EaseBounceOut(p * 2 - 1) + 0.5;
	}
}

#pragma endregion EaseFunctions

//////////////////////////////////////////////////////////////////////////
// UEasing
//////////////////////////////////////////////////////////////////////////

FEaseFunction UEasing::GetFunction( EEaseType Type )
{
	static TMap<uint8, FEaseFunction> EaseFunctionMap;

	if ( EaseFunctionMap.Num() == 0 )
	{
		EaseFunctionMap.Add( (uint8) EEaseType::Linear, &EaseLinear );
		EaseFunctionMap.Add( (uint8) EEaseType::QuadraticIn, &EaseQuarticIn );
		EaseFunctionMap.Add( (uint8) EEaseType::QuadraticOut, &EaseQuinticOut );
		EaseFunctionMap.Add( (uint8) EEaseType::QuadraticInOut, &EaseQuadraticInOut );
		EaseFunctionMap.Add( (uint8) EEaseType::CubicIn, &EaseCubicIn );
		EaseFunctionMap.Add( (uint8) EEaseType::CubicOut, &EaseCubicOut );
		EaseFunctionMap.Add( (uint8) EEaseType::CubicInOut, &EaseCubicInOut );
		EaseFunctionMap.Add( (uint8) EEaseType::QuarticIn, &EaseQuadraticIn );
		EaseFunctionMap.Add( (uint8) EEaseType::QuarticOut, &EaseQuarticOut );
		EaseFunctionMap.Add( (uint8) EEaseType::QuarticInOut, &EaseQuarticInOut );
		EaseFunctionMap.Add( (uint8) EEaseType::QuinticIn, &EaseQuinticIn );
		EaseFunctionMap.Add( (uint8) EEaseType::QuinticOut, &EaseQuinticOut );
		EaseFunctionMap.Add( (uint8) EEaseType::QuinticInOut, &EaseQuinticInOut );
		EaseFunctionMap.Add( (uint8) EEaseType::SineIn, &EaseSineIn );
		EaseFunctionMap.Add( (uint8) EEaseType::SineOut, &EaseSineOut );
		EaseFunctionMap.Add( (uint8) EEaseType::SineInOut, &EaseSineInOut );
		EaseFunctionMap.Add( (uint8) EEaseType::CircularIn, &EaseCircularIn );
		EaseFunctionMap.Add( (uint8) EEaseType::CircularOut, &EaseCircularOut );
		EaseFunctionMap.Add( (uint8) EEaseType::CircularInOut, &EaseCircularInOut );
		EaseFunctionMap.Add( (uint8) EEaseType::ElasticIn, &EaseElasticIn );
		EaseFunctionMap.Add( (uint8) EEaseType::ElasticOut, &EaseElasticOut );
		EaseFunctionMap.Add( (uint8) EEaseType::ElasticInOut, &EaseElasticInOut );
		EaseFunctionMap.Add( (uint8) EEaseType::BackIn, &EaseBackIn );
		EaseFunctionMap.Add( (uint8) EEaseType::BackOut, &EaseBackOut );
		EaseFunctionMap.Add( (uint8) EEaseType::BackInOut, &EaseBackInOut );
		EaseFunctionMap.Add( (uint8) EEaseType::BounceIn, &EaseBounceIn );
		EaseFunctionMap.Add( (uint8) EEaseType::BounceOut, &EaseBounceOut );
		EaseFunctionMap.Add( (uint8) EEaseType::BounceInOut, &EaseBounceInOut );
	}

	return EaseFunctionMap.FindRef( (uint8) Type );
}

//////////////////////////////////////////////////////////////////////////
// EaseCurve
//////////////////////////////////////////////////////////////////////////

float FEaseCurve::Ease( float Alpha ) const
{
	if (CurveFloat) 
	{
		// Using curve float.
		float MinTime, MaxTime;
		CurveFloat->GetTimeRange(MinTime, MaxTime);

		float StartValue = CurveFloat->FloatCurve.GetFirstKey().Value;
		float FinalValue = CurveFloat->FloatCurve.GetLastKey().Value;

		if (MinTime != MaxTime && StartValue != FinalValue)
		{
			float CurveFloatTime = MinTime + (Alpha * (MaxTime - MinTime));
			float CurveFloatValue = CurveFloat->GetFloatValue(CurveFloatTime);
			return (CurveFloatValue - StartValue) / (FinalValue - StartValue);
		}
	}
	else if (EaseType != EEaseType::Linear)
	{
		// Using non linear easing curve.
		return UEasing::Ease( EaseType, Alpha );
	}

	// Using linear easing curve.
	return Alpha;
}
