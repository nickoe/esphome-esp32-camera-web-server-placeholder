#pragma once

#include <cstddef>
#include <cstdint>

// Placeholder JPEG image used when the camera has no frame available.
//
// This is a 320x240 JPEG.
static const uint8_t PLACEHOLDER_JPEG[] = {
  /* placeholder_jpg (len=2915) */
  0xFF, 0xD8, 0xFF, 0xE0, 0x00, 0x10, 0x4A, 0x46, 0x49, 0x46, 0x00, 0x01,
  0x01, 0x01, 0x00, 0x60, 0x00, 0x60, 0x00, 0x00
  // TODO: paste full 2915-byte JPEG content here
};

static const size_t PLACEHOLDER_JPEG_SIZE = sizeof(PLACEHOLDER_JPEG);
