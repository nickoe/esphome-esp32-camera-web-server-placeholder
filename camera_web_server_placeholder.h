#pragma once

#ifdef USE_ESP32

#include <cinttypes>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include "esphome/components/camera/camera.h"
#include "esphome/core/component.h"
#include "esphome/core/helpers.h"

struct httpd_req;

namespace esphome {
namespace esp32_camera_web_server_placeholder {

enum Mode { STREAM, SNAPSHOT };

class CameraWebServerPlaceholder : public Component, public camera::CameraListener {
 public:
  CameraWebServerPlaceholder();
  ~CameraWebServerPlaceholder();

  void setup() override;
  void on_shutdown() override;
  void dump_config() override;
  float get_setup_priority() const override;
  void set_port(uint16_t port) { this->port_ = port; }
  void set_mode(Mode mode) { this->mode_ = mode; }
  void set_placeholder_enabled(bool enabled) { this->placeholder_enabled_ = enabled; }
  void loop() override;

  void on_camera_image(const std::shared_ptr<camera::CameraImage> &image) override;

 protected:
  std::shared_ptr<camera::CameraImage> wait_for_image_();
  esp_err_t handler_(struct httpd_req *req);
  esp_err_t streaming_handler_(struct httpd_req *req);
  esp_err_t snapshot_handler_(struct httpd_req *req);
  esp_err_t send_placeholder_(struct httpd_req *req, bool is_stream);

  uint16_t port_{0};
  void *httpd_{nullptr};
  SemaphoreHandle_t semaphore_;
  std::shared_ptr<camera::CameraImage> image_;
  bool running_{false};
  bool placeholder_enabled_{true};
  Mode mode_{STREAM};
};

}  // namespace esp32_camera_web_server_placeholder
}  // namespace esphome

#endif  // USE_ESP32