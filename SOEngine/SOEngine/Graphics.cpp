#include "Graphics.h"
#include "dxerr.h"
#include <sstream>
#include <d3dcompiler.h>

namespace wrl = Microsoft::WRL;
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")



#define GFX_EXCEPT_NOINFO(hr) Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_NOINFO(hrcall) if( FAILED( hr = (hrcall) ) ) throw Graphics::HrException( __LINE__,__FILE__,hr )

#ifndef NDEBUG
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if( FAILED( hr = (hrcall) ) ) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#define GFX_THROW_INFO_ONLY(call) infoManager.Set(); (call); {auto v = infoManager.GetMessages(); if(!v.empty()) {throw Graphics::InfoException( __LINE__,__FILE__,v);}}
#else
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr) )
#endif

Graphics::Graphics(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hWnd;
	sd.OutputWindow = hWnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;
	UINT swapCreateFlags = 0u;
#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	HRESULT hr;

GFX_THROW_INFO( D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		swapCreateFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pSwap,
		&pDevice,
		nullptr,
		&pContext
	) );

	wrl::ComPtr<ID3D11Resource> pBackBuffer;
	GFX_THROW_INFO(pSwap->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
	GFX_THROW_INFO(pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pTarget));
}

void Graphics::EndFrame()
{
	HRESULT hr;
#ifndef NDEBUG
	infoManager.Set();
#endif
	if (FAILED(hr = pSwap->Present(1u, 0u)))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			throw GFX_DEVICE_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
		}
		else
		{
			throw GFX_EXCEPT(hr);
		}
	}
}

void Graphics::ClearBuffer(float red, float green, float blue) noexcept
{
	const float color[] = { red,green,blue,1.0f };
	pContext->ClearRenderTargetView(pTarget.Get(), color);
}

void Graphics::DrawTriangle()
{
	namespace wrl = Microsoft::WRL;
	HRESULT hr;

	struct Vertex
	{
		struct 
		{
			float x;
			float y;
		} pos;

		struct 
		{
			unsigned char r;
			unsigned char g;
			unsigned char b;
			unsigned char a;
		}color;
	};

	Vertex vertecies[] =
	{
		{ 0.0f,0.5f,255,0,0,0 },
		{ 0.5f,-0.5f,0,255,0,0 },
		{ -0.5f,-0.5f,0,0,255,0 },
		{ -0.3f,0.3f,0,255,0,0 },
		{ 0.3f,0.3f,0,0,255,0 },
		{ 0.0f,-0.8f,255,0,0,0 },
	};

	vertecies[0].color.g = 255;


	wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
	D3D11_BUFFER_DESC buffDesc = {};
	
	buffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffDesc.Usage = D3D11_USAGE_DEFAULT;
	buffDesc.CPUAccessFlags = 0u;
	buffDesc.ByteWidth = sizeof(vertecies);
	buffDesc.StructureByteStride = sizeof(Vertex);
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = vertecies;
	GFX_THROW_INFO(pDevice->CreateBuffer(&buffDesc, &sd, &pVertexBuffer));

	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;
	pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);

	const unsigned short indicies[]
	{
		0,1,2,
		0,2,3,
		0,4,1,
		2,1,5,
	};

	// initializing the buffer
	wrl::ComPtr<ID3D11Buffer> pIndexBuffer;
	D3D11_BUFFER_DESC indbuffDesc = {};
	indbuffDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indbuffDesc.Usage = D3D11_USAGE_DEFAULT;
	indbuffDesc.CPUAccessFlags = 0u;
	indbuffDesc.ByteWidth = sizeof(indicies);
	indbuffDesc.StructureByteStride = sizeof(Vertex);
	D3D11_SUBRESOURCE_DATA indsd = {};
	indsd.pSysMem = indicies;
	GFX_THROW_INFO(pDevice->CreateBuffer(&indbuffDesc, &indsd, &pIndexBuffer));

	pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);

	
	wrl::ComPtr<ID3D11PixelShader> pPixelShader;// pixel shader
	wrl::ComPtr<ID3DBlob> pBlob;
	GFX_THROW_INFO(D3DReadFileToBlob(L"PixelShader.cso", &pBlob));
	GFX_THROW_INFO(pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));

	
	pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);


	
	wrl::ComPtr<ID3D11VertexShader> pVertexShader;// vertex shader
	GFX_THROW_INFO(D3DReadFileToBlob(L"VertexShader.cso", &pBlob));
	GFX_THROW_INFO(pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader));

	
	pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);


	
	wrl::ComPtr<ID3D11InputLayout> pInputLayout;// input layout for the vertex shader vertex(this shit goes in)
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "Position",0,DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "Color",0,DXGI_FORMAT_R8G8B8A8_UNORM, 0, 8u, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	GFX_THROW_INFO(pDevice->CreateInputLayout(
		ied, (UINT)std::size(ied),
		pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),
		&pInputLayout
	));

	
	pContext->IASetInputLayout(pInputLayout.Get());// bind vertex layout

	pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), nullptr);

	pContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP ); // how to draw the polygons

	D3D11_VIEWPORT vp;
	vp.Width = 800;
	vp.Height = 600;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pContext->RSSetViewports(1u, &vp);


	GFX_THROW_INFO_ONLY(pContext->DrawIndexed((UINT)std::size(indicies), 0u, 0u)); // how to draw the triangles, will they be indexed etc
}

Graphics::HrException::HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs) noexcept
	:
	Exception(line, file),
	hr(hr)
{
	for (const auto& m : infoMsgs)
	{
		info += m;
		info.push_back('\n');
	}
	if (!info.empty())
	{
		info.pop_back();
	}
}

const char* Graphics::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Error String] " << GetErrorString() << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl;
	if (!info.empty())
	{
		oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	}
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}


const char* Graphics::HrException::GetType() const noexcept
{
	return "Great Graphics Exception";
}

HRESULT Graphics::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Graphics::HrException::GetErrorString() const noexcept
{
	return DXGetErrorString(hr);
}

std::string Graphics::HrException::GetErrorDescription() const noexcept
{
	char buf[512];
	DXGetErrorDescription(hr, buf, sizeof(buf));
	return buf;
}

std::string Graphics::HrException::GetErrorInfo() const noexcept
{
	return info;
}

const char* Graphics::DeviceRemovedException::GetType() const noexcept
{
	return "Great Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}

Graphics::InfoException::InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept
	:
	Exception(line, file)
{
	for (const auto& m : infoMsgs)
	{
		info += m;
		info.push_back('\n');
	}
	// remove final newline if exists
	if (!info.empty())
	{
		info.pop_back();
	}
}


const char* Graphics::InfoException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::InfoException::GetType() const noexcept
{
	return "Graphics Info Exception";
}

std::string Graphics::InfoException::GetErrorInfo() const noexcept
{
	return info;
}
