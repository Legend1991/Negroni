#pragma once

#include "../Core/Base.h"
#include "../Core/Renderer.h"
#include "Frustum.h"
#include <d3d11.h>
#include <directxmath.h>

#include <map>

using namespace Core;

namespace DirectX
{
	typedef std::pair<ID3D11Buffer*, ID3D11Buffer*> ID3D11BufferPair;

	class D3D11Renderer : public Renderer
	{
	public:
		D3D11Renderer(HWND hWnd);
		~D3D11Renderer();

		virtual void Resize(size_t width, size_t height) override;
		virtual void LoadMesh(const MeshRef& mesh) override;
		virtual void UnloadMesh(const MeshRef& mesh) override;
		virtual void Clear(const rgba color) override;
		virtual void Draw(const GameState&) override;
		virtual void Present() override;
		virtual void SetVSync(bool enabled) override;
		virtual void SetFXAA(bool enabled) override;
		virtual void Cleanup() override;

		ID3D11Device* Device() { return device; };
		ID3D11DeviceContext* DeviceContext() { return deviceContext; }
		void CreateFXAARenderTargets();
		void ReleaseFXAARenderTargets();
		void RenderPassFXAA(ID3D11RenderTargetView* renderTargetView);

	private:
		void InitWindowSize();
		void InitSwapChainDesc();
		void CreateDeviceAndSwapChain();
		void CreateRenderTargetView();
		void CreateDepthStencilView();

		HWND outputWindow;
		UINT width;
		UINT height;

		DXGI_SWAP_CHAIN_DESC	swapChainDesc;
		ID3D11Device*			device = nullptr;
		ID3D11DeviceContext*    deviceContext = nullptr;
		IDXGISwapChain*			swapChain = nullptr;
		ID3D11RenderTargetView* renderTargetView = nullptr;
		ID3D11Texture2D*		depthStencil = nullptr;
		ID3D11DepthStencilView* depthStencilView = nullptr;

		bool vsync = true;
		bool fxaa = true;

		// Shaders
		HRESULT CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
		void SetupViewport();
		void SetupShaders();

		ID3D11VertexShader*   vertexShader = nullptr;
		ID3D11PixelShader*	  pixelShader = nullptr;
		ID3D11GeometryShader* wireframeGShader = nullptr;
		ID3D11PixelShader*    wireframePShader = nullptr;
		ID3D11VertexShader*	  visualizerVShader = nullptr;
		ID3D11PixelShader*    fxaaPShader = nullptr;

		ID3D11InputLayout* vertexLayout = nullptr;
		ID3D11Buffer*	   constantBuffer = nullptr;
		ID3D11Buffer*	   wireframeCBuffer = nullptr;
		XMMATRIX           view;
		XMMATRIX           projection;

		ID3D11Texture2D*          fxaaRenderTexture = nullptr;
		ID3D11RenderTargetView*   fxaaRenderTargetView = nullptr;
		ID3D11ShaderResourceView* fxaaShaderResourceView = nullptr;
		ID3D11SamplerState*       linearSamplerState = nullptr;

		std::vector<ID3D11Buffer*> buffers;

		Ref<Frustum> frustum;
	};
}
