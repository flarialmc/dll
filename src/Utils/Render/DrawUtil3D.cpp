#include "DrawUtil3D.hpp"
#include "MaterialUtils.hpp"
#include "../../SDK/Client/Render/Tessellator/MeshHelpers.hpp"

MCDrawUtil3D::MCDrawUtil3D(LevelRender* renderer, ScreenContext* ctx, mce::MaterialPtr* material)
    : levelRenderer(renderer), screenContext(ctx), material(material) {
    
    if (!material) {
        this->material = MaterialUtils::getSelectionOverlay();
    }
}

void MCDrawUtil3D::drawLine(Vec3<float> const& p1, Vec3<float> const& p2, D2D_COLOR_F const& color, bool immediate) {
    auto scn = screenContext;
	auto tess = scn->getTessellator();
	*scn->getColorHolder() = {1.f,1.f,1.f,1.f};
	tess->begin(mce::PrimitiveMode::LineStrip, 1); // linestrip
	tess->color(color.r, color.g, color.b, color.a);
	auto origin = levelRenderer->getOrigin();
	Vec3<float> a = p1;
	Vec3<float> b = p2;

	a.x -= origin.x;
	a.y -= origin.y;
	a.z -= origin.z;

	b.x -= origin.x;
	b.y -= origin.y;
	b.z -= origin.z;
	tess->vertex(a.x, a.y, a.z);
	tess->vertex(b.x, b.y, b.z);
    if (immediate) flush();
}

void MCDrawUtil3D::drawQuad(Vec3<float> a, Vec3<float> b, Vec3<float> c, Vec3<float> d, D2D_COLOR_F const& color) {
	auto scn = screenContext;
	auto tess = scn->getTessellator();
	*scn->getColorHolder() = { 1.f, 1.f, 1.f, 1.f };
	tess->begin(mce::PrimitiveMode::LineStrip, 1); // linestrip
	tess->color(color.r, color.g, color.b, color.a);
	auto origin = levelRenderer->getLevelRendererPlayer()->cameraPos;
	a = a.sub(origin);
	b = b.sub(origin);
	c = c.sub(origin);
	d = d.sub(origin);

	tess->vertex(a.x, a.y, a.z);
	tess->vertex(b.x, b.y, b.z);


	tess->begin(mce::PrimitiveMode::LineStrip, 1);
	tess->color(color.r, color.g, color.b, color.a);
	tess->vertex(b.x, b.y, b.z);
	tess->vertex(c.x, c.y, c.z);

	tess->begin(mce::PrimitiveMode::LineStrip, 1);
	tess->color(color.r, color.g, color.b, color.a);
	tess->vertex(c.x, c.y, c.z);
	tess->vertex(d.x, d.y, d.z);

	tess->begin(mce::PrimitiveMode::LineStrip, 1);
	tess->color(color.r, color.g, color.b, color.a);
	tess->vertex(d.x, d.y, d.z);
	tess->vertex(a.x, a.y, a.z);
}

void MCDrawUtil3D::fillQuad(Vec3<float> p1, Vec3<float> p2, Vec3<float> p3, Vec3<float> p4, D2D_COLOR_F const& color) {
	auto scn = screenContext;
	
	auto tess = scn->getTessellator();
	*scn->getColorHolder() = { 1.f, 1.f, 1.f, 1.f };
	tess->begin(mce::PrimitiveMode::QuadList, 4); // linestrip
	tess->color(color.r, color.g, color.b, color.a);
	auto origin = levelRenderer->getOrigin();
	Vec3<float> a = p1;
	Vec3<float> b = p2;
	Vec3<float> c = p3;
	Vec3<float> d = p4;

	a.x -= origin.x;
	a.y -= origin.y;
	a.z -= origin.z;

	b.x -= origin.x;
	b.y -= origin.y;
	b.z -= origin.z;

	c.x -= origin.x;
	c.y -= origin.y;
	c.z -= origin.z;

	d.x -= origin.x;
	d.y -= origin.y;
	d.z -= origin.z;

	tess->vertex(a.x, a.y, a.z);
	tess->vertex(b.x, b.y, b.z);
	tess->vertex(c.x, c.y, c.z);
	tess->vertex(d.x, d.y, d.z);

	// render other side
	tess->vertex(d.x, d.y, d.z);
	tess->vertex(c.x, c.y, c.z);
	tess->vertex(b.x, b.y, b.z);
	tess->vertex(a.x, a.y, a.z);
}

void MCDrawUtil3D::drawBox(AABB const& bb, D2D_COLOR_F const& color) {
	auto scn = screenContext;
	auto tess = scn->getTessellator();
	std::vector<std::pair<Vec3<float>, Vec3<float>>> verticies = {
		{ bb.lower, Vec3<float>(bb.lower.x, bb.upper.y, bb.lower.z) },
		{ Vec3<float>(bb.upper.x, bb.lower.y, bb.lower.z), Vec3<float>(bb.upper.x, bb.upper.y, bb.lower.z)},
		{ Vec3<float>(bb.lower.x, bb.lower.y, bb.upper.z), Vec3<float>(bb.lower.x, bb.upper.y, bb.upper.z)},
		{ Vec3<float>(bb.upper.x, bb.lower.y, bb.upper.z), Vec3<float>(bb.upper.x, bb.upper.y, bb.upper.z) },
	};
	for (auto& vert : verticies) {
		drawLine(vert.first, vert.second, color);
	}

	drawQuad(Vec3<float>(bb.lower.x, bb.lower.y, bb.lower.z), Vec3<float>(bb.upper.x, bb.lower.y, bb.lower.z),
		Vec3<float>(bb.upper.x, bb.lower.y, bb.upper.z), Vec3<float>(bb.lower.x, bb.lower.y, bb.upper.z), color);

	drawQuad(Vec3<float>(bb.lower.x, bb.upper.y, bb.lower.z), Vec3<float>(bb.upper.x, bb.upper.y, bb.lower.z),
		Vec3<float>(bb.upper.x, bb.upper.y, bb.upper.z), Vec3<float>(bb.lower.x, bb.upper.y, bb.upper.z), color);

	drawQuad(Vec3<float>(bb.lower.x, bb.upper.y, bb.lower.z), Vec3<float>(bb.upper.x, bb.upper.y, bb.lower.z),
		Vec3<float>(bb.upper.x, bb.upper.y, bb.upper.z), Vec3<float>(bb.lower.x, bb.upper.y, bb.upper.z), color);
}

void MCDrawUtil3D::flush() {
    MeshHelpers::renderMeshImmediately(screenContext, screenContext->getTessellator(), material);
}