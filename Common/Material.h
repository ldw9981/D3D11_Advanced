#pragma once


using namespace Microsoft::WRL;
using namespace DirectX::SimpleMath;
using namespace std;

struct aiMaterial;


class Material
{
public:
	Material();
	~Material();

	Vector4 m_Ambient = { 1.0f,1.0f,1.0f,1.0f };
	Vector4 m_Diffuse = { 1.0f,1.0f,1.0f,1.0f };
	Vector4 m_Specular = { 1.0f,1.0f,1.0f,1.0f };
	Vector4 m_Emissive = { 1.0f,1.0f,1.0f,1.0f };
	
	void Create(ID3D11Device* device,aiMaterial* pMaterial);
	ID3D11ShaderResourceView* m_pDiffuseRV = nullptr;	// ≈ÿΩ∫√≥ ∏Æº“Ω∫ ∫‰.
	ID3D11ShaderResourceView* m_pNormalRV = nullptr;	// ≥Î∏ª∏  ∏Æº“Ω∫ ∫‰.
	ID3D11ShaderResourceView* m_pSpecularRV = nullptr;	// Ω∫∆‰≈ß∑Ø∏  ∏Æº“Ω∫ ∫‰.
	ID3D11ShaderResourceView* m_pEmissiveRV = nullptr;	// ¿ÃπÃΩ√∫Í∏  ∏Æº“Ω∫ ∫‰.	
	ID3D11ShaderResourceView* m_pOpacityRV = nullptr;	// ≈ı∏Ì∏  ∏Æº“Ω∫ ∫‰.	
};

