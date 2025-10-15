#include "D3D11Renderer.h"

#include "../Core/Clock.h"
#include "../Core/Logger.h"
#include "../Core/Keyboard.h"
#include "../Core/Profiler.h"

#define SAFE_RELEASE(res) if (res) { res->Release(); res = nullptr; }

#include <numbers>
#include <ranges>

// Shaders
#include <d3dcompiler.h>

using namespace DirectX;

struct SimpleVertex
{
    XMFLOAT3 Pos;
    XMFLOAT3 Normal;
    XMFLOAT2 Tex;
};

struct ConstantBuffer
{
    XMMATRIX world;
    XMMATRIX view;
    XMMATRIX projection;
    XMFLOAT4 tintColor;
};

struct WireframeCBuffer
{
    XMMATRIX world;
    XMMATRIX view;
    XMMATRIX projection;
    XMFLOAT4 wireframeColor;
};

DirectX::D3D11Renderer::D3D11Renderer(HWND hWnd) : outputWindow(hWnd)
{
    InitWindowSize();
    InitSwapChainDesc();
    CreateDeviceAndSwapChain();
    CreateRenderTargetView();
    CreateDepthStencilView();
    deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
    SetupViewport();
    // Shaders
    SetupShaders();
}

DirectX::D3D11Renderer::~D3D11Renderer()
{
    Cleanup();
}

void DirectX::D3D11Renderer::CreateFXAARenderTargets()
{
    HRESULT hr = S_OK;

    // Create the render texture
    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    hr = device->CreateTexture2D(&textureDesc, nullptr, &fxaaRenderTexture);
    if (FAILED(hr))
    {
        log_error("Failed to create FXAA render texture");
        return;
    }

    // Create render target view
    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc = {};
    renderTargetViewDesc.Format = textureDesc.Format;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDesc.Texture2D.MipSlice = 0;

    hr = device->CreateRenderTargetView(fxaaRenderTexture, &renderTargetViewDesc, &fxaaRenderTargetView);
    if (FAILED(hr))
    {
        log_error("Failed to create FXAA render target view");
        return;
    }

    // Create shader resource view
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
    shaderResourceViewDesc.Format = textureDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;

    hr = device->CreateShaderResourceView(fxaaRenderTexture, &shaderResourceViewDesc, &fxaaShaderResourceView);
    if (FAILED(hr))
    {
        log_error("Failed to create FXAA shader resource view");
        return;
    }
}

void DirectX::D3D11Renderer::ReleaseFXAARenderTargets()
{
    SAFE_RELEASE(fxaaShaderResourceView);
    SAFE_RELEASE(fxaaRenderTargetView);
    SAFE_RELEASE(fxaaRenderTexture);
}

void DirectX::D3D11Renderer::Resize(size_t newWidth, size_t newHeight)
{
    if (width == newWidth && height == newHeight) return;

    width = newWidth;
    height = newHeight;

    log_info("resize to {} x {}", width, height);

    SAFE_RELEASE(renderTargetView);
    SAFE_RELEASE(depthStencil);
    SAFE_RELEASE(depthStencilView);
    ReleaseFXAARenderTargets();

    swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);

    CreateRenderTargetView();
    CreateDepthStencilView();
    CreateFXAARenderTargets();

    deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

    SetupViewport();

    projection = XMMatrixPerspectiveFovLH(XM_PI * 0.25f, width / (FLOAT)height, 0.5f, 1000.0f);
}

void DirectX::D3D11Renderer::Clear(const rgba color)
{
    deviceContext->ClearRenderTargetView(renderTargetView, color);
    deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

inline XMMATRIX XM_CALLCONV LHXMMatrixScaling(const Vec3f& scale) noexcept
{
    return XMMatrixScaling(scale.x, scale.y, scale.z);
}

inline XMMATRIX XM_CALLCONV LHXMMatrixRotationRollPitchYaw(const Rot3f& rotation) noexcept
{
    float pitch = rotation.pitch * std::numbers::pi / 180.0f;
    float yaw   = rotation.yaw * std::numbers::pi / 180.0f;
    float roll  = rotation.roll * std::numbers::pi / 180.0f;

    return XMMatrixRotationRollPitchYaw(roll, pitch, yaw);
}

inline XMMATRIX XM_CALLCONV LHXMMatrixTranslation(const Vec3f& location) noexcept
{
    return XMMatrixTranslation(location.x, location.y, location.z);
}

inline XMMATRIX XM_CALLCONV LHXMMatrixTransformation(const Transform& transform) noexcept
{
    // M = XMMatrixScaling * XMMatrixRotationRollPitchYaw * XMMatrixTranslation;

    constexpr float k = std::numbers::pi / 180.0f;

    float pitch = transform.rotation.pitch * k;
    float yaw   = transform.rotation.yaw * k;
    float roll  = transform.rotation.roll * k;

    float SP = sinf(-pitch), SY = sinf(yaw), SR = sinf(roll);
    float CP = cosf(-pitch), CY = cosf(yaw), CR = cosf(roll);

    float r0x = (CP * CY) * transform.scale.x;
    float r0y = (CP * SY) * transform.scale.x;
    float r0z = (SP) * transform.scale.x;

    float r1x = (SR * SP * CY - CR * SY) * transform.scale.y;
    float r1y = (SR * SP * SY + CR * CY) * transform.scale.y;
    float r1z = (-SR * CP) * transform.scale.y;

    float r2x = (-(CR * SP * CY + SR * SY)) * transform.scale.z;
    float r2y = (CY * SR - CR * SP * SY) * transform.scale.z;
    float r2z = (CR * CP) * transform.scale.z;

    //XMMATRIX M;
    //M.r[0] = DirectX::XMVectorSet(r0x, r0y, r0z, 0.f);
    //M.r[1] = DirectX::XMVectorSet(r1x, r1y, r1z, 0.f);
    //M.r[2] = DirectX::XMVectorSet(r2x, r2y, r2z, 0.f);
    //M.r[3] = DirectX::XMVectorSet(transform.location.x, transform.location.y, transform.location.z, 1.f);
    //return M;

    // Apply XMMatrixTranspose
    XMMATRIX M;
    M.r[0] = DirectX::XMVectorSet(r0x, r1x, r2x, transform.location.x);
    M.r[1] = DirectX::XMVectorSet(r0y, r1y, r2y, transform.location.y);
    M.r[2] = DirectX::XMVectorSet(r0z, r1z, r2z, transform.location.z);
    M.r[3] = DirectX::XMVectorSet(0.f, 0.f, 0.f, 1.f);
    return M;
}

void DirectX::D3D11Renderer::Draw(const GameState& gameState)
{
    ProfileBlock("[Renderer] Draw");

    // PASS 1: Render scene to FXAA render target
    ID3D11RenderTargetView* currentRenderTargetView = fxaa ? fxaaRenderTargetView : renderTargetView;
    deviceContext->OMSetRenderTargets(1, &currentRenderTargetView, depthStencilView);

    // Clear the FXAA render target
    constexpr float HEX_0C = 12.0f / 255.0f;
    const float clearColor[4] = { HEX_0C, HEX_0C, HEX_0C, 1.0f };
    deviceContext->ClearRenderTargetView(currentRenderTargetView, clearColor);
    deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    u32 renderCount = 0;
    u32 cullCount = 0;

    for (const auto& object : gameState.objects)
    {
        if (object->mesh == nullptr || object->mesh->data == nullptr)
        {
            //log_warn("Mesh \"{}\" is not loaded into video memory", object->mesh ? object->mesh->name : "<uknown mesh name>");
            continue;
        }

        if (!frustum->CheckSphere(object))
        {
            cullCount++;
            continue;
        }

        ID3D11BufferPair& meshBuffer = *(ID3D11BufferPair*)object->mesh->data;
        UINT stride = sizeof(SimpleVertex);
        UINT offset = 0;
        deviceContext->IASetVertexBuffers(0, 1, &meshBuffer.first, &stride, &offset);
        deviceContext->IASetIndexBuffer(meshBuffer.second, DXGI_FORMAT_R16_UINT, 0);
        deviceContext->IASetInputLayout(vertexLayout);
        deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        deviceContext->VSSetShader(vertexShader, nullptr, 0);
        deviceContext->VSSetConstantBuffers(0, 1, &constantBuffer);
        deviceContext->PSSetShader(pixelShader, nullptr, 0);
        deviceContext->PSSetConstantBuffers(0, 1, &constantBuffer);

        //--------------------------------------------------------------------------------------------------------------
        // Unity Game Objects vs Entities: performance benchmark:
        // https://www.youtube.com/watch?v=kKGiEz1enzw
        // https://github.com/visual-decomplicator/game-objects-vs-entities/blob/main/Game%20objects/CubeController.cs
        //--------------------------------------------------------------------------------------------------------------

        //XMMATRIX world = LHXMMatrixScaling(object->transform.scale) * LHXMMatrixRotationRollPitchYaw(object->transform.rotation) * LHXMMatrixTranslation(object->transform.location);
        //XMMATRIX world = LHXMMatrixTransformation(object->transform); // ~40% faster than above

        float tintColor[4] = { 1.0f, 1.0f, 1.0f, object->useTintColor ? 1.0f : 0.0f };
        memcpy(tintColor, object->tintColor, sizeof(float) * 3);

        ConstantBuffer cb = {
          .world      = LHXMMatrixTransformation(object->transform), //XMMatrixTranspose(world),
          .view       = XMMatrixTranspose(view),
          .projection = XMMatrixTranspose(projection),
          .tintColor  = XMFLOAT4(tintColor),
        };
        deviceContext->UpdateSubresource(constantBuffer, 0, nullptr, &cb, 0, 0);
        deviceContext->DrawIndexed(object->mesh->indices.size(), 0, 0);

#if defined(DEVELOPER)
        if (object->selected && false)
        {
            //deviceContext->VSSetShader(vertexShader, nullptr, 0);
            deviceContext->GSSetShader(wireframeGShader, nullptr, 0);
            deviceContext->PSSetShader(wireframePShader, nullptr, 0);
            deviceContext->VSSetConstantBuffers(0, 1, &wireframeCBuffer);
            deviceContext->GSSetConstantBuffers(0, 1, &wireframeCBuffer);
            deviceContext->PSSetConstantBuffers(0, 1, &wireframeCBuffer);

            Transform wireframe = object->transform;
            wireframe.scale.x += 0.001f;
            wireframe.scale.y += 0.001f;
            wireframe.scale.z += 0.001f;

            float time = Clock::now() / 1000.0f;
            float blinksPerSecond = 1.0f;
            float blinkFactor = (sin(time * blinksPerSecond * 2.0f * 3.14159f) + 1.0f) * 0.5f;
            float wireframeColor[4] = { blinkFactor, 0.0f, blinkFactor, 1.0f };

            WireframeCBuffer cb = {
              .world = LHXMMatrixTransformation(wireframe),
              .view = XMMatrixTranspose(view),
              .projection = XMMatrixTranspose(projection),
              .wireframeColor = XMFLOAT4(wireframeColor),
            };
            deviceContext->UpdateSubresource(wireframeCBuffer, 0, nullptr, &cb, 0, 0);

            deviceContext->DrawIndexed(object->mesh->indices.size(), 0, 0);

            deviceContext->GSSetShader(nullptr, nullptr, 0);
            deviceContext->VSSetConstantBuffers(0, 1, &constantBuffer);
        }
#endif // DEVELOPER

        renderCount++;
    }

    this->rendered = renderCount;
    this->culled = cullCount;

    // PASS 2: Apply FXAA and render to back buffer
    if (fxaa) RenderPassFXAA(renderTargetView);
}

void DirectX::D3D11Renderer::RenderPassFXAA(ID3D11RenderTargetView* renderTargetView)
{
    ProfileBlock("RenderPassFXAA");

    constexpr float HEX_0C = 12.0f / 255.0f;
    const float clearColor[4] = { HEX_0C, HEX_0C, HEX_0C, 1.0f };

    deviceContext->OMSetRenderTargets(1, &renderTargetView, nullptr);

    // Clear the back buffer
    deviceContext->ClearRenderTargetView(renderTargetView, clearColor);

    // Set up for full-screen triangle
    ID3D11Buffer* nullBuffer = nullptr;
    UINT nullUINT = 0;
    deviceContext->IASetVertexBuffers(0, 1, &nullBuffer, &nullUINT, &nullUINT);
    deviceContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
    deviceContext->IASetInputLayout(nullptr);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Set FXAA shaders
    deviceContext->VSSetShader(visualizerVShader, nullptr, 0);
    deviceContext->PSSetShader(fxaaPShader, nullptr, 0);

    // Bind the rendered scene as texture input for FXAA
    deviceContext->PSSetShaderResources(20, 1, &fxaaShaderResourceView);
    deviceContext->PSSetSamplers(0, 1, &linearSamplerState);

    // Render full-screen triangle
    deviceContext->Draw(3, 0);

    // Unbind shader resources
    ID3D11ShaderResourceView* nullSRV = nullptr;
    deviceContext->PSSetShaderResources(20, 1, &nullSRV);
}

void DirectX::D3D11Renderer::Present()
{
    swapChain->Present((UINT)vsync, 0);
}

void DirectX::D3D11Renderer::SetVSync(bool enabled)
{
    vsync = enabled;
}

void DirectX::D3D11Renderer::SetFXAA(bool enabled)
{
    fxaa = enabled;
}

void DirectX::D3D11Renderer::Cleanup()
{
    // Shaders
    ReleaseFXAARenderTargets();
    SAFE_RELEASE(linearSamplerState);
    SAFE_RELEASE(wireframeCBuffer);
    SAFE_RELEASE(constantBuffer);
    // Should be freed in reverse order to how it was created
    for (auto buffer : buffers | std::views::reverse)
    {
        SAFE_RELEASE(buffer); // indexBuffer then vertexBuffer
    }
    buffers.clear();
    SAFE_RELEASE(vertexLayout);
    SAFE_RELEASE(vertexShader);
    SAFE_RELEASE(pixelShader);
    SAFE_RELEASE(wireframeGShader);
    SAFE_RELEASE(wireframePShader);
    SAFE_RELEASE(depthStencil);
    SAFE_RELEASE(depthStencilView);
    SAFE_RELEASE(renderTargetView);
    SAFE_RELEASE(swapChain);
    SAFE_RELEASE(deviceContext);
    SAFE_RELEASE(device);
}

void DirectX::D3D11Renderer::InitWindowSize()
{
    RECT clientRect;
    GetClientRect(outputWindow, &clientRect);
    width = clientRect.right - clientRect.left;
    height = clientRect.bottom - clientRect.top;
}

void DirectX::D3D11Renderer::InitSwapChainDesc()
{
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
    swapChainDesc.BufferCount = 2;
    swapChainDesc.BufferDesc.Width = width;
    swapChainDesc.BufferDesc.Height = height;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = outputWindow;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.Windowed = TRUE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
}

void DirectX::D3D11Renderer::CreateDeviceAndSwapChain()
{
    UINT createDeviceFlags = 0;
#if defined(DEBUG)
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };

    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, 
                                                featureLevelArray, 2, D3D11_SDK_VERSION, &swapChainDesc, &swapChain, 
                                                &device, &featureLevel, &deviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED)
    {
        log_info("use high-performance WARP software driver");
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, 
                                            featureLevelArray, 2, D3D11_SDK_VERSION, &swapChainDesc, &swapChain, 
                                            &device, &featureLevel, &deviceContext);
    }
    else
    {
        log_info("use high-performance HARDWARE driver");
    }
}

void DirectX::D3D11Renderer::CreateRenderTargetView()
{
    ID3D11Texture2D* backBuffer;

    HRESULT bufferResult = swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
    if (FAILED(bufferResult))
    {
        log_error("call swapChain->GetBuffer failed");
        throw std::runtime_error("call swapChain->GetBuffer failed");
    }

    HRESULT viewResult = device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
    if (FAILED(viewResult))
    {
        log_error("call device->CreateRenderTargetView failed");
        throw std::runtime_error("call device->CreateRenderTargetView failed");
    }

    backBuffer->Release();
}

void DirectX::D3D11Renderer::CreateDepthStencilView()
{
    HRESULT hr = S_OK;

    // Create depth stencil texture
    D3D11_TEXTURE2D_DESC descDepth = {};
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    hr = device->CreateTexture2D(&descDepth, nullptr, &depthStencil);
    if (FAILED(hr))
    {
        log_error("call device->CreateTexture2D failed");
        throw std::runtime_error("call device->CreateTexture2D failed");
        return;
    }

    // Create the depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr = device->CreateDepthStencilView(depthStencil, &descDSV, &depthStencilView);
    if (FAILED(hr))
    {
        log_error("call device->CreateDepthStencilView failed");
        throw std::runtime_error("call device->CreateDepthStencilView failed");
        return;
    }
}

void DirectX::D3D11Renderer::SetupViewport()
{
    D3D11_VIEWPORT vp = {};
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;

    deviceContext->RSSetViewports(1, &vp);
}

HRESULT DirectX::D3D11Renderer::CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG)
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;

    // Disable optimizations to further improve shader debugging
    dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ID3DBlob* pErrorBlob = nullptr;
    HRESULT hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel, dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
    if (FAILED(hr))
    {
        if (pErrorBlob)
        {
            OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
            pErrorBlob->Release();
        }
        return hr;
    }
    if (pErrorBlob) pErrorBlob->Release();

    return S_OK;
}

void DirectX::D3D11Renderer::SetupShaders()
{
    HRESULT hr = S_OK;

    //------------------------------------------------------------------------------------------------------------------
    // TintColor vertex shader
    //------------------------------------------------------------------------------------------------------------------

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    hr = CompileShaderFromFile(L"DirectX\\Shaders\\TintColor.fxh", "VS", "vs_4_0", &pVSBlob);
    if (FAILED(hr))
    {
        log_error("TintColor.fxh (VS) cannot be compiled. Please run this executable from the directory that contains the FX file.");
        return;
    }

    // Create the vertex shader
    hr = device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &vertexShader);
    if (FAILED(hr))
    {
        pVSBlob->Release();
        return;
    }

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
    hr = device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &vertexLayout);
    pVSBlob->Release();
    if (FAILED(hr))
        return;

    // Set the input layout
    deviceContext->IASetInputLayout(vertexLayout);

    //------------------------------------------------------------------------------------------------------------------
    // Visualizer vertex shader
    //------------------------------------------------------------------------------------------------------------------

    // Compile the visualizer vertex shader
    pVSBlob = nullptr;
    hr = CompileShaderFromFile(L"DirectX\\Shaders\\Visualizer.hlsl", "VisualizeVS", "vs_4_0", &pVSBlob);
    if (FAILED(hr))
    {
        log_error("Visualizer.hlsl (VisualizeVS) cannot be compiled. Please run this executable from the directory that contains the FX file.");
        return;
    }

    // Create the visualizer vertex shader
    hr = device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &visualizerVShader);
    if (FAILED(hr))
    {
        pVSBlob->Release();
        return;
    }

    //------------------------------------------------------------------------------------------------------------------
    // TintColor pixel shader
    //------------------------------------------------------------------------------------------------------------------

    // Compile the pixel shader
    ID3DBlob* pPSBlob = nullptr;
    hr = CompileShaderFromFile(L"DirectX\\Shaders\\TintColor.fxh", "PS", "ps_4_0", &pPSBlob);
    if (FAILED(hr))
    {
        log_error("TintColor.fxh (PS) cannot be compiled. Please run this executable from the directory that contains the FX file.");
        return;
    }

    // Create the pixel shader
    hr = device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &pixelShader);
    pPSBlob->Release();
    if (FAILED(hr))
        return;

    //------------------------------------------------------------------------------------------------------------------
    // FXAA pixel shader
    //------------------------------------------------------------------------------------------------------------------

    // Compile the FXAA pixel shader
    pPSBlob = nullptr;
    hr = CompileShaderFromFile(L"DirectX\\Shaders\\FXAA.hlsl", "FXAAPS", "ps_5_0", &pPSBlob);
    if (FAILED(hr))
    {
        log_error("FXAA.hlsl (FXAAPS) cannot be compiled. Please run this executable from the directory that contains the FX file.");
        return;
    }

    // Create the FXAA pixel shader
    hr = device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &fxaaPShader);
    pPSBlob->Release();
    if (FAILED(hr))
        return;

    //------------------------------------------------------------------------------------------------------------------
    // Wireframe geometry shader
    //------------------------------------------------------------------------------------------------------------------

    // Compile the wireframe geometry shader
    ID3DBlob* pGSBlob = nullptr;
    hr = CompileShaderFromFile(L"DirectX\\Shaders\\Wireframe.fxh", "GS", "gs_5_0", &pGSBlob);
    if (FAILED(hr))
    {
        log_error("Wireframe.fxh (GS) cannot be compiled. Please run this executable from the directory that contains the FX file. {}", HRESULT_CODE(hr));
        return;
    }

    // Create the wireframe pixel shader
    hr = device->CreateGeometryShader(pGSBlob->GetBufferPointer(), pGSBlob->GetBufferSize(), nullptr, &wireframeGShader);
    pGSBlob->Release();
    if (FAILED(hr))
        return;

    //------------------------------------------------------------------------------------------------------------------
    // Wireframe pixel shader
    //------------------------------------------------------------------------------------------------------------------

    // Compile the wireframe pixel shader
    pPSBlob = nullptr;
    hr = CompileShaderFromFile(L"DirectX\\Shaders\\Wireframe.fxh", "PS", "ps_5_0", &pPSBlob);
    if (FAILED(hr))
    {
        log_error("Wireframe.fxh (PS) cannot be compiled. Please run this executable from the directory that contains the FX file.");
        return;
    }

    // Create the wireframe pixel shader
    hr = device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &wireframePShader);
    pPSBlob->Release();
    if (FAILED(hr))
        return;

    //------------------------------------------------------------------------------------------------------------------

    //LoadMesh(Core::CubeMesh);

    // Set primitive topology
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Create the constant buffer
    D3D11_BUFFER_DESC bufferDesc = {};
    ZeroMemory(&bufferDesc, sizeof(bufferDesc));
    bufferDesc.MiscFlags = 0;
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof(ConstantBuffer);
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    hr = device->CreateBuffer(&bufferDesc, nullptr, &constantBuffer);
    if (FAILED(hr))
        return;

    // Create the wireframe constant buffer
    bufferDesc.ByteWidth = sizeof(WireframeCBuffer);
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    hr = device->CreateBuffer(&bufferDesc, nullptr, &wireframeCBuffer);
    if (FAILED(hr))
        return;

    // Initialize the world matrix
    //world1 = XMMatrixIdentity();
    //world2 = XMMatrixIdentity();

    // Initialize the view matrix
    XMVECTOR eye = XMVectorSet(0.0f, 5.0f, -5.0f, 0.0f);
    XMVECTOR at = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    view = XMMatrixLookAtLH(eye, at, up);

    // Initialize the projection matrix
    projection = XMMatrixPerspectiveFovLH(XM_PI * 0.25f, width / (FLOAT)height, 0.5f, 1000.0f);

    frustum = MakeRef<Frustum>(1000.0f, projection, view);

    // NOTE: Should we keep it here instead of Draw method?
    // Because it's just bounding shaders to a stage in the pipeline.
    // Or it should be called right before concrete Draw call because it stricly bounded to the rendered object?
    // For example if we want to add a glow effect to the object, but we don't want to apply it to every object.
    //deviceContext->VSSetShader(vertexShader, nullptr, 0);
    //deviceContext->PSSetShader(pixelShader, nullptr, 0);

    // Create linear sampler state for FXAA
    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    hr = device->CreateSamplerState(&sampDesc, &linearSamplerState);
    if (FAILED(hr))
    {
        log_error("Failed to create linear sampler state");
        return;
    }

    // Create FXAA render targets
    CreateFXAARenderTargets();

    log_info("shaders setup finished OK");
}

void DirectX::D3D11Renderer::LoadMesh(const MeshRef& mesh)
{
    if (mesh->data != nullptr) return;

    HRESULT hr = S_OK;

    //------------------------------------------------------------------------------------------------------------------
    // Create vertex buffer
    //------------------------------------------------------------------------------------------------------------------

    D3D11_BUFFER_DESC bufferDesc = {};
    ZeroMemory(&bufferDesc, sizeof(bufferDesc));
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof(SimpleVertex) * mesh->vertices.size();
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA initData = {};
    ZeroMemory(&initData, sizeof(initData));
    initData.pSysMem = mesh->vertices.data();

    ID3D11Buffer* vertexBuffer = nullptr;
    hr = device->CreateBuffer(&bufferDesc, &initData, &vertexBuffer);
    if (FAILED(hr))
    {
        log_error("Failed to create a vertex buffer for \"{}\" mesh", mesh->name);
        return;
    }

    log_info("vertex buffer OK");

    //------------------------------------------------------------------------------------------------------------------
    // Create index buffer
    //------------------------------------------------------------------------------------------------------------------

    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof(u16) * mesh->indices.size(); // 36 vertices needed for 12 triangles in a triangle list
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;

    initData.pSysMem = mesh->indices.data();

    ID3D11Buffer* indexBuffer = nullptr;
    hr = device->CreateBuffer(&bufferDesc, &initData, &indexBuffer);
    if (FAILED(hr))
    {
        log_error("Failed to create an index buffer for \"{}\" mesh", mesh->name);
        SAFE_RELEASE(vertexBuffer);
        return;
    }

    buffers.push_back(vertexBuffer);
    buffers.push_back(indexBuffer);

    log_info("index buffer OK");
    
    // NOTE: Will be freed during UnloadMesh call
    mesh->data = (void*)(new ID3D11BufferPair(vertexBuffer, indexBuffer));
}

void DirectX::D3D11Renderer::UnloadMesh(const MeshRef& mesh)
{
    //if (mesh->data == nullptr) return;
    //delete mesh->data;
}

