#pragma once
#include <Utils/Utils.hpp>

namespace cosmetic {
	struct TextureData {
		unsigned int mWidth;
		unsigned int mHeight;

		std::string mPath;

		TextureData() : mWidth(0), mHeight(0) {}
		TextureData(unsigned int width, unsigned int height, const std::string& path)
			: mWidth(width), mHeight(height), mPath(path) {}


		bool operator==(const TextureData& other) const { return this->mWidth == other.mWidth && this->mHeight == other.mHeight && this->mPath == other.mPath; }
		bool operator!=(const TextureData& other) const { return this->mWidth == other.mWidth && this->mHeight == other.mHeight && this->mPath == other.mPath; }
	};


	struct TextureFaceUV {
		Vec2<float> uv;
		Vec2<float> uvSize;
		Vec4<float> normUv;
	};

	struct TexturePerFaceUV {
		union {
			struct {
				TextureFaceUV north;
				TextureFaceUV east;
				TextureFaceUV south;
				TextureFaceUV west;
				TextureFaceUV up;
				TextureFaceUV down;
			};
			TextureFaceUV faces[6];
		};
		TexturePerFaceUV() {
			for (size_t i = 0; i < _countof(faces); i++)
				new (&faces[i]) TextureFaceUV();
		}
		TexturePerFaceUV(const TexturePerFaceUV& other) {
			for (size_t i = 0; i < _countof(faces); i++)
				new (&faces[i]) TextureFaceUV(other.faces[i]);
			uvPos = other.uvPos;
		}
		TexturePerFaceUV(TexturePerFaceUV&& other) {
			for (size_t i = 0; i < _countof(faces); i++)
				new (&faces[i]) TextureFaceUV(std::move(other.faces[i]));
			uvPos = other.uvPos;
		}
		void operator=(const TexturePerFaceUV& other) {
			for (size_t i = 0; i < _countof(faces); i++)
				faces[i] = other.faces[i];
			uvPos = other.uvPos;
		}
		void operator=(TexturePerFaceUV&& other) {
			for (size_t i = 0; i < _countof(faces); i++)
				faces[i] = std::move(other.faces[i]);
			uvPos = other.uvPos;
		}

		Vec2<float> uvPos;
	};

	struct TextureUV {
		bool isPerFaceUV;

		TexturePerFaceUV faces;
	};
}