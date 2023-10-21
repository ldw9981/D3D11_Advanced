#pragma once

struct aiNode;
class Model;
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


	void Create(Model* model,aiNode* node);
	void Update(float deltaTime);
	
};	

