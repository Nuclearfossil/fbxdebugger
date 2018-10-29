#include "anim.h"
#include "misc/Utils.h"

AnimCurve::AnimCurve(AnimCurveEnum curveType)
{
	m_CurveType = curveType;
}

AnimCurve::~AnimCurve()
{}

//
// Our intent is not to shrink the list below the size of the currently allocated keys and `reserve` does this automatically.
//
//   see: http://www.cplusplus.com/reference/vector/vector/reserve/
//     "If n is greater than the current vector capacity, the function causes the container to reallocate
//      its storage increasing its capacity to n (or greater). In all other cases, the function call does
//      not cause a reallocation and the vector capacity is not affected."
//
size_t AnimCurve::ReserveKeys(size_t capacity)
{
	m_Keys.reserve(capacity);

	return m_Keys.capacity();
}

AnimBlock::AnimBlock()
{
}

AnimBlock::~AnimBlock()
{
}

AnimCurve* AnimBlock::AddCurve(AnimCurveEnum curveType)
{
	AnimCurve* curve = GetCurve(curveType);

	if (curve = nullptr)
	{
		AnimCurve curveToAdd(curveType);
		m_Curves.push_back(curveToAdd);
		curve = &m_Curves[curveType];
	}
	return curve;
}

AnimCurve* AnimBlock::GetCurve(AnimCurveEnum curveType)
{
	// I'm iterating over a small list, rather than building out a dictionary or populating an indexed container.
	unsigned int curveCount = SizeT2UInt32(m_Curves.size());
	for (unsigned int index = 0; index < curveCount; index++)
	{
		if (m_Curves[index].m_CurveType == curveType)
		{
			return &m_Curves[index];
		}
	}

	return nullptr;
}

size_t AnimBlock::ReserveKeys(size_t initialCapacity)
{
	m_Curves.reserve(initialCapacity);

	return m_Curves.capacity();
}

Anim::Anim()
{}

Anim::~Anim()
{}