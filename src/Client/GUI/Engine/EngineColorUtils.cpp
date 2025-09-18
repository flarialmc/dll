#include "Engine.hpp"
#include "../../Client.hpp"
#include <cmath>
#include <string>
#include <stdexcept>

// Color utility functions extracted from Engine.cpp for better compilation performance

D2D_COLOR_F FlarialGUI::LerpColor(D2D_COLOR_F color1, D2D_COLOR_F color2, float t) {
	if (!Client::settings.getSettingByName<bool>("disableanims")->value) {
		// Interpolate each color channel separately
		float r = color1.r + (color2.r - color1.r) * t;
		float g = color1.g + (color2.g - color1.g) * t;
		float b = color1.b + (color2.b - color1.b) * t;
		float a = color1.a + (color2.a - color1.a) * t;

		return D2D1::ColorF(r, g, b, a);
	}
	else return color2;
}

void FlarialGUI::HSLToRGB(float h, float s, float l, float& r, float& g, float& b) {
	if (s == 0.0f) {
		r = g = b = l;
	}
	else {
		float q = (l < 0.5f) ? l * (1.0f + s) : l + s - l * s;
		float p = 2.0f * l - q;

		r = HueToRGB(p, q, h + 1.0f / 3.0f);
		g = HueToRGB(p, q, h);
		b = HueToRGB(p, q, h - 1.0f / 3.0f);
	}
}

float FlarialGUI::HueToRGB(float p, float q, float t) {
	if (t < 0.0f) t += 1.0f;
	if (t > 1.0f) t -= 1.0f;

	if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
	if (t < 1.0f / 2.0f) return q;
	if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;

	return p;
}

ImColor FlarialGUI::D2DColorToImColor(D2D1_COLOR_F color) {
	return ImColor(color.r, color.g, color.b, color.a);
}

HSV FlarialGUI::RGBtoHSV(D2D1_COLOR_F rgb) {
	float r = rgb.r;
	float g = rgb.g;
	float b = rgb.b;

	HSV out{};
	double min, max, delta;

	min = r < g ? r : g;
	min = min < b ? min : b;

	max = r > g ? r : g;
	max = max > b ? max : b;

	out.value = (float)max;                                // v
	delta = max - min;
	if (delta < 0.00001) {
		out.saturation = 0;
		out.hue = 0; // undefined, maybe nan?
		return out;
	}
	if (max > 0.0) { // NOTE: if Max is == 0, this divide would cause a crash
		out.saturation = (float)(delta / max);                  // s
	}
	else {
		// if max is 0, then r = g = b = 0
		// s = 0, h is undefined
		out.saturation = 0.0;
		out.hue = NAN;                            // its now undefined
		return out;
	}
	if (r >= max)                           // > is bogus, just keeps compilor happy
		out.hue = (float)((g - b) / delta);        // between yellow & magenta
	else if (g >= max)
		out.hue = 2.0f + (b - r) / delta;  // between cyan & yellow
	else
		out.hue = 4.0f + (r - g) / delta;  // between magenta & cyan

	out.hue *= 60.0;                              // degrees

	if (out.hue < 0.0)
		out.hue += 360.0;

	return out;
}

D2D1::ColorF FlarialGUI::HSVtoColorF(float H, float s, float v) {
	if (H > 360 || H < 0 || s > 1 || s < 0 || v > 1 || v < 0) {
		return { 0, 0, 0 };
	}
	float C = s * v;
	float X = C * (1 - abs(fmod(H / 60.0f, 2) - 1));
	float m = v - C;
	float r, g, b;
	if (H >= 0 && H < 60) {
		r = C, g = X, b = 0;
	}
	else if (H >= 60 && H < 120) {
		r = X, g = C, b = 0;
	}
	else if (H >= 120 && H < 180) {
		r = 0, g = C, b = X;
	}
	else if (H >= 180 && H < 240) {
		r = 0, g = X, b = C;
	}
	else if (H >= 240 && H < 300) {
		r = X, g = 0, b = C;
	}
	else {
		r = C, g = 0, b = X;
	}

	return { r + m, g + m, b + m };
}

D2D1::ColorF FlarialGUI::HexToColorF(const std::string& hexString) {
	if (hexString.length() != 6) {
		// Return black if the string length is not 6
		return { 0.0f, 0.0f, 0.0f };
	}

	try {
		// Convert the hex string to individual color components
		uint32_t hex = std::stoul(hexString, nullptr, 16);
		uint8_t red = (hex >> 16) & 0xFF;
		uint8_t green = (hex >> 8) & 0xFF;
		uint8_t blue = hex & 0xFF;

		// Normalize the color components to the range [0.0, 1.0]
		float normalizedRed = static_cast<float>(red) / 255.0f;
		float normalizedGreen = static_cast<float>(green) / 255.0f;
		float normalizedBlue = static_cast<float>(blue) / 255.0f;

		// Return the D2D1::ColorF struct
		return { normalizedRed, normalizedGreen, normalizedBlue };
	}
	catch (const std::exception&) {
		// Return black if the conversion fails
		return { 0.0f, 0.0f, 0.0f };
	}
}

std::string FlarialGUI::ColorFToHex(const D2D1_COLOR_F& color) {
	// Convert the color components from the range [0.0, 1.0] to [0, 255]
	auto red = static_cast<uint8_t>(color.r * 255.0f);
	auto green = static_cast<uint8_t>(color.g * 255.0f);
	auto blue = static_cast<uint8_t>(color.b * 255.0f);

	// Create a 6-character hex string
	char hexString[7];
	snprintf(hexString, sizeof(hexString), "%02X%02X%02X", red, green, blue);

	return std::string(hexString);
}