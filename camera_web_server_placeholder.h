#pragma once

#include "esphome/core/component.h"
#include "esphome/components/esp32_camera/esp32_camera.h"

namespace esphome {
namespace camera_web_server_placeholder {

class CameraWebServerPlaceholder : public Component {
 public:
  CameraWebServerPlaceholder() = default;

  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;

  void set_port(uint16_t port) { this->port_ = port; }
  void set_camera(esp32_camera::ESP32Camera *camera) { this->camera_ = camera; }

 protected:
  uint16_t port_{8080};
  esp32_camera::ESP32Camera *camera_{nullptr};
};

}  // namespace camera_web_server_placeholder
}  // namespace esphome
