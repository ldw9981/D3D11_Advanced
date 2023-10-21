#pragma once

#include "Mesh.h"
#include "Material.h"
#include "Animation.h"
#include "Node.h"


class Model
{
public:
	Model() {}
	~Model() {}
	
	std::vector<Mesh> m_Meshes;
	std::vector<Matrix*> m_MeshesWorldMatrix;
	std::vector<Material> m_Materials;
	std::vector<NodeAnimation> m_NodeAnimations;
	Node m_RootNode;

	bool ReadFile(ID3D11Device* device,const char* filePath);

	void LinkInstance();
	Material* GetMaterial(UINT index);

	void Update(float deltaTime);

};

