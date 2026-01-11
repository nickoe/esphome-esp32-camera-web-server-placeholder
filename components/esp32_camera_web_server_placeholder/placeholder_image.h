#pragma once

#include <cstdint>
#include <cstddef>

// =============================================================================
// PLACEHOLDER JPEG IMAGE
// =============================================================================
// This is a minimal 1x1 pixel gray JPEG image shown when camera is unavailable.
//
// To create your own placeholder image:
// 
// METHOD 1: Using xxd (Linux/Mac/WSL)
// ---------------------------------
// 1. Create your image (PNG, JPEG, etc.) - recommended: 320x240 or smaller
// 2. Convert to JPEG if needed:
//    convert placeholder.png -quality 80 placeholder.jpg
// 3. Convert to C array:
//    xxd -i placeholder.jpg > placeholder.h
// 4. Copy the array contents here
//
// METHOD 2: Using Python
// ---------------------------------
// 1. Create your image file
// 2. Run this Python script:
//
//    import sys
//    
//    def image_to_cpp_array(image_path):
//        with open(image_path, 'rb') as f:
//            data = f.read()
//        
//        print("static const uint8_t PLACEHOLDER_JPEG[] = {")
//        for i in range(0, len(data), 12):
//            chunk = data[i:i+12]
//            hex_values = ', '.join(f'0x{b:02X}' for b in chunk)
//            print(f"  {hex_values},")
//        print("};")
//        print(f"\nstatic const size_t PLACEHOLDER_JPEG_SIZE = {len(data)};")
//    
//    if __name__ == "__main__":
//        image_to_cpp_array(sys.argv[1])
//
// 3. Run: python img_to_array.py placeholder.jpg
//
// METHOD 3: Online converter
// ---------------------------------
// 1. Use a tool like: https://notisrac.github.io/FileToCArray/
// 2. Upload your JPEG file
// 3. Copy the generated array
//
// Tips:
// - Keep the image small (< 50KB) to save flash memory
// - Use low JPEG quality (60-80) for smaller size
// - Consider a simple icon or text like "Camera Offline"
// - Test that it's a valid JPEG by opening in an image viewer first
//
// Current placeholder: 1x1 gray pixel (~150 bytes)
// =============================================================================

static const uint8_t PLACEHOLDER_JPEG[] = {
  0xFF, 0xD8, 0xFF, 0xE0, 0x00, 0x10, 0x4A, 0x46, 0x49, 0x46, 0x00, 0x01, 0x01, 0x01, 0x00, 0x48,
  0x00, 0x48, 0x00, 0x00, 0xFF, 0xDB, 0x00, 0x43, 0x00, 0x03, 0x02, 0x02, 0x02, 0x02, 0x02, 0x03,
  0x02, 0x02, 0x02, 0x03, 0x03, 0x03, 0x03, 0x04, 0x06, 0x04, 0x04, 0x04, 0x04, 0x04, 0x08, 0x06,
  0x06, 0x05, 0x06, 0x09, 0x08, 0x0A, 0x0A, 0x09, 0x08, 0x09, 0x09, 0x0A, 0x0C, 0x0F, 0x0C, 0x0A,
  0x0B, 0x0E, 0x0B, 0x09, 0x09, 0x0D, 0x11, 0x0D, 0x0E, 0x0F, 0x10, 0x10, 0x11, 0x10, 0x0A, 0x0C,
  0x12, 0x13, 0x12, 0x10, 0x13, 0x0F, 0x10, 0x10, 0x10, 0xFF, 0xC9, 0x00, 0x0B, 0x08, 0x00, 0x01,
  0x00, 0x01, 0x01, 0x01, 0x11, 0x00, 0xFF, 0xCC, 0x00, 0x06, 0x00, 0x10, 0x10, 0x05, 0xFF, 0xDA,
  0x00, 0x08, 0x01, 0x01, 0x00, 0x00, 0x3F, 0x00, 0xD2, 0xCF, 0x20, 0xFF, 0xD9
};

static const size_t PLACEHOLDER_JPEG_SIZE = sizeof(PLACEHOLDER_JPEG);

