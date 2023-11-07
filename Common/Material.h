#pragma once

struct aiMaterial;



class Material
{
public:
	Material();
	~Material();

	void Create(ID3D11Device* device,aiMaterial* pMaterial);
	
	std::wstring m_FilePathDiffuse;
	std::wstring m_FilePathNormal;
	std::wstring m_FilePathSpecular;
	std::wstring m_FilePathEmissive;
	std::wstring m_FilePathOpacity;

	Math::Vector4 m_Color;	
	
	ID3D11ShaderResourceView* m_pDiffuseRV = nullptr;	// ≈ÿΩ∫√≥ ∏Æº“Ω∫ ∫‰.
	ID3D11ShaderResourceView* m_pNormalRV = nullptr;	// ≥Î∏ª∏  ∏Æº“Ω∫ ∫‰.
	ID3D11ShaderResourceView* m_pSpecularRV = nullptr;	// Ω∫∆‰≈ß∑Ø∏  ∏Æº“Ω∫ ∫‰.
	ID3D11ShaderResourceView* m_pEmissiveRV = nullptr;	// ¿ÃπÃΩ√∫Í∏  ∏Æº“Ω∫ ∫‰.	
	ID3D11ShaderResourceView* m_pOpacityRV = nullptr;	// ≈ı∏Ì∏  ∏Æº“Ω∫ ∫‰.	
};

