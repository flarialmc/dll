#include "JsonModel.hpp"
#include "../../SDK/Client/Render/ScreenContext.hpp"
#include "../../SDK/SDK.hpp"
#include "../../SDK/Client/Render/Tessellator/MeshHelpers.hpp"
#include <Utils/Render/MaterialUtils.hpp>
#include "glm/glm/ext/matrix_transform.hpp"

namespace cosmetic {

	JsonBone* JsonModel::getBone(const std::string& name) {
		for (auto& bone : this->bones) {
			if (bone.name == name)
				return &bone;
		}
		return nullptr;
	}



	static glm::mat4x4 getNewTransform(glm::mat4x4& parentMatrix, glm::vec3 pivot, glm::vec3 rotation) {
		pivot.x = -pivot.x;
		// Translate to the pivot point
		glm::mat4x4 translationMatrix = glm::translate(glm::mat4x4(1.0f), pivot);

		// Rotate around the pivot point
		glm::mat4x4 rotationMatrix(1.f);
		rotationMatrix = glm::rotate(rotationMatrix, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		rotationMatrix = glm::rotate(rotationMatrix, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		rotationMatrix = glm::rotate(rotationMatrix, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));

		// Translate back to the original position
		glm::mat4x4 inverseTranslationMatrix = glm::translate(glm::mat4x4(1.0f), -pivot);

		// Combine the transformations
		glm::mat4x4 newTransform = parentMatrix * translationMatrix * rotationMatrix * inverseTranslationMatrix;

		return newTransform;
	}

	static void renderTexturedCube(JsonCube& cube, bool renderInsideFaces) {
		Vec3<float> pos = cube.origin.mul(Vec3<float>(-1.f, 1.f, 1.f));
		Vec3<float> size = cube.size.mul(Vec3(-1.f, 1.f, 1.f));
		Tessellator* t = SDK::scn->getTessellator();

		pos.x -= -cube.Inflate;
		pos.y -= cube.Inflate;
		pos.z -= cube.Inflate;
		size.x += -cube.Inflate * 2.f;
		size.y += cube.Inflate * 2.f;
		size.z += cube.Inflate * 2.f;


		glm::mat4x4 prevMatrix = t->transformMatrix;
		t->applyTransform = true;
		t->transformMatrix = t->transformMatrix = getNewTransform(t->transformMatrix, glm::vec3(cube.pivot.x, cube.pivot.y, cube.pivot.z), glm::vec3(cube.rrotation.x, cube.rrotation.y, cube.rrotation.z));

		// bottom
		const Vec4<float>* normUv = &cube.uv.faces.down.normUv;
		t->vertexUV(pos.x + size.x, pos.y, pos.z + size.z, normUv->z, normUv->y);
		t->vertexUV(pos.x + size.x, pos.y, pos.z, normUv->z, normUv->w);
		t->vertexUV(pos.x, pos.y, pos.z, normUv->x, normUv->w);
		t->vertexUV(pos.x, pos.y, pos.z + size.z, normUv->x, normUv->y);


		// top 
		normUv = &cube.uv.faces.up.normUv;
		t->vertexUV(pos.x, pos.y + size.y, pos.z + size.z, normUv->x, normUv->y);
		t->vertexUV(pos.x, pos.y + size.y, pos.z, normUv->x, normUv->w);
		t->vertexUV(pos.x + size.x, pos.y + size.y, pos.z, normUv->z, normUv->w);
		t->vertexUV(pos.x + size.x, pos.y + size.y, pos.z + size.z, normUv->z, normUv->y);


		// back (north)
		normUv = &cube.uv.faces.north.normUv;
		t->vertexUV(pos.x + size.x, pos.y, pos.z, normUv->z, normUv->w);
		t->vertexUV(pos.x + size.x, pos.y + size.y, pos.z, normUv->z, normUv->y);
		t->vertexUV(pos.x, pos.y + size.y, pos.z, normUv->x, normUv->y);
		t->vertexUV(pos.x, pos.y, pos.z, normUv->x, normUv->w);


		// back south
		normUv = &cube.uv.faces.south.normUv;
		t->vertexUV(pos.x, pos.y + size.y, pos.z + size.z, normUv->z, normUv->y);
		t->vertexUV(pos.x + size.x, pos.y + size.y, pos.z + size.z, normUv->x, normUv->y);
		t->vertexUV(pos.x + size.x, pos.y, pos.z + size.z, normUv->x, normUv->w);
		t->vertexUV(pos.x, pos.y, pos.z + size.z, normUv->z, normUv->w);


		// right (east)
		normUv = &cube.uv.faces.east.normUv;
		t->vertexUV(pos.x, pos.y, pos.z + size.z, normUv->x, normUv->w);
		t->vertexUV(pos.x, pos.y, pos.z, normUv->z, normUv->w);
		t->vertexUV(pos.x, pos.y + size.y, pos.z, normUv->z, normUv->y);
		t->vertexUV(pos.x, pos.y + size.y, pos.z + size.z, normUv->x, normUv->y);

		// left (west)
		normUv = &cube.uv.faces.west.normUv;
		t->vertexUV(pos.x + size.x, pos.y + size.y, pos.z + size.z, normUv->z, normUv->y);
		t->vertexUV(pos.x + size.x, pos.y + size.y, pos.z, normUv->x, normUv->y);
		t->vertexUV(pos.x + size.x, pos.y, pos.z, normUv->x, normUv->w);
		t->vertexUV(pos.x + size.x, pos.y, pos.z + size.z, normUv->z, normUv->w);



		if (renderInsideFaces) {
			// bottom
			normUv = &cube.uv.faces.down.normUv;
			t->vertexUV(pos.x, pos.y, pos.z + size.z, normUv->x, normUv->y);
			t->vertexUV(pos.x, pos.y, pos.z, normUv->x, normUv->w);
			t->vertexUV(pos.x + size.x, pos.y, pos.z, normUv->z, normUv->w);
			t->vertexUV(pos.x + size.x, pos.y, pos.z + size.z, normUv->z, normUv->y);


			// top 
			normUv = &cube.uv.faces.up.normUv;
			t->vertexUV(pos.x + size.x, pos.y + size.y, pos.z + size.z, normUv->z, normUv->y);
			t->vertexUV(pos.x + size.x, pos.y + size.y, pos.z, normUv->z, normUv->w);
			t->vertexUV(pos.x, pos.y + size.y, pos.z, normUv->x, normUv->w);
			t->vertexUV(pos.x, pos.y + size.y, pos.z + size.z, normUv->x, normUv->y);


			// back (north)
			normUv = &cube.uv.faces.north.normUv;
			t->vertexUV(pos.x, pos.y, pos.z, normUv->x, normUv->w);
			t->vertexUV(pos.x, pos.y + size.y, pos.z, normUv->x, normUv->y);
			t->vertexUV(pos.x + size.x, pos.y + size.y, pos.z, normUv->z, normUv->y);
			t->vertexUV(pos.x + size.x, pos.y, pos.z, normUv->z, normUv->w);


			// back south
			normUv = &cube.uv.faces.south.normUv;
			t->vertexUV(pos.x, pos.y, pos.z + size.z, normUv->z, normUv->w);
			t->vertexUV(pos.x + size.x, pos.y, pos.z + size.z, normUv->x, normUv->w);
			t->vertexUV(pos.x + size.x, pos.y + size.y, pos.z + size.z, normUv->x, normUv->y);
			t->vertexUV(pos.x, pos.y + size.y, pos.z + size.z, normUv->z, normUv->y);


			// right (west)
			normUv = &cube.uv.faces.west.normUv;
			t->vertexUV(pos.x, pos.y + size.y, pos.z + size.z, normUv->x, normUv->y);
			t->vertexUV(pos.x, pos.y + size.y, pos.z, normUv->z, normUv->y);
			t->vertexUV(pos.x, pos.y, pos.z, normUv->z, normUv->w);
			t->vertexUV(pos.x, pos.y, pos.z + size.z, normUv->x, normUv->w);

			// left (east)
			normUv = &cube.uv.faces.east.normUv;
			t->vertexUV(pos.x + size.x, pos.y, pos.z + size.z, normUv->z, normUv->w);
			t->vertexUV(pos.x + size.x, pos.y, pos.z, normUv->x, normUv->w);
			t->vertexUV(pos.x + size.x, pos.y + size.y, pos.z, normUv->x, normUv->y);
			t->vertexUV(pos.x + size.x, pos.y + size.y, pos.z + size.z, normUv->z, normUv->y);
		}

		t->transformMatrix = prevMatrix;
	}

	static void renderBone(JsonBone& bone, bool renderInsideFaces) {
		Tessellator* t = SDK::scn->getTessellator();
		glm::mat4x4 prevMatrix = t->transformMatrix;


		t->transformMatrix = getNewTransform(t->transformMatrix, glm::vec3(bone.pivot.x, bone.pivot.y, bone.pivot.z), glm::vec3(bone.rrotation.x, bone.rrotation.y, bone.rrotation.z));
		t->applyTransform = true;

		for (auto& cube : bone.cubes) {
			renderTexturedCube(cube, renderInsideFaces);
		}

		for (auto* child : bone.childBones)
			renderBone(*child, renderInsideFaces);


		t->transformMatrix = prevMatrix;
	}



	void JsonModel::render(ScreenContext* screenContext, BedrockTextureData texture, const Vec3<float>& position, const Vec3<float>& rotation, const Vec3<float>& scale, bool renderInsideFaces) {
		Tessellator* t = screenContext->getTessellator();
		const Vec3<float> prevScale = t->getPostScaleOffset();
		Vec3<float> renderOrigin = SDK::clientInstance->getLevelRender()->getOrigin().mul(-1.f);
		t->setPostScaleOffset(Vec3(0.0588235294f, 0.0588235294f, 0.0588235294f).mul(prevScale));

		t->transformMatrix = glm::mat4(1.f);
		t->applyTransform = true;

		//user movement
		glm::mat4x4 transform(1.f);
		transform = glm::translate(transform, glm::vec3(renderOrigin.x, renderOrigin.y, renderOrigin.z));
		transform = glm::translate(transform, glm::vec3(position.x, position.y, position.z));
		transform = glm::scale(transform, glm::vec3(scale.x, scale.y, scale.z));
		transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0, 0, 1));
		transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0, 1, 0));
		transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(1, 0, 0));
		auto& stack = SDK::clientInstance->getCamera().getWorldMatrixStack();

		stack.push();

		auto& matrix = stack.top().matrix;
		matrix = transform * matrix;

		*screenContext->getColorHolder() = MCCColor(1.f, 1.f, 1.f, 1.f);
		t->begin(mce::PrimitiveMode::QuadList, mQuadVerticeCount);

		for (auto* bone : this->rootBones)
			renderBone(*bone, renderInsideFaces);

		AABB modelAABB = this->aabb;
		glm::vec4 modelAabbStart = matrix * glm::vec4(modelAABB.lower.x, modelAABB.lower.y, modelAABB.lower.y, 1.f);
		glm::vec4 modelAabbStop = matrix * glm::vec4(modelAABB.upper.x, modelAABB.upper.y, modelAABB.upper.y, 1.f);
		modelAabbStart.x -= renderOrigin.x;
		modelAabbStart.y -= renderOrigin.y;
		modelAabbStart.z -= renderOrigin.z;
		modelAabbStop.x -= renderOrigin.x;
		modelAabbStop.y -= renderOrigin.y;
		modelAabbStop.z -= renderOrigin.z;

		modelAABB.lower.x = modelAabbStart.x;
		modelAABB.lower.y = modelAabbStart.y;
		modelAABB.lower.z = modelAabbStart.z;
		modelAABB.upper.x = modelAabbStop.x;
		modelAABB.upper.y = modelAabbStop.y;
		modelAABB.upper.z = modelAabbStop.z;

		MeshHelpers::renderMeshImmediately2(screenContext, t, MaterialUtils::getSelectionOverlay(), texture);

		t->setPostScaleOffset(prevScale);

		t->applyTransform = false;

		stack.pop();
	}

	void JsonModel::ComputeAABB() {
		AABB area;
		Vec3 modelScale = Vec3(0.058f, 0.058f, 0.058f);
		for (auto& bone : this->bones) {
			for (auto& cube : bone.cubes) {
				Vec3 pos = cube.origin.mul(Vec3(-1.f, 1.f, 1.f));
				Vec3 size = cube.size.mul(Vec3(-1.f, 1.f, 1.f));
				pos.x -= -cube.Inflate;
				pos.y -= cube.Inflate;
				pos.z -= cube.Inflate;
				size.x += -cube.Inflate * 2.f;
				size.y += cube.Inflate * 2.f;
				size.z += cube.Inflate * 2.f;

				area.expandTo(pos.mul(modelScale));
				area.expandTo(pos.add(size).mul(modelScale));
			}
		}
		this->aabb = area;
	}
}
