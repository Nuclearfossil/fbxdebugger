#pragma once

#include <string>
#include <vector>

class Node;
class Program;

namespace fbxsdk
{
	class FbxNode;
}

typedef std::shared_ptr<Node> NodeSharedPtr;

class Node
{
public:
	Node() {}
	~Node() {}

	virtual void Render(bool highlighted, Program* program) {}

	// std::shared_ptr requires at least one virutal funtion to make this class polymorphic
	// So, may as well have a 'debug' method, for now
	virtual void Debug() {}

	std::string Name;
	fbxsdk::FbxNode* FbxNodeRef;

	std::vector<NodeSharedPtr> Children;
};
