#pragma once

#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include "../Module.hpp"
#include "../../../../SDK/SDK.hpp"
#include <Windows.h>
#include <d3dcompiler.h>
#include <wrl/client.h>

class MotionBlurListener : public Listener {

    Module *module;

public:
    explicit MotionBlurListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;

    }

    static inline std::vector<ID3D11ShaderResourceView*> previousFrames;


    void onRender(RenderEvent &event) override {
        int maxFrames = (int)round(module->settings.getSettingByName<float>("intensity2")->value);

        if (true) {
            if (previousFrames.size() >= static_cast<int>(maxFrames)) {
                // Remove excess frames if maxFrames is reduced
                int framesToRemove = (int)previousFrames.size() - static_cast<int>(maxFrames);
                for (int i = 0; i < framesToRemove; ++i) {
                    if(previousFrames[i]) previousFrames[i]->Release();
                }
                previousFrames.erase(previousFrames.begin(), previousFrames.begin() + framesToRemove);
            }

            previousFrames.push_back(SaveBackbuffer());


            float alpha = 0.3f;

            if(!SwapchainHook::queue) {
                for (ID3D11ShaderResourceView* frame: previousFrames) {

                    if(SwapchainHook::d3d11Device) {
                        if(!vertexBuffer) InitializeFullscreenQuad(SwapchainHook::d3d11Device);
                        RenderFullscreenQuad(frame, alpha);
                    }
                    alpha *= module->settings.getSettingByName<float>("intensity")->value;
                }
            } else {
                // do later
            }

            } else {

                for (ID3D11ShaderResourceView* bitmap: previousFrames) Memory::SafeRelease(bitmap);
                previousFrames.clear();
            }
        }


    ID3D11ShaderResourceView* SaveBackbuffer()
    {

        ID3D11DeviceContext* deviceContext;
        SwapchainHook::d3d11Device->GetImmediateContext(&deviceContext);
        IDXGISurface1* backBuffer = nullptr;
        HRESULT hr;
        SwapchainHook::swapchain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));

        ID3D11Texture2D* buffer2D = nullptr;
        if(FAILED(backBuffer->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&buffer2D)))) std::cout << "failed to get 2d" << std::endl;

        D3D11_TEXTURE2D_DESC desc;
        buffer2D->GetDesc(&desc);

        ID3D11Texture2D* stageTex = nullptr;
        D3D11_TEXTURE2D_DESC stageDesc = desc;
        stageDesc.Usage = D3D11_USAGE_STAGING;
        stageDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        stageDesc.BindFlags = 0;

        //desc.Usage = DXGI_USAGE_SHADER_INPUT;
        HRESULT r = SwapchainHook::d3d11Device->CreateTexture2D(&stageDesc, nullptr, &stageTex);
        deviceContext->CopyResource(stageTex, buffer2D);

        if (FAILED(r))  std::cout << "Failed to create stage texture: " << std::hex << r << std::endl;


        D3D11_TEXTURE2D_DESC defaultDesc = desc;
        defaultDesc.Usage = D3D11_USAGE_DEFAULT;
        defaultDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        defaultDesc.CPUAccessFlags = 0;

        ID3D11Texture2D* defaultTexture = nullptr;
        hr = SwapchainHook::d3d11Device->CreateTexture2D(&defaultDesc, nullptr, &defaultTexture);
        if (FAILED(hr)) {
            std::cout << "Failed to create def texture: " << std::hex << r << std::endl;
        }

        deviceContext->CopyResource(defaultTexture, stageTex);

        stageTex->Release();

        ID3D11ShaderResourceView* outSRV;
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        srvDesc.Format = desc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = desc.MipLevels;
        srvDesc.Texture2D.MostDetailedMip = 0;

        if (FAILED(hr = SwapchainHook::d3d11Device->CreateShaderResourceView(defaultTexture, &srvDesc, &outSRV)))
        {
            std::cout << "Failed to create shader resource view: " << std::hex << hr << std::endl;
        }

        //if(outSRV) std::cout << "Wroekd" << std::endl;

        backBuffer->Release();
        buffer2D->Release();
        defaultTexture->Release();

        return outSRV;
    }

    struct Vertex {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT2 texcoord;
    };

    const char* vertexShaderSrc = R"(
cbuffer ConstantBuffer : register(b0)
{
    float opacity;
};
struct VS_INPUT
{
    float3 pos : POSITION;
    float2 tex : TEXCOORD;
};
struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};
PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    output.pos = float4(input.pos, 1.0f);
    output.tex = input.tex;
    return output;
}
)";

    const char* pixelShaderSrc = R"(
cbuffer ConstantBuffer : register(b0)
{
    float opacity;
};
Texture2D shaderTexture : register(t0);
SamplerState samplerState : register(s0);
struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};
float4 main(PS_INPUT input) : SV_Target
{
    return shaderTexture.Sample(samplerState, input.tex) * opacity;
}
)";

    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;
    ID3D11InputLayout* inputLayout;
    ID3D11Buffer* vertexBuffer;
    ID3D11Buffer* constantBuffer;
    ID3D11SamplerState* samplerState;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView = nullptr;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView = nullptr;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer = nullptr;

    struct CONSTANT_BUFFER_DATA {
        float opacity;
    };


    void InitializeFullscreenQuad(ID3D11Device* device) {
        ID3DBlob* vsBlob;
        ID3DBlob* psBlob;
        D3DCompile(vertexShaderSrc, strlen(vertexShaderSrc), nullptr, nullptr, nullptr, "main", "vs_5_0", 0, 0, &vsBlob, nullptr);
        D3DCompile(pixelShaderSrc, strlen(pixelShaderSrc), nullptr, nullptr, nullptr, "main", "ps_5_0", 0, 0, &psBlob, nullptr);
        device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vertexShader);
        device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pixelShader);
        D3D11_INPUT_ELEMENT_DESC layout[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        HRESULT hr = device->CreateInputLayout(layout, ARRAYSIZE(layout), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout);
        if(FAILED(hr)) std::cout << "Failed to create input Layout: " << std::hex << hr << std::endl;
        vsBlob->Release();
        psBlob->Release();
        Vertex vertices[] = {
            { { -1.0f, -1.0f, 0.0f }, { 0.0f, 1.0f } },
            { { -1.0f,  1.0f, 0.0f }, { 0.0f, 0.0f } },
            { {  1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f } },
            { {  1.0f,  1.0f, 0.0f }, { 1.0f, 0.0f } },
        };
        D3D11_BUFFER_DESC bufferDesc = { sizeof(vertices), D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0, 0, 0 };
        D3D11_SUBRESOURCE_DATA initData = { vertices, 0, 0 };
        hr = device->CreateBuffer(&bufferDesc, &initData, &vertexBuffer);
        if(FAILED(hr)) std::cout << "Failed to create vertex buffer: " << std::hex << hr << std::endl;
        D3D11_BUFFER_DESC cbDesc;
        cbDesc.ByteWidth = sizeof( CONSTANT_BUFFER_DATA );
        cbDesc.Usage = D3D11_USAGE_DEFAULT;
        cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        cbDesc.MiscFlags = 0;
        cbDesc.StructureByteStride = 0;
        hr = device->CreateBuffer(&cbDesc, nullptr, &constantBuffer);
        if(FAILED(hr)) std::cout << "Failed to create constant buffer: " << std::hex << hr << std::endl;
        D3D11_SAMPLER_DESC sampDesc = {};
        sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        device->CreateSamplerState(&sampDesc, &samplerState);
    }


void RenderFullscreenQuad(ID3D11ShaderResourceView* srv, float opacity) {

    ID3D11DeviceContext* context = nullptr;
    SwapchainHook::d3d11Device->GetImmediateContext(&context);

    UINT stride = sizeof(Vertex);
    UINT offset = 0;


    if (!SwapchainHook::d3d11Device || !context || !vertexBuffer || !constantBuffer || !srv || !SwapchainHook::init || !inputLayout) {
        std::cout << "One or more required resources are null." << std::endl;
        if (context) {
            context->Release();
        }
        return;
    }

    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer = nullptr;
    HRESULT hr = SwapchainHook::swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
    if (FAILED(hr)) {
        std::cout << "Failed to get the swap chain buffer. HRESULT: " << std::hex << hr << std::endl;
        if (context) {
            context->Release();
        }
        return;
    }

    hr = SwapchainHook::d3d11Device->CreateRenderTargetView(backBuffer.Get(), nullptr, &renderTargetView);
    if (FAILED(hr)) {
        std::cout << "Failed to create render target view. HRESULT: " << std::hex << hr << std::endl;
        if (context) {
            context->Release();
        }
        return;
    }

    if (!depthStencilBuffer) {
        D3D11_TEXTURE2D_DESC depthStencilDesc = {};
        depthStencilDesc.Width = MC::windowSize.x;
        depthStencilDesc.Height = MC::windowSize.y;
        depthStencilDesc.MipLevels = 1;
        depthStencilDesc.ArraySize = 1;
        depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthStencilDesc.SampleDesc.Count = 1;
        depthStencilDesc.SampleDesc.Quality = 0;
        depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
        depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        depthStencilDesc.CPUAccessFlags = 0;
        depthStencilDesc.MiscFlags = 0;

        hr = SwapchainHook::d3d11Device->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencilBuffer);
        if (FAILED(hr)) {
            std::cout << "Failed to create depth stencil buffer. HRESULT: " << std::hex << hr << std::endl;
            if (context) {
                context->Release();
            }
            return;
        }

        hr = SwapchainHook::d3d11Device->CreateDepthStencilView(depthStencilBuffer.Get(), nullptr, &depthStencilView);
        if (FAILED(hr)) {
            std::cout << "Failed to create depth stencil view. HRESULT: " << std::hex << hr << std::endl;
            if (context) {
                context->Release();
            }
            return;
        }
    }

    context->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

    D3D11_VIEWPORT viewport = {};
    viewport.Width = MC::windowSize.x;
    viewport.Height = MC::windowSize.y;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    context->RSSetViewports(1, &viewport);

    CONSTANT_BUFFER_DATA data{opacity};
    context->UpdateSubresource(constantBuffer, 0, nullptr, &data, 0, 0);
    context->VSSetConstantBuffers(0, 1, &constantBuffer);
    context->PSSetConstantBuffers(0, 1, &constantBuffer);

    context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    context->VSSetShader(vertexShader, nullptr, 0);
    context->PSSetShader(pixelShader, nullptr, 0);
    context->PSSetShaderResources(0, 1, &srv);
    context->PSSetSamplers(0, 1, &samplerState);
    context->IASetInputLayout(inputLayout);



    context->Draw(4, 0);


    if (context) {
        context->Release();
    }
}


};
