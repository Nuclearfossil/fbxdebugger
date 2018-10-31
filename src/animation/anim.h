#pragma once

#include <vector>

const double kFBXTimeUnit = 1.0 / 46186158000.0; // http://help.autodesk.com/cloudhelp/2018/ENU/FBX-Developer-Help/cpp_ref/class_fbx_anim_curve.html

enum TangentModeEnum
{
	TangentModeAuto,
	TangentModeTCB,                         // Tension, Continuity, Bias
	TangentModeInOut,                       // slope to the left equal to slop on the right
	TangentModeGenericBreak,
	TangentModeTangentBreak,
	TangentModeAutoBreak,
	TangentModeGenericClamp,
	TangentModeGenericTimeIndependent,
	TangentModeGenericClampProgressive
};

enum InterpolationTypeEnum
{
	InterpolationTypeConstant,
	InterpolationTypeLinear,
	InterpolationTypeCubic
};

enum WeightModeEnum
{
	WeightModeNone,
	WeightModeRight,
	WeightModeNextLeft,
	WeightModeAll
};

enum VelocityModeEnum
{
	VelocityModeNone,
	VelocityModeRight,
	VelocityModeNextLeft,
	VelocityModeAll
};

enum ConstantModeEnum
{
	ConstantModeStandard,
	ConstantModeNext
};

enum TangentVisibilityEnum
{
	TangentVisibilityShowNone,
	TangentVisibilityShowLeft,
	TangentVisibilityShowRight,
	TangentVisibilityShowBoth
};

enum AnimCurveEnum
{
	AnimCurveTranslationX,
	AnimCurveTranslationY,
	AnimCurveTranslationZ,
	AnimCurveRotationX,
	AnimCurveRotationY,
	AnimCurveRotationZ,
	AnimCurveScaleX,
	AnimCurveScaleY,
	AnimCurveScaleZ,
	AnimCurveWeight
};

struct AnimKey
{
	double Time;
	double Value;
	InterpolationTypeEnum InterpolationType;
	TangentModeEnum TangentMode;
	WeightModeEnum TangentWeightMode;
	VelocityModeEnum TangentVelocityMode;
	ConstantModeEnum ConstantMode;
	TangentVisibilityEnum TangentVisibility;
};

class AnimCurve
{
public:
	AnimCurve(AnimCurveEnum curveType);
	~AnimCurve();

	//
	// Allow us to reserve a pre-know number of keys, so we don't reallocate
	// as the key list grows.
	//
	// capacity:  How large to resize the key array to
	// returns: The number of keys reserved.
	size_t ReserveKeys(size_t initialCapacity);

	void AddKey(double time, double value);

public:
	AnimCurveEnum m_CurveType;

private:
	std::vector<AnimKey> m_Keys;
};

class AnimBlock
{
public:
	AnimBlock();
	~AnimBlock();
	void SetName(const char* name);
	const std::string GetName() { m_Name; }


	AnimCurve* AddCurve(AnimCurveEnum curveType);
	AnimCurve* GetCurve(AnimCurveEnum curveType);

	size_t ReserveKeys(size_t initialCapacity);

private:
	std::vector<AnimCurve> m_Curves;
	std::string m_Name;
};

class Anim
{
public:
	Anim();
	~Anim();

	AnimBlock& AddAnimBlock(const char* name);

private:
	std::vector<AnimBlock> m_Block;
};