/*#include <d3d12.h>
#include <d3dcompiler.h>
#include <stdexcept>
#include <vector>
#include <d3dx12.h>
#include <d3d11.h>
#include "../../Engine.hpp"
#include "../../../../Hook/Hooks/Render/SwapchainHook.hpp"
#include "../../../../Module/Modules/MotionBlur/MotionBlurListener.hpp"

static const XMFLOAT4 quadVertices[] =
    {
        XMFLOAT4(1.0, -1.0, 0.0, 1.0),
        XMFLOAT4(-1.0, -1.0, 0.0, 1.0),
        XMFLOAT4(-1.0, 1.0, 0.0, 1.0),
        XMFLOAT4(-1.0, 1.0, 0.0, 1.0),
        XMFLOAT4(1.0, 1.0, 0.0, 1.0),
        XMFLOAT4(1.0, -1.0, 0.0, 1.0)};

const char *vertexShaderSrc = "struct VS_INPUT {\
    float4 pos : POSITION;\
};\
\
float4 main(VS_INPUT input) : SV_POSITION {\
    return input.pos;\
}";

const char *downsampleShaderSrc = R"(
cbuffer BlurInputBuffer : register(b0)
{
    float2 resolution;
    float2 offset;
    float2 halfPixel;
};

sampler sampler0 : register(s0);
Texture2D texture0 : register(t0);

float4 main(float4 screenSpace : SV_Position) : SV_TARGET
{
    float2 uv = screenSpace.xy / resolution;
    float4 colorSum = float4(0.0, 0.0, 0.0, 0.0);

    static const float2 offsets[9] = {
        float2(-1.0, -1.0) * halfPixel * offset,
        float2(0.0, -1.0) * halfPixel * offset,
        float2(1.0, -1.0) * halfPixel * offset,
        float2(-1.0, 0.0) * halfPixel * offset,
        float2(0.0, 0.0) * halfPixel * offset,
        float2(1.0, 0.0) * halfPixel * offset,
        float2(-1.0, 1.0) * halfPixel * offset,
        float2(0.0, 1.0) * halfPixel * offset,
        float2(1.0, 1.0) * halfPixel * offset
    };

    static const float weights[9] = {
        0.06136, 0.12245, 0.06136,
        0.12245, 0.24477, 0.12245,
        0.06136, 0.12245, 0.06136
    };

    float weightSum = 0.0;
    for (int i = 0; i < 9; i++)
    {
        weightSum += weights[i];
    }

    for (int i = 0; i < 9; i++)
    {
        colorSum += texture0.Sample(sampler0, uv + offsets[i]) * (weights[i] / weightSum);
    }

    return colorSum;
}
)";

const char *upsampleShaderSrc = R"(
cbuffer BlurInputBuffer : register(b0)
{
    float2 resolution;
    float2 offset;
    float2 halfPixel;
};

sampler sampler0 : register(s0);
Texture2D texture0 : register(t0);

float4 main(float4 screenSpace : SV_Position) : SV_TARGET
{
    float2 uv = screenSpace.xy / resolution;
    float4 colorSum = float4(0.0, 0.0, 0.0, 0.0);

    static const float2 offsets[9] = {
        float2(-1.0, -1.0) * halfPixel * offset,
        float2(0.0, -1.0) * halfPixel * offset,
        float2(1.0, -1.0) * halfPixel * offset,
        float2(-1.0, 0.0) * halfPixel * offset,
        float2(0.0, 0.0) * halfPixel * offset,
        float2(1.0, 0.0) * halfPixel * offset,
        float2(-1.0, 1.0) * halfPixel * offset,
        float2(0.0, 1.0) * halfPixel * offset,
        float2(1.0, 1.0) * halfPixel * offset
    };

    static const float weights[9] = {
        0.06136, 0.12245, 0.06136,
        0.12245, 0.24477, 0.12245,
        0.06136, 0.12245, 0.06136
    };

    float weightSum = 0.0;
    for (int i = 0; i < 9; i++)
    {
        weightSum += weights[i];
    }

    for (int i = 0; i < 9; i++)
    {
        colorSum += texture0.Sample(sampler0, uv + offsets[i]) * (weights[i] / weightSum);
    }

    return colorSum;
}
)";


ID3DBlob* TryCompileShader(const char* pSrcData, const char* pTarget)
{
    HRESULT hr;

    ID3DBlob* shaderBlob;
    ID3DBlob* errorBlob;
    hr = D3DCompile(pSrcData, strlen(pSrcData), nullptr, nullptr, nullptr, "main", pTarget, 0, 0, &shaderBlob, &errorBlob);

    if (FAILED(hr))
    {
        Logger::error("[Blur] Failed to compile shader");
        errorBlob->Release();
        throw std::logic_error("Failed to compile shader!");
    }
    return shaderBlob;
}

ID3D12PipelineState* dbgShader;

void Blur::InitializePipeline()
{
    HRESULT hr;
    ID3D12Device* pDevice = SwapchainHook::d3d12Device5;
    ID3D12GraphicsCommandList* pCommandList = SwapchainHook::d3d12CommandList;

    // Create constant buffer
    D3D12_HEAP_PROPERTIES heapProps = {};
    heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
    heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

    D3D12_RESOURCE_DESC resourceDesc = {};
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resourceDesc.Width = sizeof(BlurInputBuffer);
    resourceDesc.Height = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    pDevice->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&pConstantBuffer));

    // Create vertex buffer
    resourceDesc.Width = sizeof(quadVertices);
    pDevice->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
        nullptr,
        IID_PPV_ARGS(&pVertexBuffer));

    // Compile shaders
    ID3DBlob* vertexShaderBlob = TryCompileShader(vertexShaderSrc, "vs_5_0");
    ID3DBlob* pixelShaderBlob = TryCompileShader(upsampleShaderSrc, "ps_5_0");

    // Create root signature
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
    rootSignatureDesc.NumParameters = 0;
    rootSignatureDesc.pParameters = nullptr;
    rootSignatureDesc.NumStaticSamplers = 0;
    rootSignatureDesc.pStaticSamplers = nullptr;
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    ID3DBlob* signature;
    ID3DBlob* error;
    D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
    pDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&pRootSignature));

    // Create pipeline state object
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
    psoDesc.pRootSignature = pRootSignature;
    psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShaderBlob);
    psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShaderBlob);
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState.DepthEnable = FALSE;
    psoDesc.DepthStencilState.StencilEnable = FALSE;
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.SampleDesc.Count = 1;

    pDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pUpsampleShader));
}*/