#include "Cosmetic.hpp"
#include <GUI/Engine/Engine.hpp>

Vec3<float> rotateVec(const Vec3<float>& v, float yaw, float pitch) {
    // Rotate around X-axis (pitch)
    float cosPitch = cos(pitch);
    float sinPitch = sin(pitch);
    float x1 = v.x;
    float y1 = v.y * cosPitch - v.z * sinPitch;
    float z1 = v.y * sinPitch + v.z * cosPitch;

    // Rotate around Y-axis (yaw)
    float cosYaw = cos(yaw);
    float sinYaw = sin(yaw);
    float x2 = x1 * cosYaw + z1 * sinYaw;
    float z2 = -x1 * sinYaw + z1 * cosYaw;
    float y2 = y1;

    return Vec3<float>(x2, y2, z2);
}

Vec3<float> transformVertex(Vec3<float>& V, Vec3<float>& P_pivot, float headYaw, float headPitch) {
    Vec3<float> V_rel = V.sub(P_pivot);          // Subtract pivot to get relative position
    Vec3<float> V_rot = rotateVec(V_rel, headYaw, headPitch); // Rotate the relative vector
    return P_pivot.add(V_rot);                   // Add pivot back to get final position
}

class RedBandanna : public Cosmetic {
public:
    
    RedBandanna() : Cosmetic("Red Bandanna") {}

    void render(Vec3<float> pos, MCDrawUtil3D& dc, float headYaw, float headPitch) override {
        D2D1_COLOR_F color;
        color.r = .8f;
        color.g = .5f;
        color.b = .1f;
        color.a = 1.f;

        auto drawUp = [&]() {
            // Colors for the bandanna
            D2D1_COLOR_F headbandColor = { 0.6f, 0.1f, 0.1f, 1.0f };  // Dark red for main bandanna
            D2D1_COLOR_F headbandAccent = { 0.8f, 0.2f, 0.2f, 1.0f }; // Lighter red for accent
            D2D1_COLOR_F knotColor = { 0.4f, 0.05f, 0.05f, 1.0f };    // Darker maroon for knot and tails

            // Dimensions
            float headSize = 0.5f;           // Head size (assuming Bedrock Edition scale)
            float bandHeight = 0.1f;         // Height of the bandanna
            float bandStartY = pos.y + 0.12f; // Starting y-position (middle of head)
            float smallQuadSize = 0.1f;      // Size of each segment for the bandanna wrap
            int numSegments = static_cast<int>(headSize / smallQuadSize); // 5 segments
            float thickness = 0.02f;         // Thickness of the bandanna

            // Pivot point at the base center of the head for transformations
            Vec3<float> P_pivot(pos.x + headSize / 2, pos.y - 0.2, pos.z + headSize / 2);

            // Lambda to transform vertices based on head rotation
            auto transform = [&](Vec3<float> V) {
                return transformVertex(V, P_pivot, headYaw, headPitch);
                };

            // **Front Face** (z = pos.z, outer face at z - thickness)
            for (int i = 0; i < numSegments; i++) {
                float x_start = pos.x + i * smallQuadSize;
                float x_end = pos.x + (i + 1) * smallQuadSize;
                D2D1_COLOR_F color = (i % 2 == 0) ? headbandColor : headbandAccent;

                Vec3<float> inner_bottom_left = transform(Vec3<float>(x_start, bandStartY, pos.z));
                Vec3<float> inner_bottom_right = transform(Vec3<float>(x_end, bandStartY, pos.z));
                Vec3<float> inner_top_left = transform(Vec3<float>(x_start, bandStartY + bandHeight, pos.z));
                Vec3<float> inner_top_right = transform(Vec3<float>(x_end, bandStartY + bandHeight, pos.z));
                Vec3<float> outer_bottom_left = transform(Vec3<float>(x_start, bandStartY, pos.z - thickness));
                Vec3<float> outer_bottom_right = transform(Vec3<float>(x_end, bandStartY, pos.z - thickness));
                Vec3<float> outer_top_left = transform(Vec3<float>(x_start, bandStartY + bandHeight, pos.z - thickness));
                Vec3<float> outer_top_right = transform(Vec3<float>(x_end, bandStartY + bandHeight, pos.z - thickness));

                dc.fillQuad(outer_bottom_left, outer_bottom_right, outer_top_right, outer_top_left, color); // Front
                dc.fillQuad(inner_top_left, inner_top_right, outer_top_right, outer_top_left, color);      // Top
                dc.fillQuad(inner_bottom_left, outer_bottom_left, outer_bottom_right, inner_bottom_right, color); // Bottom
                dc.fillQuad(inner_bottom_left, inner_top_left, outer_top_left, outer_bottom_left, color);  // Left
                dc.fillQuad(inner_bottom_right, outer_bottom_right, outer_top_right, inner_top_right, color); // Right
            }

            // **Back Face** (z = pos.z + headSize, outer face at z + headSize + thickness)
            for (int i = 0; i < numSegments; i++) {
                float x_start = pos.x + i * smallQuadSize;
                float x_end = pos.x + (i + 1) * smallQuadSize;
                D2D1_COLOR_F color = (i % 2 == 0) ? headbandColor : headbandAccent;

                Vec3<float> inner_bottom_left = transform(Vec3<float>(x_start, bandStartY, pos.z + headSize));
                Vec3<float> inner_bottom_right = transform(Vec3<float>(x_end, bandStartY, pos.z + headSize));
                Vec3<float> inner_top_left = transform(Vec3<float>(x_start, bandStartY + bandHeight, pos.z + headSize));
                Vec3<float> inner_top_right = transform(Vec3<float>(x_end, bandStartY + bandHeight, pos.z + headSize));
                Vec3<float> outer_bottom_left = transform(Vec3<float>(x_start, bandStartY, pos.z + headSize + thickness));
                Vec3<float> outer_bottom_right = transform(Vec3<float>(x_end, bandStartY, pos.z + headSize + thickness));
                Vec3<float> outer_top_left = transform(Vec3<float>(x_start, bandStartY + bandHeight, pos.z + headSize + thickness));
                Vec3<float> outer_top_right = transform(Vec3<float>(x_end, bandStartY + bandHeight, pos.z + headSize + thickness));

                dc.fillQuad(outer_bottom_left, outer_bottom_right, outer_top_right, outer_top_left, color); // Back
                dc.fillQuad(inner_top_left, inner_top_right, outer_top_right, outer_top_left, color);      // Top
                dc.fillQuad(inner_bottom_left, outer_bottom_left, outer_bottom_right, inner_bottom_right, color); // Bottom
                dc.fillQuad(inner_bottom_left, inner_top_left, outer_top_left, outer_bottom_left, color);  // Left
                dc.fillQuad(inner_bottom_right, outer_bottom_right, outer_top_right, inner_top_right, color); // Right
            }

            // **Knot and Tails on Back Face**
            float x_center = pos.x + headSize / 2;  // Center x of back face
            float y_center = bandStartY + bandHeight / 2; // Center y of bandanna
            float z_back = pos.z + headSize;        // Back face z-position

            // Knot dimensions
            float knotWidth = 0.15f;  // Wider knot for realism
            float knotHeight = 0.1f;  // Height of knot
            float knotDepth = 0.05f;  // How much the knot protrudes

            // First crossed quad for knot (top-left to bottom-right)
            Vec3<float> p1 = transform(Vec3<float>(x_center - knotWidth / 2, y_center + knotHeight / 2, z_back));
            Vec3<float> p2 = transform(Vec3<float>(x_center + knotWidth / 2, y_center - knotHeight / 2, z_back));
            Vec3<float> p3 = transform(Vec3<float>(x_center + knotWidth / 2, y_center - knotHeight / 2, z_back + knotDepth));
            Vec3<float> p4 = transform(Vec3<float>(x_center - knotWidth / 2, y_center + knotHeight / 2, z_back + knotDepth));
            dc.fillQuad(p1, p2, p3, p4, knotColor);

            // Second crossed quad for knot (bottom-left to top-right)
            Vec3<float> q1 = transform(Vec3<float>(x_center - knotWidth / 2, y_center - knotHeight / 2, z_back));
            Vec3<float> q2 = transform(Vec3<float>(x_center + knotWidth / 2, y_center + knotHeight / 2, z_back));
            Vec3<float> q3 = transform(Vec3<float>(x_center + knotWidth / 2, y_center + knotHeight / 2, z_back + knotDepth));
            Vec3<float> q4 = transform(Vec3<float>(x_center - knotWidth / 2, y_center - knotHeight / 2, z_back + knotDepth));
            dc.fillQuad(q1, q2, q3, q4, knotColor);

            // Tails dimensions
            float tailWidth = 0.08f;  // Width of each tail at the top
            float tailLength = 0.3f;  // Length of hanging tails
            float slant = 0.04f;      // Slight outward spread for natural hang

            // Left tail (hanging from knot)
            Vec3<float> tail_left_top_left = transform(Vec3<float>(x_center - tailWidth, bandStartY, z_back + knotDepth));
            Vec3<float> tail_left_top_right = transform(Vec3<float>(x_center, bandStartY, z_back + knotDepth));
            Vec3<float> tail_left_bottom_left = transform(Vec3<float>(x_center - tailWidth - slant, bandStartY - tailLength, z_back + knotDepth));
            Vec3<float> tail_left_bottom_right = transform(Vec3<float>(x_center - slant, bandStartY - tailLength, z_back + knotDepth));
            dc.fillQuad(tail_left_top_left, tail_left_top_right, tail_left_bottom_right, tail_left_bottom_left, knotColor);

            // Right tail (hanging from knot)
            Vec3<float> tail_right_top_left = transform(Vec3<float>(x_center, bandStartY, z_back + knotDepth));
            Vec3<float> tail_right_top_right = transform(Vec3<float>(x_center + tailWidth, bandStartY, z_back + knotDepth));
            Vec3<float> tail_right_bottom_left = transform(Vec3<float>(x_center + slant, bandStartY - tailLength, z_back + knotDepth));
            Vec3<float> tail_right_bottom_right = transform(Vec3<float>(x_center + tailWidth + slant, bandStartY - tailLength, z_back + knotDepth));
            dc.fillQuad(tail_right_top_left, tail_right_top_right, tail_right_bottom_right, tail_right_bottom_left, knotColor);

            // **Left Face** (x = pos.x, outer face at x - thickness)
            for (int i = 0; i < numSegments; i++) {
                float z_start = pos.z + i * smallQuadSize;
                float z_end = pos.z + (i + 1) * smallQuadSize;
                D2D1_COLOR_F color = (i % 2 == 0) ? headbandColor : headbandAccent;

                Vec3<float> inner_bottom_front = transform(Vec3<float>(pos.x, bandStartY, z_start));
                Vec3<float> inner_bottom_back = transform(Vec3<float>(pos.x, bandStartY, z_end));
                Vec3<float> inner_top_front = transform(Vec3<float>(pos.x, bandStartY + bandHeight, z_start));
                Vec3<float> inner_top_back = transform(Vec3<float>(pos.x, bandStartY + bandHeight, z_end));
                Vec3<float> outer_bottom_front = transform(Vec3<float>(pos.x - thickness, bandStartY, z_start));
                Vec3<float> outer_bottom_back = transform(Vec3<float>(pos.x - thickness, bandStartY, z_end));
                Vec3<float> outer_top_front = transform(Vec3<float>(pos.x - thickness, bandStartY + bandHeight, z_start));
                Vec3<float> outer_top_back = transform(Vec3<float>(pos.x - thickness, bandStartY + bandHeight, z_end));

                dc.fillQuad(outer_bottom_front, outer_bottom_back, outer_top_back, outer_top_front, color); // Left
                dc.fillQuad(inner_top_front, inner_top_back, outer_top_back, outer_top_front, color);      // Top
                dc.fillQuad(inner_bottom_front, outer_bottom_front, outer_bottom_back, inner_bottom_back, color); // Bottom
                dc.fillQuad(inner_bottom_front, inner_top_front, outer_top_front, outer_bottom_front, color); // Front
                dc.fillQuad(inner_bottom_back, outer_bottom_back, outer_top_back, inner_top_back, color);  // Back
            }

            // **Right Face** (x = pos.x + headSize, outer face at x + headSize + thickness)
            for (int i = 0; i < numSegments; i++) {
                float z_start = pos.z + i * smallQuadSize;
                float z_end = pos.z + (i + 1) * smallQuadSize;
                D2D1_COLOR_F color = (i % 2 == 0) ? headbandColor : headbandAccent;

                Vec3<float> inner_bottom_front = transform(Vec3<float>(pos.x + headSize, bandStartY, z_start));
                Vec3<float> inner_bottom_back = transform(Vec3<float>(pos.x + headSize, bandStartY, z_end));
                Vec3<float> inner_top_front = transform(Vec3<float>(pos.x + headSize, bandStartY + bandHeight, z_start));
                Vec3<float> inner_top_back = transform(Vec3<float>(pos.x + headSize, bandStartY + bandHeight, z_end));
                Vec3<float> outer_bottom_front = transform(Vec3<float>(pos.x + headSize + thickness, bandStartY, z_start));
                Vec3<float> outer_bottom_back = transform(Vec3<float>(pos.x + headSize + thickness, bandStartY, z_end));
                Vec3<float> outer_top_front = transform(Vec3<float>(pos.x + headSize + thickness, bandStartY + bandHeight, z_start));
                Vec3<float> outer_top_back = transform(Vec3<float>(pos.x + headSize + thickness, bandStartY + bandHeight, z_end));

                dc.fillQuad(outer_bottom_front, outer_bottom_back, outer_top_back, outer_top_front, color); // Right
                dc.fillQuad(inner_top_front, inner_top_back, outer_top_back, outer_top_front, color);      // Top
                dc.fillQuad(inner_bottom_front, outer_bottom_front, outer_bottom_back, inner_bottom_back, color); // Bottom
                dc.fillQuad(inner_bottom_front, inner_top_front, outer_top_front, outer_bottom_front, color); // Front
                dc.fillQuad(inner_bottom_back, outer_bottom_back, outer_top_back, inner_top_back, color);  // Back
            }
            };

        drawUp();
    }
};