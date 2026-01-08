#ifdef USE_ESP32

#include "camera_web_server_placeholder.h"
#include "esphome/core/application.h"
#include "esphome/core/hal.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"
#include "esphome/core/util.h"

#include <cstdlib>
#include <esp_http_server.h>
#include <utility>

namespace esphome {
namespace esp32_camera_web_server_placeholder {

static const int IMAGE_REQUEST_TIMEOUT = 1000;  // Reduced to 1 second for placeholder
static const char *const TAG = "camera_web_server_placeholder";

#define PART_BOUNDARY "123456789000000000000987654321"
#define CONTENT_TYPE "image/jpeg"
#define CONTENT_LENGTH "Content-Length"

static const char *const STREAM_HEADER = "HTTP/1.0 200 OK\r\n"
                                         "Access-Control-Allow-Origin: *\r\n"
                                         "Connection: close\r\n"
                                         "Content-Type: multipart/x-mixed-replace;boundary=" PART_BOUNDARY "\r\n"
                                         "\r\n"
                                         "--" PART_BOUNDARY "\r\n";
static const char *const STREAM_PART = "Content-Type: " CONTENT_TYPE "\r\n" CONTENT_LENGTH ": %u\r\n\r\n";
static const char *const STREAM_BOUNDARY = "\r\n"
                                           "--" PART_BOUNDARY "\r\n";

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

CameraWebServerPlaceholder::CameraWebServerPlaceholder() {}

CameraWebServerPlaceholder::~CameraWebServerPlaceholder() {}

void CameraWebServerPlaceholder::setup() {
  if (!camera::Camera::instance()) {
    ESP_LOGW(TAG, "No camera found, will serve placeholder only");
    // Don't mark as failed - we'll serve placeholder
  }

  this->semaphore_ = xSemaphoreCreateBinary();

  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = this->port_;
  config.ctrl_port = this->port_;
  config.max_open_sockets = 1;
  config.backlog_conn = 2;
  config.lru_purge_enable = true;

  if (httpd_start(&this->httpd_, &config) != ESP_OK) {
    mark_failed();
    return;
  }

  httpd_uri_t uri = {
      .uri = "/",
      .method = HTTP_GET,
      .handler = [](struct httpd_req *req) { return ((CameraWebServerPlaceholder *) req->user_ctx)->handler_(req); },
      .user_ctx = this};

  httpd_register_uri_handler(this->httpd_, &uri);

  if (camera::Camera::instance()) {
    camera::Camera::instance()->add_listener(this);
  }
}

void CameraWebServerPlaceholder::on_camera_image(const std::shared_ptr<camera::CameraImage> &image) {
  if (this->running_ && image->was_requested_by(camera::WEB_REQUESTER)) {
    this->image_ = image;
    xSemaphoreGive(this->semaphore_);
  }
}

void CameraWebServerPlaceholder::on_shutdown() {
  this->running_ = false;
  this->image_ = nullptr;
  httpd_stop(this->httpd_);
  this->httpd_ = nullptr;
  vSemaphoreDelete(this->semaphore_);
  this->semaphore_ = nullptr;
}

void CameraWebServerPlaceholder::dump_config() {
  ESP_LOGCONFIG(TAG, "ESP32 Camera Web Server (Placeholder):");
  ESP_LOGCONFIG(TAG, "  Port: %d", this->port_);
  ESP_LOGCONFIG(TAG, "  Mode: %s", this->mode_ == STREAM ? "stream" : "snapshot");
  ESP_LOGCONFIG(TAG, "  Placeholder: %s", this->placeholder_enabled_ ? "enabled" : "disabled");

  if (this->is_failed()) {
    ESP_LOGE(TAG, "  Setup Failed");
  }
}

float CameraWebServerPlaceholder::get_setup_priority() const { return setup_priority::LATE; }

void CameraWebServerPlaceholder::loop() {
  if (!this->running_) {
    this->image_ = nullptr;
  }
}

std::shared_ptr<esphome::camera::CameraImage> CameraWebServerPlaceholder::wait_for_image_() {
  std::shared_ptr<esphome::camera::CameraImage> image;
  image.swap(this->image_);

  if (!image) {
    xSemaphoreTake(this->semaphore_, IMAGE_REQUEST_TIMEOUT / portTICK_PERIOD_MS);
    image.swap(this->image_);
  }

  return image;
}

static esp_err_t httpd_send_all(httpd_req_t *r, const char *buf, size_t buf_len) {
  int ret;
  while (buf_len > 0) {
    ret = httpd_send(r, buf, buf_len);
    if (ret < 0) {
      return ESP_FAIL;
    }
    buf += ret;
    buf_len -= ret;
  }
  return ESP_OK;
}

esp_err_t CameraWebServerPlaceholder::send_placeholder_(struct httpd_req *req, bool is_stream) {
  esp_err_t res = ESP_OK;
  char part_buf[64];

  if (is_stream) {
    size_t hlen = snprintf(part_buf, 64, STREAM_PART, PLACEHOLDER_JPEG_SIZE);
    res = httpd_send_all(req, part_buf, hlen);
    if (res != ESP_OK) return res;
  }

  res = httpd_send_all(req, (const char *)PLACEHOLDER_JPEG, PLACEHOLDER_JPEG_SIZE);
  
  if (is_stream && res == ESP_OK) {
    res = httpd_send_all(req, STREAM_BOUNDARY, strlen(STREAM_BOUNDARY));
  }

  return res;
}

esp_err_t CameraWebServerPlaceholder::handler_(struct httpd_req *req) {
  esp_err_t res = ESP_FAIL;

  this->image_ = nullptr;
  this->running_ = true;

  switch (this->mode_) {
    case STREAM:
      res = this->streaming_handler_(req);
      break;
    case SNAPSHOT:
      res = this->snapshot_handler_(req);
      break;
  }

  this->running_ = false;
  this->image_ = nullptr;
  return res;
}

esp_err_t CameraWebServerPlaceholder::streaming_handler_(struct httpd_req *req) {
  esp_err_t res = ESP_OK;
  
  res = httpd_send_all(req, STREAM_HEADER, strlen(STREAM_HEADER));
  if (res != ESP_OK) {
    ESP_LOGW(TAG, "STREAM: failed to set HTTP header");
    return res;
  }

  uint32_t last_frame = millis();
  uint32_t frames = 0;
  uint32_t placeholder_frames = 0;

  if (camera::Camera::instance() && !camera::Camera::instance()->is_failed()) {
    camera::Camera::instance()->start_stream(esphome::camera::WEB_REQUESTER);
  }

  while (res == ESP_OK && this->running_) {
    auto image = this->wait_for_image_();

    if (!image) {
      // No camera image available
      if (this->placeholder_enabled_) {
        ESP_LOGD(TAG, "STREAM: serving placeholder frame");
        res = this->send_placeholder_(req, true);
        placeholder_frames++;
      } else {
        ESP_LOGW(TAG, "STREAM: no frame available");
        res = ESP_FAIL;
      }
    } else {
      // Send real camera image
      char part_buf[64];
      size_t hlen = snprintf(part_buf, 64, STREAM_PART, image->get_data_length());
      res = httpd_send_all(req, part_buf, hlen);
      
      if (res == ESP_OK) {
        res = httpd_send_all(req, (const char *)image->get_data_buffer(), image->get_data_length());
      }
      if (res == ESP_OK) {
        res = httpd_send_all(req, STREAM_BOUNDARY, strlen(STREAM_BOUNDARY));
      }
      frames++;
    }

    if (res == ESP_OK) {
      int64_t frame_time = millis() - last_frame;
      last_frame = millis();
      ESP_LOGV(TAG, "MJPG: %" PRIu32 "ms (%.1ffps)", (uint32_t)frame_time, 1000.0 / (uint32_t)frame_time);
    }

    // Delay between frames
    delay(100);  // 10fps max for placeholder
  }

  if (camera::Camera::instance() && !camera::Camera::instance()->is_failed()) {
    camera::Camera::instance()->stop_stream(esphome::camera::WEB_REQUESTER);
  }

  ESP_LOGI(TAG, "STREAM: closed. Real frames: %" PRIu32 ", Placeholder frames: %" PRIu32, frames, placeholder_frames);

  return res;
}

esp_err_t CameraWebServerPlaceholder::snapshot_handler_(struct httpd_req *req) {
  esp_err_t res = ESP_OK;

  if (camera::Camera::instance() && !camera::Camera::instance()->is_failed()) {
    camera::Camera::instance()->request_image(esphome::camera::WEB_REQUESTER);
  }

  auto image = this->wait_for_image_();

  if (!image) {
    if (this->placeholder_enabled_) {
      ESP_LOGD(TAG, "SNAPSHOT: serving placeholder");
      res = httpd_resp_set_type(req, CONTENT_TYPE);
      if (res == ESP_OK) {
        httpd_resp_set_hdr(req, "Content-Disposition", "inline; filename=placeholder.jpg");
        res = httpd_resp_send(req, (const char *)PLACEHOLDER_JPEG, PLACEHOLDER_JPEG_SIZE);
      }
    } else {
      ESP_LOGW(TAG, "SNAPSHOT: no frame available");
      httpd_resp_send_500(req);
      res = ESP_FAIL;
    }
    return res;
  }

  res = httpd_resp_set_type(req, CONTENT_TYPE);
  if (res != ESP_OK) {
    ESP_LOGW(TAG, "SNAPSHOT: failed to set HTTP response type");
    return res;
  }

  httpd_resp_set_hdr(req, "Content-Disposition", "inline; filename=capture.jpg");

  if (res == ESP_OK) {
    res = httpd_resp_send(req, (const char *)image->get_data_buffer(), image->get_data_length());
  }
  return res;
}

}  // namespace esp32_camera_web_server_placeholder
}  // namespace esphome

#endif  // USE_ESP32