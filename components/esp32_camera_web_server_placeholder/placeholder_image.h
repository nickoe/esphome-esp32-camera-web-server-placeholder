#pragma once

#ifdef USE_ESP32

#include <cstdint>
#include <cstddef>

namespace esphome {
namespace esp32_camera_web_server_placeholder {

/*
 * Placeholder Image Generation Methods:
 * 
 * This file contains a JPEG image encoded as a C array for use as a placeholder
 * when the camera is not available or during initialization.
 * 
 * Method 1: Using xxd (Linux/Unix command-line tool)
 * ---------------------------------------------------
 * xxd -i placeholder.jpg > placeholder_image.h
 * 
 * This generates a C array from your JPEG file. You'll need to manually add
 * the namespace and ifdef guards.
 * 
 * Method 2: Using Python script
 * ------------------------------
 * import sys
 * 
 * def convert_image_to_header(image_path, output_path):
 *     with open(image_path, 'rb') as f:
 *         data = f.read()
 *     
 *     with open(output_path, 'w') as f:
 *         f.write('const uint8_t PLACEHOLDER_JPEG[] = {\n')
 *         for i, byte in enumerate(data):
 *             if i % 12 == 0:
 *                 f.write('  ')
 *             f.write(f'0x{byte:02x}')
 *             if i < len(data) - 1:
 *                 f.write(', ')
 *             if (i + 1) % 12 == 0:
 *                 f.write('\n')
 *         f.write('\n};\n')
 *         f.write(f'const size_t PLACEHOLDER_JPEG_SIZE = {len(data)};\n')
 * 
 * if __name__ == '__main__':
 *     convert_image_to_header('placeholder.jpg', 'placeholder_data.h')
 * 
 * Method 3: Using online converter
 * ---------------------------------
 * Visit: https://notisrac.github.io/FileToCArray/
 * 1. Upload your JPEG file
 * 2. Set the array name to "PLACEHOLDER_JPEG"
 * 3. Download the generated header file
 * 4. Copy the array into this file with appropriate namespaces
 */

// Small placeholder JPEG image (example data - replace with actual image)
const uint8_t PLACEHOLDER_JPEG[] = {
  0xff, 0xd8, 0xff, 0xe0, 0x00, 0x10, 0x4a, 0x46, 0x49, 0x46, 0x00, 0x01,
  0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0xff, 0xdb, 0x00, 0x43,
  0x00, 0x08, 0x06, 0x06, 0x07, 0x06, 0x05, 0x08, 0x07, 0x07, 0x07, 0x09,
  0x09, 0x08, 0x0a, 0x0c, 0x14, 0x0d, 0x0c, 0x0b, 0x0b, 0x0c, 0x19, 0x12,
  0x13, 0x0f, 0x14, 0x1d, 0x1a, 0x1f, 0x1e, 0x1d, 0x1a, 0x1c, 0x1c, 0x20,
  0x24, 0x2e, 0x27, 0x20, 0x22, 0x2c, 0x23, 0x1c, 0x1c, 0x28, 0x37, 0x29,
  0x2c, 0x30, 0x31, 0x34, 0x34, 0x34, 0x1f, 0x27, 0x39, 0x3d, 0x38, 0x32,
  0x3c, 0x2e, 0x33, 0x34, 0x32, 0xff, 0xd9
};

const size_t PLACEHOLDER_JPEG_SIZE = sizeof(PLACEHOLDER_JPEG);

}  // namespace esp32_camera_web_server_placeholder
}  // namespace esphome

#endif  // USE_ESP32
