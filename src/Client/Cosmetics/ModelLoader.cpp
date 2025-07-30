#include <Utils/Utils.hpp>
#include "ModelLoader.hpp"
#include "JsonModel.hpp"

namespace cosmetic {

	JsonGeometryDescription::JsonGeometryDescription(const json& json) {
		this->identifier = json.at("identifier");
		this->textureWidth = json.at("texture_width");
		this->textureHeight = json.at("texture_height");
		if (json.contains("visible_bounds_offset")) {
			std::vector<float> boundsOffset = json["visible_bounds_offset"];
			this->visibleBoundsOffset = Vec3(boundsOffset.at(0), boundsOffset.at(1), boundsOffset.at(2));
		}
		if (json.contains("visible_bounds_width")) {
			this->visibleBoundsWidth = json["visible_bounds_width"];
		}
		if (json.contains("visible_bounds_height")) {
			this->visibleBoundsHeight = json["visible_bounds_height"];
		}
	}

	std::vector<std::unique_ptr<JsonModel>> ModelLoader::LoadFromFile(const std::filesystem::path& filepath) {
		std::ifstream file(filepath);
		if (file.fail() || !file.is_open()) {
			Logger::error("Couldn't load Geometry Model, File doesn't exist or isnt open");
		}
		try {
			auto m = LoadFromStream(file);
			file.close();
			return m;
		}
		catch (const std::exception& e) {
			file.close();
			Logger::error("Couldn't load Geometry Model {}", e.what());
		}
	}

	std::vector<std::unique_ptr<JsonModel>> ModelLoader::LoadFromStream(std::ifstream& in) {
		return LoadFromJson(json::parse(in, nullptr, true, true));
	}

	std::vector<std::unique_ptr<JsonModel>> ModelLoader::LoadFromString(const std::string& str) {
		return LoadFromJson(json::parse(str, nullptr, true, true));
	}

	std::vector<std::unique_ptr<JsonModel>> ModelLoader::LoadFromJson(const json& json) {
		std::vector<std::unique_ptr<JsonModel>> models;
		if (json.at("format_version").get<std::string>() != "1.12.0")
			throw ModelLoaderException("Unsupported format version");

		for (auto& objects : json.at("minecraft:geometry")) {
			std::unique_ptr<JsonModel> model = std::make_unique<JsonModel>();
			model->description = JsonGeometryDescription(objects.at("description"));

			for (auto& boneVal : objects.at("bones")) {
				JsonBone bone{};
				bone.name = boneVal["name"];

				if (boneVal.contains("parent")) {
					bone.parent = boneVal["parent"];
				}

				if (boneVal.contains("pivot")) {
					std::vector<float> pivot = boneVal["pivot"];
					bone.pivot = Vec3(pivot[0], pivot[1], pivot[2]);
				}

				if (boneVal.contains("rotation")) {
					std::vector<float> rotation = boneVal["rotation"];
					bone.rotation = Vec3(rotation[0], rotation[1], rotation[2]);
					bone.rrotation = Vec3(glm::radians(-bone.rotation.x), glm::radians(-bone.rotation.y), glm::radians(-bone.rotation.z));
				}

				if (!boneVal.contains("cubes")) {
					model->bones.push_back(bone);
					continue;
				}

				for (auto& cubeVal : boneVal.at("cubes")) {
					JsonCube cube{};

					if (cubeVal.contains("origin")) {
						std::vector<float> origin = cubeVal["origin"];
						cube.origin = Vec3(origin.at(0), origin.at(1), origin.at(2));
					}

					if (cubeVal.contains("size")) {
						std::vector<float> size = cubeVal["size"];
						cube.size = Vec3(size.at(0), size.at(1), size.at(2));
					}

					if (cubeVal.contains("pivot")) {
						std::vector<float> pivot = cubeVal["pivot"];
						cube.pivot = Vec3(pivot.at(0), pivot.at(1), pivot.at(2));
					}

					if (cubeVal.contains("rotation")) {
						std::vector<float> rotation = cubeVal["rotation"];
						cube.rotation = Vec3(rotation.at(0), rotation.at(1), rotation.at(2));
						cube.rrotation = Vec3(glm::radians(-cube.rotation.x), glm::radians(-cube.rotation.y), glm::radians(-cube.rotation.z));
					}

					if (cubeVal.contains("inflate")) {
						cube.inflate = cubeVal["inflate"];
					}

					TextureUV uv{};
					Vec2 textureSize((float)model->description.textureWidth, (float)model->description.textureHeight);
					auto normUv = [&](Vec4<float> r) -> Vec4<float>&{
						r.x /= textureSize.x;
						r.y /= textureSize.y;
						r.z /= textureSize.x;
						r.w /= textureSize.y;
						return r;
						};

					auto computeNormUv = [&](float u, float v, float w, float h) -> Vec4<float> {
						float u0 = u;
						float v0 = v;
						float u1 = u + w;
						float v1 = v + h;
						// Normalize all coordinates
						u0 /= textureSize.x;
						u1 /= textureSize.x;
						v0 /= textureSize.y;
						v1 /= textureSize.y;
						// Ensure (u_min, v_min) is top-left and (u_max, v_max) is bottom-right
						float u_min = std::min(u0, u1);
						float u_max = std::max(u0, u1);
						float v_min = std::min(v0, v1);
						float v_max = std::max(v0, v1);
						return Vec4<float>(u_min, v_min, u_max, v_max);
					};

					if (cubeVal.at("uv").is_array()) {
						uv.isPerFaceUV = false;
						std::vector<float> uvArray = cubeVal["uv"];
						Vec2 uvPos = Vec2(uvArray[0], uvArray[1]);
						uv.faces.uvPos = uvPos;

						uv.faces.up.normUv = normUv(Vec4<float>(uvPos.x + cube.size.z, uvPos.y, cube.size.x, cube.size.z));
						uv.faces.down.normUv = normUv(Vec4<float>(uvPos.x + cube.size.z + cube.size.x, uvPos.y, cube.size.x, cube.size.z));
						uv.faces.north.normUv = normUv(Vec4<float>(uvPos.x + cube.size.z, uvPos.y + cube.size.z, cube.size.x, cube.size.y));
						uv.faces.south.normUv = normUv(Vec4<float>(uvPos.x + cube.size.z * 2.f + cube.size.x, uvPos.y + cube.size.z, cube.size.x, cube.size.y));
						uv.faces.west.normUv = normUv(Vec4<float>(uvPos.x, uvPos.y + cube.size.z, cube.size.z, cube.size.y));
						uv.faces.east.normUv = normUv(Vec4<float>(uvPos.x + cube.size.z + cube.size.x, uvPos.y + cube.size.z, cube.size.z, cube.size.y));
					}
					else {
						uv.isPerFaceUV = true;
						auto uvMap = cubeVal["uv"];

						auto loadUv = [&](TextureFaceUV& out, const char* name) {
							std::vector<float> uvArray = uvMap[name]["uv"];
							std::vector<float> uvSizeArray = uvMap[name]["uv_size"];
							out.uv = Vec2(uvArray[0], uvArray[1]);
							out.uvSize = Vec2(uvSizeArray[0], uvSizeArray[1]);
							out.normUv = computeNormUv(out.uv.x, out.uv.y, out.uvSize.x, out.uvSize.y);
							// Optionally keep the original uv/uvSize for mesh creation logic
						};

						loadUv(uv.faces.north, "north");
						loadUv(uv.faces.east, "east");
						loadUv(uv.faces.south, "south");
						loadUv(uv.faces.west, "west");
						loadUv(uv.faces.up, "up");
						loadUv(uv.faces.down, "down");
					}

					cube.uv = uv;
					bone.cubes.push_back(cube);
				}

				model->bones.push_back(bone);
			}


			//parenting
			model->mQuadVerticeCount = 0;
			for (auto& bone : model->bones) {
				model->mQuadVerticeCount += (int)(bone.cubes.size() * 36);
				if (bone.parent.empty()) {
					model->rootBones.push_back(&bone);
					continue;
				}
				JsonBone* parent = model->getBone(bone.parent);
				if (parent) {
					bone.parentBone = parent;
					parent->childBones.push_back(&bone);
				}
			}
			model->ComputeAABB();
			models.push_back(std::move(model));

			//Logger::debug("Loaded Geometry: {}", model->description.identifier);
		}
		return models;
	}

	static std::unique_ptr<JsonModel> findModel(std::vector<std::unique_ptr<JsonModel>>& models, const std::string& string) {
		for (auto& model : models) {
			if (model->description.identifier == string) {
				return std::move(model);
			}
		}
		return {};
	}

	std::unique_ptr<JsonModel> ModelLoader::LoadFromFile(const std::filesystem::path& filepath, const std::string& geometryName) {
		auto v = LoadFromFile(filepath);
		return findModel(v, geometryName);
	}

	std::unique_ptr<JsonModel> ModelLoader::LoadFromStream(std::ifstream& in, const std::string& geometryName) {
		auto v = LoadFromStream(in);
		return findModel(v, geometryName);
	}

	std::unique_ptr<JsonModel> ModelLoader::LoadFromString(const std::string& str, const std::string& geometryName) {
		auto v = LoadFromString(str);
		return findModel(v, geometryName);
	}

	std::unique_ptr<JsonModel> ModelLoader::LoadFromJson(const json& json, const std::string& geometryName) {
		auto v = LoadFromJson(json);
		return findModel(v, geometryName);
	}
}