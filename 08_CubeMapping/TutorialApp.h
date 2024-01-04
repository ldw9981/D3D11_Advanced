#pragma once
#include "../Common/GameApp.h"
#include "../Common/Mesh.h"
#include "../Common/Material.h"
#include "../Common/Model.h"
#include "../Common/Texture.h"	

struct CB_Transform
{
	Math::Matrix mWorld;
	Math::Matrix mView;
	Math::Matrix mProjection;
}; 

struct CB_DirectionLight
{
	Math::Vector3 Direction = { 0.5f, 0.0f, 0.8f };
	float pad0;
	Math::Vector4 Ambient = { 0.1f,0.1f,0.1f,0.1f };
	Math::Vector4 Diffuse = { 1.0f,1.0f,1.0f,1.0f };
	Math::Vector4 Specular = { 1.0f,1.0f,1.0f,1.0f };
	Math::Vector3 EyePosition;
	float pad1;
};


class TutorialApp :
    public GameApp
{
public:
	TutorialApp(HINSTANCE hInstance);
	~TutorialApp();

	// ������ ������������ �����ϴ� �ʼ� ��ü�� �������̽�
	ID3D11Device* m_pDevice = nullptr;						// ����̽�
	ID3D11DeviceContext* m_pDeviceContext = nullptr;		// ��� ����̽� ���ؽ�Ʈ
	IDXGISwapChain* m_pSwapChain = nullptr;					// ����ü��
	ID3D11RenderTargetView* m_pRenderTargetView = nullptr;	// ������ Ÿ�ٺ�
	ID3D11DepthStencilView* m_pDepthStencilView = nullptr;	// ����/���ٽ� ��

	// ������ ���������ο� �����ϴ� ���ҽ� ��ü�� �������̽�
	ID3D11RasterizerState* m_CCWcullMode = nullptr;
	ID3D11RasterizerState* m_CWcullMode = nullptr;

	ID3D11VertexShader* m_pVertexShader = nullptr;		// ���� ���̴�.
	ID3D11PixelShader* m_pPixelShader = nullptr;		// �ȼ� ���̴�.


	ID3D11InputLayout* m_pInputLayout = nullptr;		// �Է� ���̾ƿ�.
	ID3D11SamplerState* m_pSamplerLinear = nullptr;		// ���÷� ����.
	ID3D11SamplerState* m_pSamplerCube = nullptr;		// ť��� ���÷�

	ID3D11BlendState* m_pAlphaBlendState = nullptr;		// ���� ���� ���� (������ó�������� �������)
	ID3D11Buffer* m_pGpuCbMaterial = nullptr;				// ��� ����: ��ȯ���
	ID3D11Buffer* m_pCBTransform = nullptr;				// ��� ����: ��ȯ���
	ID3D11Buffer* m_pCBDirectionLight = nullptr;		// ��� ����: ���Ɽ
			
	Math::Matrix  m_View;					// ����ǥ�� �������� ��ȯ�� ���� ���.
	Math::Matrix  m_Projection;			// ������ġ��ǥ��( Normalized Device Coordinate) �������� ��ȯ�� ���� ���.

	Math::Vector3 m_ClearColor ={ 0.0f, 0.0f, 0.0f};

	Math::Vector3 m_CameraRot = { 0.0f, 0.0f, 0.0f }; // degree
	Math::Vector3 m_CameraPos = Math::Vector3(0.0f, 200.0f, -1000.0f);
	

	CB_Transform m_Transform;
	CB_Marterial m_CpuCbMaterial;
	CB_DirectionLight m_Light;
	float m_MeshScale=1.0f;
	
	
	Model m_ModelSkyBox;
	Model m_ModelObject;


	ComPtr<ID3D11VertexShader> m_VertexShaderSkyBox;		// ���� ���̴�.
	ComPtr<ID3D11PixelShader> m_PixelShaderSkyBox;		// �ȼ� ���̴�.	
	ComPtr<ID3D11Texture3D> m_TextureCubeMap;
	ComPtr<ID3D11ShaderResourceView> m_ShaderResourceViewCubeMap;

	virtual bool Initialize(UINT Width, UINT Height);
	virtual void Update();
	virtual void Render();

	bool InitD3D();			
	void UninitD3D();

	bool InitImGUI();
	void UninitImGUI();

	void RenderModel(Model& model);
	void ApplyMaterial(const Material* material);

	void CreateD3D_StaticMesh();
	void CreateD3D_SkyBox();
	void CreateConstantBuffer();
	void CreateSamplerState();
	void CreateRasterizerState();
	void CreateBlendState();

	Texture CreateCubeMapTexture(UINT width, UINT height, DXGI_FORMAT format, UINT levels);

	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

