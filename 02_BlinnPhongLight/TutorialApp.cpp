#include "TutorialApp.h"

#include "../Common/Helper.h"
#include <d3dcompiler.h>
#include <Directxtk/DDSTextureLoader.h>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")



TutorialApp::TutorialApp(HINSTANCE hInstance)
	:GameApp(hInstance)
{

}

TutorialApp::~TutorialApp()
{
	UninitScene();	
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

	if (!InitScene())
		return false;

	return true;
}

void TutorialApp::Update()
{
	__super::Update();

	float t = GameTimer::m_Instance->TotalTime();

	m_World =  Matrix::CreateScale(m_MeshScale) * Matrix::CreateFromYawPitchRoll(Vector3(XMConvertToRadians(m_Rotation.x), XMConvertToRadians(m_Rotation.y),0));
	
	m_View = XMMatrixLookToLH(m_CameraPos, Vector3(0, 0, 1), Vector3(0, 1, 0));

	m_Light.EyePosition = m_CameraPos;

}

void TutorialApp::Render()
{
	// Clear the back buffer
	const float clear_color_with_alpha[4] = { m_ClearColor.x , m_ClearColor.y , m_ClearColor.z, 1.0f};
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, clear_color_with_alpha);
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);


	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &m_VertexBufferStride, &m_VertexBufferOffset);
	m_pDeviceContext->IASetInputLayout(m_pInputLayout);
	m_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	m_pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pCBTransform);
	m_pDeviceContext->VSSetConstantBuffers(1, 1, &m_pCBDirectionLight);
	m_pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
	m_pDeviceContext->PSSetConstantBuffers(0, 1, &m_pCBTransform);
	m_pDeviceContext->PSSetConstantBuffers(1, 1, &m_pCBDirectionLight);
	m_pDeviceContext->PSSetConstantBuffers(2, 1, &m_pCBMaterial);
	m_pDeviceContext->PSSetShaderResources(0, 1, &m_pTextureRV);
	m_pDeviceContext->PSSetSamplers(0, 1, &m_pSamplerLinear);

	//
	// Update matrix variables and lighting variables
	//
	m_Transform.mWorld = XMMatrixTranspose(m_World);
	m_Transform.mView = XMMatrixTranspose(m_View);
	m_Transform.mProjection = XMMatrixTranspose(m_Projection);
	m_pDeviceContext->UpdateSubresource(m_pCBTransform, 0, nullptr, &m_Transform, 0, 0);
	m_pDeviceContext->UpdateSubresource(m_pCBDirectionLight, 0, nullptr, &m_Light, 0, 0);
	m_pDeviceContext->UpdateSubresource(m_pCBMaterial, 0, nullptr, &m_Material, 0, 0);

	m_pDeviceContext->DrawIndexed(m_nIndices, 0, 0);

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	{
		ImGui::Begin("Properties");
		ImGui::Text("Cube");            
		ImGui::SliderFloat("Scale", (float*)&m_MeshScale, 10, 1000);
		ImGui::SliderFloat2("Rotation",(float*)&m_Rotation, 0, 90);	

		ImGui::Text("Light");
		ImGui::SliderFloat3("LightDirection", (float*)&m_Light.Direction, -1.0f, 1.0f);
		ImGui::ColorEdit4("LightAmbient", (float*)&m_Light.Ambient);
		ImGui::ColorEdit4("LightDiffuse", (float*)&m_Light.Diffuse);
		ImGui::ColorEdit4("LightSpecular", (float*)&m_Light.Specular);

		ImGui::Text("Material");
		ImGui::ColorEdit4("MaterialAmbient", (float*)&m_Material.Ambient);
		ImGui::ColorEdit4("MaterialDiffuse", (float*)&m_Material.Diffuse);
		ImGui::ColorEdit4("MaterialSpecular", (float*)&m_Material.Specular);
		ImGui::SliderFloat("MaterialSpecularPower", (float*)&m_Material.SpecularPower, 2.0f, 4096.0f);

		ImGui::Text("Camera");
		ImGui::SliderFloat3("Position", (float*)&m_CameraPos, -5000.0f, 5000.0f);

		ImGui::Text("BackBuffer");
		ImGui::ColorEdit4("clear color", (float*)&m_ClearColor); // Edit 3 floats representing a color	
		ImGui::End();
	}
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	m_pSwapChain->Present(0, 0);	// Present our back buffer to our front buffer
}

bool TutorialApp::InitD3D()
{
	HRESULT hr = 0;	// 결과값.

	// 스왑체인 속성 설정 구조체 생성.
	DXGI_SWAP_CHAIN_DESC swapDesc = {};
	swapDesc.BufferCount = 1;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.OutputWindow = m_hWnd;	// 스왑체인 출력할 창 핸들 값.
	swapDesc.Windowed = true;		// 창 모드 여부 설정.
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// 백버퍼(텍스처)의 가로/세로 크기 설정.
	swapDesc.BufferDesc.Width = m_ClientWidth;
	swapDesc.BufferDesc.Height = m_ClientHeight;
	// 화면 주사율 설정.
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	// 샘플링 관련 설정.
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;

	UINT creationFlags = 0;
#ifdef _DEBUG
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	// 1. 장치 생성.   2.스왑체인 생성. 3.장치 컨텍스트 생성.
	HR_T(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, creationFlags, NULL, NULL,
		D3D11_SDK_VERSION, &swapDesc, &m_pSwapChain, &m_pDevice, NULL, &m_pDeviceContext));

	// 4. 렌더타겟뷰 생성.  (백버퍼를 이용하는 렌더타겟뷰)	
	ID3D11Texture2D* pBackBufferTexture = nullptr;
	HR_T(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBufferTexture));
	HR_T(m_pDevice->CreateRenderTargetView(pBackBufferTexture, NULL, &m_pRenderTargetView));  // 텍스처는 내부 참조 증가
	SAFE_RELEASE(pBackBufferTexture);	//외부 참조 카운트를 감소시킨다.
	// 렌더 타겟을 최종 출력 파이프라인에 바인딩합니다.
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, NULL);

	//5. 뷰포트 설정.	
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)m_ClientWidth;
	viewport.Height = (float)m_ClientHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	m_pDeviceContext->RSSetViewports(1, &viewport);

	//6. 뎊스&스텐실 뷰 생성
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
	return true;
}

void TutorialApp::UninitD3D()
{
	// Cleanup DirectX
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pDeviceContext);
	SAFE_RELEASE(m_pSwapChain);
	SAFE_RELEASE(m_pRenderTargetView);
}

// 1. Render() 에서 파이프라인에 바인딩할 버텍스 버퍼및 버퍼 정보 준비
// 2. Render() 에서 파이프라인에 바인딩할 InputLayout 생성 	
// 3. Render() 에서 파이프라인에 바인딩할  버텍스 셰이더 생성
// 4. Render() 에서 파이프라인에 바인딩할 인덱스 버퍼 생성
// 5. Render() 에서 파이프라인에 바인딩할 픽셀 셰이더 생성
// 6. Render() 에서 파이프라인에 바인딩할 상수 버퍼 생성
bool TutorialApp::InitScene()
{
	HRESULT hr=0; // 결과값.
	// 1. Render() 에서 파이프라인에 바인딩할 버텍스 버퍼및 버퍼 정보 준비
	// Local or Object or Model Space
	Vertex vertices[] =
	{
		{ Vector3(-1.0f, 1.0f,-1.0f), Vector2(1.0f, 0.0f),Vector3( 0.0f, 1.0f, 0.0f) },  // 윗면이라 y전부 +1
		{ Vector3( 1.0f, 1.0f,-1.0f), Vector2(0.0f, 0.0f),Vector3( 0.0f, 1.0f, 0.0f) },
		{ Vector3( 1.0f, 1.0f, 1.0f), Vector2(0.0f, 1.0f),Vector3( 0.0f, 1.0f, 0.0f) },
		{ Vector3(-1.0f, 1.0f, 1.0f), Vector2(1.0f, 1.0f),Vector3( 0.0f, 1.0f, 0.0f) },
					
		{ Vector3(-1.0f,-1.0f,-1.0f), Vector2(0.0f, 0.0f),Vector3( 0.0f,-1.0f, 0.0f) },  // 아랫면이라 y전부 -1
		{ Vector3( 1.0f,-1.0f,-1.0f), Vector2(1.0f, 0.0f),Vector3( 0.0f,-1.0f, 0.0f) },
		{ Vector3( 1.0f,-1.0f, 1.0f), Vector2(1.0f, 1.0f),Vector3( 0.0f,-1.0f, 0.0f) },
		{ Vector3(-1.0f,-1.0f, 1.0f), Vector2(0.0f, 1.0f),Vector3( 0.0f,-1.0f, 0.0f) },
					 
		{ Vector3(-1.0f,-1.0f, 1.0f), Vector2(0.0f, 1.0f),Vector3(-1.0f, 0.0f, 0.0f) },	// 왼쪽면 이라 x전부 -1
		{ Vector3(-1.0f,-1.0f,-1.0f), Vector2(1.0f, 1.0f),Vector3(-1.0f, 0.0f, 0.0f) },
		{ Vector3(-1.0f, 1.0f,-1.0f), Vector2(1.0f, 0.0f),Vector3(-1.0f, 0.0f, 0.0f) },
		{ Vector3(-1.0f, 1.0f, 1.0f), Vector2(0.0f, 0.0f),Vector3(-1.0f, 0.0f, 0.0f) },
						 	    
		{ Vector3( 1.0f,-1.0f, 1.0f), Vector2(1.0f, 1.0f),Vector3( 1.0f, 0.0f, 0.0f) },	// 오른쪽면 이라 x전부 +1
		{ Vector3( 1.0f,-1.0f,-1.0f), Vector2(0.0f, 1.0f),Vector3( 1.0f, 0.0f, 0.0f) },
		{ Vector3( 1.0f, 1.0f,-1.0f), Vector2(0.0f, 0.0f),Vector3( 1.0f, 0.0f, 0.0f) },
		{ Vector3( 1.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f),Vector3( 1.0f, 0.0f, 0.0f) },
						    
		{ Vector3(-1.0f,-1.0f,-1.0f), Vector2(0.0f, 1.0f),Vector3( 0.0f, 0.0f,-1.0f) },  // 앞면이라 z전부 -1
		{ Vector3( 1.0f,-1.0f,-1.0f), Vector2(1.0f, 1.0f),Vector3( 0.0f, 0.0f,-1.0f) },
		{ Vector3( 1.0f, 1.0f,-1.0f), Vector2(1.0f, 0.0f),Vector3( 0.0f, 0.0f,-1.0f) },
		{ Vector3(-1.0f, 1.0f,-1.0f), Vector2(0.0f, 0.0f),Vector3( 0.0f, 0.0f,-1.0f) },
							    
		{ Vector3(-1.0f,-1.0f, 1.0f), Vector2(1.0f, 1.0f),Vector3( 0.0f, 0.0f, 1.0f) },	//뒷면이라 z전부 +1
		{ Vector3( 1.0f,-1.0f, 1.0f), Vector2(0.0f, 1.0f),Vector3( 0.0f, 0.0f, 1.0f) },
		{ Vector3( 1.0f, 1.0f, 1.0f), Vector2(0.0f, 0.0f),Vector3( 0.0f, 0.0f, 1.0f) },
		{ Vector3(-1.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f),Vector3( 0.0f, 0.0f, 1.0f) },
	};

	D3D11_BUFFER_DESC bd = {};
	bd.ByteWidth = sizeof(Vertex) * ARRAYSIZE(vertices);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA vbData = {};
	vbData.pSysMem = vertices;
	HR_T( m_pDevice->CreateBuffer(&bd, &vbData, &m_pVertexBuffer));	

	// 버텍스 버퍼 정보
	m_VertexBufferStride = sizeof(Vertex);
	m_VertexBufferOffset = 0;


	// 2. Render() 에서 파이프라인에 바인딩할 InputLayout 생성 	
	ID3D10Blob* vertexShaderBuffer = nullptr;
	hr = CompileShaderFromFile(L"02_BlinnPhong_VS.hlsl", "main", "vs_4_0", &vertexShaderBuffer);
	if (FAILED(hr))
	{
		hr = D3DReadFileToBlob(L"02_BlinnPhong_VS.cso", &vertexShaderBuffer);
	}
	HR_T(hr);

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },	
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL" , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	hr = m_pDevice->CreateInputLayout(layout, ARRAYSIZE(layout),
		vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_pInputLayout);

	// 3. Render() 에서 파이프라인에 바인딩할  버텍스 셰이더 생성
	HR_T(m_pDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), NULL, &m_pVertexShader));
	SAFE_RELEASE(vertexShaderBuffer);

	// 4. Render() 에서 파이프라인에 바인딩할 인덱스 버퍼 생성
	WORD indices[] =
	{
		3,1,0, 2,1,3,
		6,4,5, 7,4,6,
		11,9,8, 10,9,11,
		14,12,13, 15,12,14,
		19,17,16, 18,17,19,
		22,20,21, 23,20,22
	};

	// 인덱스 개수 저장.
	m_nIndices = ARRAYSIZE(indices);

	bd = {};
	bd.ByteWidth = sizeof(WORD) * ARRAYSIZE(indices);
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA ibData = {};
	ibData.pSysMem = indices;
	HR_T(m_pDevice->CreateBuffer(&bd, &ibData, &m_pIndexBuffer));


	// 5. Render() 에서 파이프라인에 바인딩할 픽셀 셰이더 생성
	ID3D10Blob* pixelShaderBuffer = nullptr;
	hr = CompileShaderFromFile(L"02_BlinnPhong_PS.hlsl", "main", "ps_4_0", &pixelShaderBuffer);
	if (FAILED(hr))
	{
		hr = D3DReadFileToBlob(L"02_BlinnPhong_PS.cso", &pixelShaderBuffer);
	}
	HR_T(hr);

	HR_T(m_pDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(),
		pixelShaderBuffer->GetBufferSize(), NULL, &m_pPixelShader));
	SAFE_RELEASE(pixelShaderBuffer);


	// 6. Render() 에서 파이프라인에 바인딩할 상수 버퍼 생성
	// Create the constant buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(CB_Transform);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	HR_T( m_pDevice->CreateBuffer(&bd, nullptr, &m_pCBTransform));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(CB_DirectionLight);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	HR_T(m_pDevice->CreateBuffer(&bd, nullptr, &m_pCBDirectionLight));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(CB_Marterial);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	HR_T(m_pDevice->CreateBuffer(&bd, nullptr, &m_pCBMaterial));


	// Load the Texture
	HR_T( CreateDDSTextureFromFile(m_pDevice, L"seafloor.dds", nullptr, &m_pTextureRV));

	// Create the sample state
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HR_T( m_pDevice->CreateSamplerState(&sampDesc, &m_pSamplerLinear));


	// Initialize the world matrix
	m_World = XMMatrixIdentity();


	// Initialize the view matrix
	XMVECTOR Eye = XMVectorSet(0.0f, 0.0f, -1000.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	m_View = XMMatrixLookAtLH(Eye, At, Up);

	// Initialize the projection matrix
	m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, m_ClientWidth / (FLOAT)m_ClientHeight, 0.01f, 10000.0f);


	m_Light.Direction = { 0.0f, 0.0f, 1.0f };
	m_Light.Diffuse = { 1.0f, 1.0f, 1.0f,1.0f };
	return true;
}

void TutorialApp::UninitScene()
{
	SAFE_RELEASE(m_pCBMaterial);
	SAFE_RELEASE(m_pCBTransform);
	SAFE_RELEASE(m_pCBDirectionLight);

	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pInputLayout);
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pDepthStencilView);
}
bool TutorialApp::InitImGUI()
{
	/*
		ImGui 초기화.
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

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK TutorialApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	return __super::WndProc(hWnd, message, wParam, lParam);
}
