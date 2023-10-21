#pragma once

struct aiNode;
class Model;
struct NodeAnimation;
class Node
{
public:
	Node() {}
	~Node() {}

	Node* m_pParent=nullptr;
	std::string m_Name;
	std::vector<Node> m_Children;
	Math::Matrix m_World;
	Math::Matrix m_Local;
	std::vector<UINT> m_MeshIndices;
	NodeAnimation* m_pNodeAnimation = nullptr;

	void Create(Model* model,aiNode* node);
	void UpdateAnimation(float progressTime);
	
	Node* FindNode(const std::string& name);
};	

