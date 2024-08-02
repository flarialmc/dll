#pragma once

#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include "../Module.hpp"
#include "../../../../SDK/SDK.hpp"
#include <Windows.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <wrl/client.h>

using namespace DirectX;

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

        if (SDK::currentScreen == "hud_screen" && !SwapchainHook::queue) {
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

                for (ID3D11ShaderResourceView* frame: previousFrames) {

                    if(SwapchainHook::d3d11Device) {
                        ImageWithOpacity(frame, {MC::windowSize.x, MC::windowSize.y}, alpha);
                    }
                    alpha *= module->settings.getSettingByName<float>("intensity")->value;
                }


            } else {

                for (ID3D11ShaderResourceView* bitmap: previousFrames) Memory::SafeRelease(bitmap);
                previousFrames.clear();
            }
        }

    void ImageWithOpacity(ID3D11ShaderResourceView* srv, ImVec2 size, float opacity)
    {
        if (opacity <= 0.0f)
            return;

        opacity = opacity > 1.0f ? 1.0f : opacity < 0.0f ? 0.0f : opacity;
        ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
        ImVec2 pos = {0, 0};
        ImU32 col = IM_COL32(255, 255, 255, static_cast<int>(opacity * 255));
        draw_list->AddImage((void*)srv, pos, ImVec2(pos.x + size.x, pos.y + size.y), ImVec2(0, 0), ImVec2(1, 1), col);
        ImGui::SetCursorScreenPos(ImVec2(pos.x + size.x, pos.y));
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


const char* vertexShaderSrc = R"(
cbuffer ConstantBuffer : register(b0)
{
    float4x4 projectionMatrix;
    float2 screenSize;
    float opacity;
}

struct VS_INPUT
{
    float3 pos : POSITION;
    float2 tex : TEXCOORD0;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    output.pos = mul(float4(input.pos, 1.0f), projectionMatrix);
    output.tex = input.tex;
    return output;
}
)";

const char* pixelShaderSrc = R"(
Texture2D shaderTexture : register(t0);
SamplerState sampleType : register(s0);

cbuffer ConstantBuffer : register(b0)
{
    float4x4 projectionMatrix;
    float2 screenSize;
    float opacity;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 color = shaderTexture.Sample(sampleType, input.tex);
    color.a *= opacity;
    return color;
}
)";

struct ConstantBuffer
{
    XMMATRIX projectionMatrix;
    XMFLOAT2 screenSize;
    float opacity;
    float padding; // Padding to align float4
};

    struct Vertex
    {
        XMFLOAT3 position;
        XMFLOAT2 texcoord;
    };

ID3D11VertexShader* gVertexShader = nullptr;
ID3D11PixelShader* gPixelShader = nullptr;
ID3D11InputLayout* gInputLayout = nullptr;
ID3D11Buffer* gConstantBuffer = nullptr;
ID3D11Buffer* gVertexBuffer = nullptr;
ID3D11SamplerState* gSamplerState = nullptr;
ID3D11RenderTargetView* gRTV = nullptr;
ID3D11DepthStencilView* gDSV = nullptr;


void InitializeRenderResources(ID3D11Device* device)
{
    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;

    // Compile shaders
    D3DCompile(vertexShaderSrc, strlen(vertexShaderSrc), nullptr, nullptr, nullptr, "main", "vs_5_0", 0, 0, &vsBlob, nullptr);
    D3DCompile(pixelShaderSrc, strlen(pixelShaderSrc), nullptr, nullptr, nullptr, "main", "ps_5_0", 0, 0, &psBlob, nullptr);


    // Create shaders
    device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &gVertexShader);
    device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &gPixelShader);

    // Define input layout
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    device->CreateInputLayout(layout, ARRAYSIZE(layout), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &gInputLayout);

    // Release blobs
    vsBlob->Release();
    psBlob->Release();

    // Create constant buffer
    D3D11_BUFFER_DESC cbd = {};
    cbd.Usage = D3D11_USAGE_DEFAULT;
    cbd.ByteWidth = sizeof(ConstantBuffer);
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    device->CreateBuffer(&cbd, nullptr, &gConstantBuffer);

    // Create sampler state
    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    device->CreateSamplerState(&sampDesc, &gSamplerState);

    Vertex vertices[] =
{
        { XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(-1.0f,  1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3( 1.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3( 1.0f,  1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) }
};

    // Create the vertex buffer
    D3D11_BUFFER_DESC vbd = {};
    vbd.Usage = D3D11_USAGE_DEFAULT;
    vbd.ByteWidth = sizeof(vertices);
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = vertices;

    device->CreateBuffer(&vbd, &initData, &gVertexBuffer);

    ID3D11Texture2D* backBuffer = nullptr;
    SwapchainHook::swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);

    // Create the render target view
    device->CreateRenderTargetView(backBuffer, nullptr, &gRTV);
    backBuffer->Release();

    // Create a depth stencil buffer and view
    D3D11_TEXTURE2D_DESC depthStencilDesc = {};
    depthStencilDesc.Width = MC::windowSize.x;  // Replace with actual width
    depthStencilDesc.Height = MC::windowSize.y;  // Replace with actual height
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags = 0;

    ID3D11Texture2D* depthStencilBuffer = nullptr;
    device->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencilBuffer);
    device->CreateDepthStencilView(depthStencilBuffer, nullptr, &gDSV);
    depthStencilBuffer->Release();

}

    void RenderSRV(ID3D11ShaderResourceView* srv, float width, float height, float opacity)
{

    ID3D11DeviceContext* context;
    SwapchainHook::d3d11Device->GetImmediateContext(&context);


    context->OMSetRenderTargets(1, &gRTV, gDSV);

    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = width;
    viewport.Height = height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    context->RSSetViewports(1, &viewport);

    ConstantBuffer cb;
    cb.screenSize = XMFLOAT2(width, height);
    cb.opacity = opacity;

    XMMATRIX orthoMatrix = XMMatrixOrthographicOffCenterLH(0.0f, width, height, 0.0f, 0.0f, 1.0f);
    cb.projectionMatrix = XMMatrixTranspose(orthoMatrix);

    context->UpdateSubresource(gConstantBuffer, 0, nullptr, &cb, 0, 0);

    context->VSSetShader(gVertexShader, nullptr, 0);
    context->PSSetShader(gPixelShader, nullptr, 0);
    context->IASetInputLayout(gInputLayout);

    context->PSSetShaderResources(0, 1, &srv);
    context->PSSetSamplers(0, 1, &gSamplerState);

    UINT stride = sizeof(float) * 5;
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &gVertexBuffer, &stride, &offset);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    context->VSSetConstantBuffers(0, 1, &gConstantBuffer);
    context->PSSetConstantBuffers(0, 1, &gConstantBuffer);

    context->Draw(4, 0);

    //context->OMSetRenderTargets(1, nullptr, nullptr);


    std::cout << "trolled" << std::endl;
 }
};