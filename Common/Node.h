#pragma once

struct aiNode;
class Model;
struct NodeAnimation;
class Node
{
public:
	Node() {}
	~Node() {}

	std::string m_Name;	
	Math::Matrix m_Local;	
	Math::Matrix m_World;
	std::vector<Node> m_Children;
	std::vector<UINT> m_MeshIndices;
	Node* m_pParent = nullptr;
	NodeAnimation* m_pNodeAnimation = nullptr;

	void Create(Model* model,aiNode* node);
	void UpdateAnimation(float progressTime);
	
	Node* FindNode(const std::string& name);
};	

