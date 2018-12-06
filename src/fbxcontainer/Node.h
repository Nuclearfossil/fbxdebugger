#pragma once

#include <string>
#include <vector>
#include <memory>

class Node;
class Program;

namespace fbxsdk
{
	class FbxNode;
	class FbxNodeAttribute;
}

typedef std::shared_ptr<Node> NodeSharedPtr;

class Node
{
public:
	Node()
	{
		NodeType = ""; // Don't display anything, we're just a node
	}
	~Node() {}

	virtual void Render(bool highlighted, Program* program) {}

	// std::shared_ptr requires at least one virutal funtion to make this class polymorphic
	// So, may as well have a 'debug' method, for now
	virtual void Debug() {}

	std::string Name;
	std::string NodeType;
	fbxsdk::FbxNode* FbxNodeRef;
	fbxsdk::FbxNodeAttribute* FbxNodeAttr;

	std::vector<NodeSharedPtr> Children;
};
