#pragma once

#include "Node.h"

class Take;
typedef std::shared_ptr<Take> TakeSharedPtr;

class Take : public Node
{
public:
	Take() {}
	~Take() {}
};