#pragma once

#include <cstdint>

struct RingBufferControl {
	const uint32_t m_size;
	uint32_t m_current;
	uint32_t m_write;
	uint32_t m_read;
};