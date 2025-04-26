#pragma once

#include "Texture.hpp"
#include <vector>
#include <Utils/Utils.hpp>
#include "../../SDK/SDK.hpp"
#include "../../Config/Settings.hpp"

namespace cosmetic {
	struct JsonCube {
		Vec3<float> origin = Vec3<float>();
		Vec3<float> size = Vec3<float>();
		Vec3<float> pivot = Vec3<float>();
		Vec3<float> rotation = Vec3<float>();
		Vec3<float> rrotation = Vec3<float>();
		float inflate;
		TextureUV uv;
	};

	struct JsonBone {
		std::string name;
		std::string parent;
		JsonBone* parentBone;
		std::vector<JsonBone*> childBones;
		Vec3<float> pivot = Vec3<float>();
		Vec3<float> rotation = Vec3<float>();
		Vec3<float> rrotation = Vec3<float>();
		std::vector<JsonCube> cubes;
	};

	struct JsonGeometryDescription {
		std::string identifier;
		int textureWidth;
		int textureHeight;
		int visibleBoundsWidth;
		int visibleBoundsHeight;
		Vec3<float> visibleBoundsOffset = Vec3<float>();
		JsonGeometryDescription() = default;
		JsonGeometryDescription(const json& json);
	};

	struct JsonModel {
		JsonGeometryDescription description;
		std::vector<JsonBone> bones;
		std::vector<JsonBone*> rootBones;
		AABB aabb;


		JsonBone* getBone(const std::string& name);
		void render(ScreenContext* ctx, BedrockTextureData tex, const Vec3<float>& pos, const Vec3<float>& rot, const Vec3<float>& scl, bool drawInnerFaces);

	private:
		void ComputeAABB();

	private:
		int mQuadVerticeCount = 0;

		friend class ModelLoader;
	};
}