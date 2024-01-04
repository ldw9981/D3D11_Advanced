#include "pch.h"
#include "TutorialApp.h"

#include "../Common/Helper.h"
#include <d3dcompiler.h>
#include <Directxtk/DDSTextureLoader.h>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


#pragma comment (lib, "d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")


using namespace DirectX;

TutorialApp::TutorialApp(HINSTANCE hInstance)
	:GameApp(hInstance)
{

}

TutorialApp::~TutorialApp()
{
	UninitImGUI();
	UninitD3D();
}

bool TutorialApp::Initialize(UINT Width, UINT Height)
{
	__super::Initialize(Width, Height);

	if (!InitD3D())
		return false;

	if (!InitImGUI())
		return false;

	return true;
}

void TutorialApp::Update()
{
	__super::Update();

	float t = m_Timer.TotalTime();
	
	m_ModelObject.Update(t);
	m_ModelSkyBox.m_WorldPosistion = m_CameraPos;
	m_ModelSkyBox.Update(t);// ��ī�� �ڽ��� ��ġ�� ������� ī�޶� ��ġ�������Ǿ���Ѵ�.

	Math::Matrix world = Math::Matrix::CreateFromYawPitchRoll(
		XMConvertToRadians(m_CameraRot.y), XMConvertToRadians(m_CameraRot.x), XMConvertToRadians(m_CameraRot.z))
		* Math::Matrix::CreateTranslation(m_CameraPos);


	m_View = XMMatrixLookToLH(m_CameraPos, -world.Forward(), world.Up());
	m_Light.EyePosition = m_CameraPos;
}

void TutorialApp::Render()
{
	// Clear the back buffer
	const float clear_color_with_alpha[4] = { m_ClearColor.x , m_ClearColor.y , m_ClearColor.z, 1.0f};
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, clear_color_with_alpha);
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pDeviceContext->IASetInputLayout(m_pInputLayout);


	m_pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pCBTransform);
	m_pDeviceContext->VSSetConstantBuffers(1, 1, &m_pCBDirectionLight);
	m_pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
	m_pDeviceContext->PSSetConstantBuffers(0, 1, &m_pCBTransform);
	m_pDeviceContext->PSSetConstantBuffers(1, 1, &m_pCBDirectionLight);
	m_pDeviceContext->PSSetConstantBuffers(2, 1, &m_pGpuCbMaterial);

	m_pDeviceContext->PSSetSamplers(0, 1, &m_pSamplerLinear);
	m_pDeviceContext->PSSetSamplers(1, 1, &m_pSamplerCube);
	

	//
	// Update matrix variables and lighting variables
	//
	m_Transform.mView = XMMatrixTranspose(m_View);
	m_Transform.mProjection = XMMatrixTranspose(m_Projection);
	m_pDeviceContext->UpdateSubresource(m_pCBTransform, 0, nullptr, &m_Transform, 0, 0);

	m_Light.Direction.Normalize();
	m_pDeviceContext->UpdateSubresource(m_pCBDirectionLight, 0, nullptr, &m_Light, 0, 0);	

	
	m_pDeviceContext->RSSetState(m_CWcullMode);
	m_pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
	m_pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	RenderModel(m_ModelObject);

	m_pDeviceContext->RSSetState(m_CCWcullMode);
	m_pDeviceContext->PSSetShader(m_PixelShaderSkyBox.Get(), nullptr, 0);
	m_pDeviceContext->VSSetShader(m_VertexShaderSkyBox.Get(), nullptr, 0);
	RenderModel(m_ModelSkyBox);



	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	{
		ImGui::Begin("Properties");

		ImGui::Text("Light");
		ImGui::SliderFloat3("LightDirection", (float*)&m_Light.Direction, -1.0f, 1.0f);
		ImGui::ColorEdit4("LightAmbient", (float*)&m_Light.Ambient);
		ImGui::ColorEdit4("LightDiffuse", (float*)&m_Light.Diffuse);
		ImGui::ColorEdit4("LightSpecular", (float*)&m_Light.Specular);

		ImGui::Text("Material");
		ImGui::ColorEdit4("MaterialAmbient", (float*)&m_CpuCbMaterial.Ambient);
		ImGui::ColorEdit4("MaterialDiffuse", (float*)&m_CpuCbMaterial.Diffuse);
		ImGui::ColorEdit4("MaterialSpecular", (float*)&m_CpuCbMaterial.Specular);
		ImGui::ColorEdit4("MaterialEmissive", (float*)&m_CpuCbMaterial.Emissive);
		ImGui::SliderFloat("MaterialSpecularPower", (float*)&m_CpuCbMaterial.SpecularPower, 2.0f, 4096.0f);

		ImGui::Text("Camera");
		ImGui::SliderFloat3("Position", (float*)&m_CameraPos, -2000.0f, 2000.0f);	
		ImGui::SliderFloat3("Rotation", (float*)&m_CameraRot, -180.f, 180.0f);

		ImGui::Text("BackBuffer");
		ImGui::ColorEdit4("clear color", (float*)&m_ClearColor); // Edit 3 floats representing a color	

		ImGui::Text("Model");
		ImGui::SliderFloat3("ModelPosition", (float*)&m_ModelObject.m_WorldPosistion, -2000.0f, 2000.0f);
		ImGui::SliderFloat3("ModelRotation", (float*)&m_ModelObject.m_WorldRotation, -180.f, 180.0f);

		ImGui::End();
	}
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	m_pSwapChain->Present(0, 0);	// Present our back buffer to our front buffer
}

bool TutorialApp::InitD3D()
{
	HRESULT hr = 0;	// �����.

	// ����ü�� �Ӽ� ���� ����ü ����.
	DXGI_SWAP_CHAIN_DESC swapDesc = {};
	swapDesc.BufferCount = 1;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.OutputWindow = m_hWnd;	// ����ü�� ����� â �ڵ� ��.
	swapDesc.Windowed = true;		// â ��� ���� ����.
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// �����(�ؽ�ó)�� ����/���� ũ�� ����.
	swapDesc.BufferDesc.Width = m_ClientWidth;
	swapDesc.BufferDesc.Height = m_ClientHeight;
	// ȭ�� �ֻ��� ����.
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	// ���ø� ���� ����.
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;

	UINT creationFlags = 0;
#ifdef _DEBUG
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	// 1. ��ġ ����.   2.����ü�� ����. 3.��ġ ���ؽ�Ʈ ����.
	HR_T(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, creationFlags, NULL, NULL,
		D3D11_SDK_VERSION, &swapDesc, &m_pSwapChain, &m_pDevice, NULL, &m_pDeviceContext));

	// 4. ����Ÿ�ٺ� ����.  (����۸� �̿��ϴ� ����Ÿ�ٺ�)	
	ID3D11Texture2D* pBackBufferTexture = nullptr;
	HR_T(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBufferTexture));
	HR_T(m_pDevice->CreateRenderTargetView(pBackBufferTexture, NULL, &m_pRenderTargetView));  // �ؽ�ó�� ���� ���� ����
	SAFE_RELEASE(pBackBufferTexture);	//�ܺ� ���� ī��Ʈ�� ���ҽ�Ų��.
	// ���� Ÿ���� ���� ��� ���������ο� ���ε��մϴ�.
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, NULL);

	//5. ����Ʈ ����.	
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)m_ClientWidth;
	viewport.Height = (float)m_ClientHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	m_pDeviceContext->RSSetViewports(1, &viewport);

	//6. �X��&���ٽ� �� ����
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = m_ClientWidth;
	descDepth.Height = m_ClientHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;

	ID3D11Texture2D* textureDepthStencil = nullptr;
	HR_T(m_pDevice->CreateTexture2D(&descDepth, nullptr, &textureDepthStencil));

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	HR_T(m_pDevice->CreateDepthStencilView(textureDepthStencil, &descDSV, &m_pDepthStencilView));
	SAFE_RELEASE(textureDepthStencil);

	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	CreateConstantBuffer();
	CreateBlendState();
	CreateSamplerState();
	CreateRasterizerState();

	CreateD3D_StaticMesh();
	CreateD3D_SkyBox();

	m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, m_ClientWidth / (FLOAT)m_ClientHeight, 1.0f, 1000000.0f);


	return true;
}

void TutorialApp::UninitD3D()
{
	SAFE_RELEASE(m_CCWcullMode);
	SAFE_RELEASE(m_CWcullMode);
	SAFE_RELEASE(m_pGpuCbMaterial);
	SAFE_RELEASE(m_pCBTransform);
	SAFE_RELEASE(m_pCBDirectionLight);
	SAFE_RELEASE(m_pAlphaBlendState);
	SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pInputLayout);
	SAFE_RELEASE(m_pSamplerLinear);

	// Cleanup DirectX
	SAFE_RELEASE(m_pRenderTargetView);
	SAFE_RELEASE(m_pDepthStencilView);
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pDeviceContext);
	SAFE_RELEASE(m_pSwapChain);
}



void TutorialApp::CreateRasterizerState()
{
	HRESULT hr = 0;
	//RasterizerState  �ݴ�� ��ī��ť�� �׸����
	D3D11_RASTERIZER_DESC cmdesc;
	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_BACK;
	cmdesc.FrontCounterClockwise = true;
	HR_T( m_pDevice->CreateRasterizerState(&cmdesc, &m_CCWcullMode));

	cmdesc.FrontCounterClockwise = false;
	HR_T( m_pDevice->CreateRasterizerState(&cmdesc, &m_CWcullMode));
}

void TutorialApp::CreateBlendState()
{
	//7. ���ĺ����� ���� ���� ���� ����
	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	D3D11_RENDER_TARGET_BLEND_DESC rtBlendDesc = {};
	rtBlendDesc.BlendEnable = true; // ���� ��� ����
	// FinalRGB = SrcRGB *SrcBlend + DestRGB*DestBlend
	rtBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
	rtBlendDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;	// SrcBlend�� SrcColor�� ���İ�
	rtBlendDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;	// DestBlend�� (1-SourceColor.a)
	// FinalAlpha = (SrcAlpha * SrcBlendAlpha) + (DestAlpha * DestBlendAlpha)
	rtBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtBlendDesc.SrcBlendAlpha = D3D11_BLEND_ONE;	// SrcBlendAlpha = 1
	rtBlendDesc.DestBlendAlpha = D3D11_BLEND_ONE;	// DestBlendAlpha = 1	
	rtBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL; // ����Ÿ�ٿ� RGBA ��� Write
	blendDesc.RenderTarget[0] = rtBlendDesc;
	HR_T(m_pDevice->CreateBlendState(&blendDesc, &m_pAlphaBlendState));
}

bool TutorialApp::InitImGUI()
{
	/*
		ImGui �ʱ�ȭ.
	*/
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();


	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(m_hWnd);
	ImGui_ImplDX11_Init(this->m_pDevice, this->m_pDeviceContext);

	//
	return true;
}

void TutorialApp::UninitImGUI()
{
	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void TutorialApp::RenderModel(Model& model)
{
	m_Transform.mWorld = XMMatrixTranspose(model.m_World);
	m_pDeviceContext->UpdateSubresource(m_pCBTransform, 0, nullptr, &m_Transform, 0, 0);

	for (size_t i = 0; i < model.m_Meshes.size(); i++)
	{
		size_t mi = model.m_Meshes[i].m_MaterialIndex;
		model.m_Materials[mi].ApplyDeviceContext(m_pDeviceContext, &m_CpuCbMaterial, m_pGpuCbMaterial, m_pAlphaBlendState);
		model.m_Meshes[i].Render(m_pDeviceContext);
	}
}

void TutorialApp::ApplyMaterial(const Material* material)
{
	m_pDeviceContext->PSSetShaderResources(0, 1, &material->m_pDiffuseRV);
	m_pDeviceContext->PSSetShaderResources(1, 1, &material->m_pNormalRV);
	m_pDeviceContext->PSSetShaderResources(2, 1, &material->m_pSpecularRV);
	m_pDeviceContext->PSSetShaderResources(3, 1, &material->m_pEmissiveRV);
	m_pDeviceContext->PSSetShaderResources(4, 1, &material->m_pOpacityRV);

	m_CpuCbMaterial.Diffuse = material->m_Color;
	m_CpuCbMaterial.UseDiffuseMap = material->m_pDiffuseRV != nullptr ? true : false;
	m_CpuCbMaterial.UseNormalMap = material->m_pNormalRV != nullptr ? true : false;
	m_CpuCbMaterial.UseSpecularMap = material->m_pSpecularRV != nullptr ? true : false;
	m_CpuCbMaterial.UseEmissiveMap = material->m_pEmissiveRV != nullptr ? true : false;
	m_CpuCbMaterial.UseOpacityMap = material->m_pOpacityRV != nullptr ? true : false;

	if (m_CpuCbMaterial.UseOpacityMap && m_pAlphaBlendState != nullptr)
		m_pDeviceContext->OMSetBlendState(m_pAlphaBlendState, nullptr, 0xffffffff); // ���ĺ��� ���¼��� , �ٸ��ɼ��� �⺻�� 
	else
		m_pDeviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);	// �������� , �ٸ��ɼ��� �⺻��

	m_pDeviceContext->UpdateSubresource(m_pGpuCbMaterial, 0, nullptr, &m_CpuCbMaterial, 0, 0);
}

void TutorialApp::CreateD3D_StaticMesh()
{
	ID3D10Blob* vertexShaderBuffer = nullptr;
	HRESULT hr=0;
	hr = CompileShaderFromFile(L"08_VS.hlsl", nullptr, "main", "vs_5_0", &vertexShaderBuffer);
	if (FAILED(hr))
	{
		hr = D3DReadFileToBlob(L"08_VS.cso", &vertexShaderBuffer);
	}
	HR_T(hr);

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL" , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT" , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	hr = m_pDevice->CreateInputLayout(layout, ARRAYSIZE(layout),
		vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_pInputLayout);

	// 3. Render() ���� ���������ο� ���ε���  ���ؽ� ���̴� ����
	HR_T(m_pDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), NULL, &m_pVertexShader));
	SAFE_RELEASE(vertexShaderBuffer);

	// 5. Render() ���� ���������ο� ���ε��� �ȼ� ���̴� ����
	ID3D10Blob* pixelShaderBuffer = nullptr;	
	hr = CompileShaderFromFile(L"08_PS.hlsl", nullptr, "main", "ps_5_0", &pixelShaderBuffer);
	if (FAILED(hr))
	{
		hr = D3DReadFileToBlob(L"08_PS.cso", &pixelShaderBuffer);
	}
	HR_T(hr);

	HR_T(m_pDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(),
		pixelShaderBuffer->GetBufferSize(), NULL, &m_pPixelShader));
	SAFE_RELEASE(pixelShaderBuffer);

	m_ModelObject.ReadSceneFile(m_pDevice, "../Resource/cube.fbx");
	m_ModelObject.m_WorldScale = Vector3(50, 50, 50);
}



void TutorialApp::CreateD3D_SkyBox()
{
	ComPtr<ID3D10Blob> vertexShaderBuffer = nullptr;
	HRESULT hr = 0;
	HR_T( CompileShaderFromFile(L"08_VS.hlsl", nullptr, "main_skybox", "vs_5_0", vertexShaderBuffer.GetAddressOf()) );
	HR_T( m_pDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), NULL, &m_VertexShaderSkyBox));


	ComPtr<ID3D10Blob> pixelShaderBuffer = nullptr;
	HR_T( CompileShaderFromFile(L"08_PS.hlsl", nullptr, "main_skybox", "ps_5_0", pixelShaderBuffer.GetAddressOf()));
	HR_T( m_pDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(),
		pixelShaderBuffer->GetBufferSize(), NULL, &m_PixelShaderSkyBox));

	hr = CreateTextureFromFile(m_pDevice, L"../Resource/cubemap.dds", m_ShaderResourceViewCubeMap.GetAddressOf(),
		(ID3D11Resource**)m_TextureCubeMap.GetAddressOf());
	HR_T(hr);

	m_pDeviceContext->PSSetShaderResources(7, 1, m_ShaderResourceViewCubeMap.GetAddressOf());

	// 8. FBX Loading
	m_ModelSkyBox.ReadSceneFile(m_pDevice, "../Resource/Cube.fbx");
	m_ModelSkyBox.m_WorldScale = Vector3(100000, 100000, 100000);
}

void TutorialApp::CreateConstantBuffer()
{
	// 6. Render() ���� ���������ο� ���ε��� ��� ���� ����
	// Create the constant buffer
	D3D11_BUFFER_DESC bd = {};
	bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(CB_Transform);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	HR_T(m_pDevice->CreateBuffer(&bd, nullptr, &m_pCBTransform));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(CB_DirectionLight);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	HR_T(m_pDevice->CreateBuffer(&bd, nullptr, &m_pCBDirectionLight));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(CB_Marterial);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	HR_T(m_pDevice->CreateBuffer(&bd, nullptr, &m_pGpuCbMaterial));
}

void TutorialApp::CreateSamplerState()
{
	// 7. �ؽ�ó ���÷� ����
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HR_T(m_pDevice->CreateSamplerState(&sampDesc, &m_pSamplerLinear));

	// Describe the Sample State
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//Create the Sample State
	HR_T(m_pDevice->CreateSamplerState(&sampDesc, &m_pSamplerCube));
}

Texture TutorialApp::CreateCubeMapTexture(UINT width, UINT height, DXGI_FORMAT format, UINT levels)
{
	Texture texture;
	texture.width = width;
	texture.height = height;
	texture.levels = (levels > 0) ? levels : Utility::numMipmapLevels(width, height);

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = levels;
	desc.ArraySize = 6;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	if (levels == 0) {
		desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		desc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
	}

	if (FAILED(m_pDevice->CreateTexture2D(&desc, nullptr, &texture.texture))) {
		throw std::runtime_error("Failed to create cubemap texture");
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = -1;
	if (FAILED(m_pDevice->CreateShaderResourceView(texture.texture.Get(), &srvDesc, &texture.srv))) {
		throw std::runtime_error("Failed to create cubemap texture SRV");
	}
	return texture;
}


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK TutorialApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	return __super::WndProc(hWnd, message, wParam, lParam);
}
