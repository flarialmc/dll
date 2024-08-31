#pragma once
#include <d3d11.h>
#include <opencv2/opencv.hpp>

class ImageProcessor {
public:
    static cv::Mat ApplyGaussianBlur(const cv::Mat& src, double sigma) {
        cv::Mat dst;
        cv::GaussianBlur(src, dst, cv::Size(0, 0), sigma);
        return dst;
    }
    static ID3D11Texture2D* LoadTextureFromMat(cv::Mat& mat, ID3D11Device* device, ID3D11DeviceContext* context) {

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
};
