#pragma once
#include <vector>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <opencv2/core/hal/interface.h>
#include <opencv2/opencv.hpp>
#include "../../Engine.hpp"
#include "../../../../Hook/Hooks/Render/SwapchainHook.hpp"
#include "../../../../Module/Modules/MotionBlur/MotionBlurListener.hpp"



void Blur::RenderBlur(ID3D11Texture2D* inputTexture, ID3D11Device* device, ID3D11DeviceContext* deviceContext) {
    ID3D11ShaderResourceView* inputSRV = nullptr;
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;
    device->CreateShaderResourceView(inputTexture, &srvDesc, &inputSRV);

    D3D11_TEXTURE2D_DESC inputDesc;
    inputTexture->GetDesc(&inputDesc);

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    deviceContext->Map(inputTexture, 0, D3D11_MAP_READ, 0, &mappedResource);

    cv::Mat src(inputDesc.Height, inputDesc.Width, CV_8UC4, mappedResource.pData, mappedResource.RowPitch);
    deviceContext->Unmap(inputTexture, 0);

    cv::Mat dst = ImageProcessor::ApplyGaussianBlur(src, 15);

    ID3D11Texture2D* texture_blur = ImageProcessor::LoadTextureFromMat(dst, device, deviceContext);

    ID3D11ShaderResourceView* texture_srv = nullptr;
    device->CreateShaderResourceView(texture_blur, &srvDesc, &texture_srv);

    ImGui::Image(texture_srv, ImVec2(dst.cols, dst.rows));

    if (inputSRV) inputSRV->Release();
    if (texture_srv) texture_srv->Release();
    if (texture_blur) texture_blur->Release();
}

cv::Mat ImageProcessor::ApplyGaussianBlur(const cv::Mat& src, double sigma) {
    cv::Mat dst;
    GaussianBlur(src, dst, cv::Size(0, 0), sigma);
    return dst;
}

ID3D11Texture2D* ImageProcessor::LoadTextureFromMat(cv::Mat& mat, ID3D11Device* device, ID3D11DeviceContext* context) {
    if (mat.empty() || device == nullptr || context == nullptr) return nullptr;

    cv::Mat matBGRA;
    if (mat.channels() == 3) {
        cvtColor(mat, matBGRA, cv::COLOR_BGR2BGRA);
    }
    else if (mat.channels() == 4) {
        matBGRA = mat;
    }
    else {
        return nullptr;
    }

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = matBGRA.cols;
    desc.Height = matBGRA.rows;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;

    ID3D11Texture2D* texture = nullptr;
    HRESULT hr = device->CreateTexture2D(&desc, nullptr, &texture);
    if (FAILED(hr)) return nullptr;

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    hr = context->Map(texture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(hr)) return nullptr;

    for (int y = 0; y < matBGRA.rows; ++y) {
        memcpy(static_cast<BYTE*>(mappedResource.pData) + y * mappedResource.RowPitch, matBGRA.ptr<uchar>(y), matBGRA.cols * 4);
    }

    context->Unmap(texture, 0);

    return texture;
}