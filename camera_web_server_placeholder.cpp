#include "esphome.h"
#include "esphome/core/log.h"
#include "esphome/components/esp32_camera/esp32_camera.h"
#include "esphome/components/web_server_base/web_server_base.h"

namespace esphome {
namespace esp32_camera {

static const char *const TAG = "esp32_camera.web_server_placeholder";

// Placeholder JPEG image (1x1 pixel gray image)
// This is a valid minimal JPEG file that can be served when camera is not available
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

class CameraWebServerPlaceholder : public Component {
 public:
  CameraWebServerPlaceholder(ESP32Camera *camera) : camera_(camera) {}

  void setup() override {
    ESP_LOGCONFIG(TAG, "Setting up ESP32 Camera Web Server with Placeholder...");
    
    // Register web server handlers
    if (web_server_base::get_global_web_server_base() != nullptr) {
      web_server_base::get_global_web_server_base()->init();
      this->setup_handlers_();
    } else {
      ESP_LOGE(TAG, "Web server base not found!");
    }
  }

  float get_setup_priority() const override { return setup_priority::LATE; }

 protected:
  void setup_handlers_() {
    auto *server = web_server_base::get_global_web_server_base()->get_server();
    
    // Snapshot endpoint
    server->on("/snapshot.jpg", HTTP_GET, [this](AsyncWebServerRequest *request) {
      this->handle_snapshot_(request);
    });

    // Stream endpoint
    server->on("/stream", HTTP_GET, [this](AsyncWebServerRequest *request) {
      this->handle_stream_(request);
    });

    // Main camera page
    server->on("/camera", HTTP_GET, [this](AsyncWebServerRequest *request) {
      this->handle_camera_page_(request);
    });

    ESP_LOGCONFIG(TAG, "Registered camera web server handlers");
  }

  void handle_snapshot_(AsyncWebServerRequest *request) {
    if (this->camera_ == nullptr) {
      ESP_LOGW(TAG, "Camera not available, serving placeholder");
      this->serve_placeholder_(request);
      return;
    }

    auto pic = this->camera_->get_image();
    if (pic == nullptr || pic->get_data_length() == 0) {
      ESP_LOGW(TAG, "Failed to capture image, serving placeholder");
      this->serve_placeholder_(request);
      return;
    }

    AsyncWebServerResponse *response = request->beginResponse_P(
        200, "image/jpeg", pic->get_data_buffer(), pic->get_data_length());
    response->addHeader("Content-Disposition", "inline; filename=snapshot.jpg");
    response->addHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    request->send(response);
  }

  void handle_stream_(AsyncWebServerRequest *request) {
    if (this->camera_ == nullptr) {
      request->send(503, "text/plain", "Camera not available");
      return;
    }

    AsyncWebServerResponse *response = request->beginChunkedResponse(
        "multipart/x-mixed-replace; boundary=frame",
        [this](uint8_t *buffer, size_t maxLen, size_t index) -> size_t {
          return this->stream_handler_(buffer, maxLen, index);
        });
    
    response->addHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    request->send(response);
  }

  size_t stream_handler_(uint8_t *buffer, size_t maxLen, size_t index) {
    if (this->camera_ == nullptr) {
      return 0;
    }

    auto pic = this->camera_->get_image();
    if (pic == nullptr || pic->get_data_length() == 0) {
      // Serve placeholder in stream
      static const char *header = "--frame\r\nContent-Type: image/jpeg\r\n\r\n";
      static const char *footer = "\r\n";
      
      size_t header_len = strlen(header);
      size_t footer_len = strlen(footer);
      size_t total_len = header_len + PLACEHOLDER_JPEG_SIZE + footer_len;
      
      if (maxLen < total_len) {
        return 0;
      }
      
      memcpy(buffer, header, header_len);
      memcpy(buffer + header_len, PLACEHOLDER_JPEG, PLACEHOLDER_JPEG_SIZE);
      memcpy(buffer + header_len + PLACEHOLDER_JPEG_SIZE, footer, footer_len);
      
      delay(100); // Small delay for streaming
      return total_len;
    }

    // Build multipart frame
    String frame = "--frame\r\nContent-Type: image/jpeg\r\n\r\n";
    size_t frame_len = frame.length();
    size_t pic_len = pic->get_data_length();
    size_t footer_len = 2; // "\r\n"
    
    size_t total_len = frame_len + pic_len + footer_len;
    
    if (maxLen < total_len) {
      return 0;
    }
    
    memcpy(buffer, frame.c_str(), frame_len);
    memcpy(buffer + frame_len, pic->get_data_buffer(), pic_len);
    memcpy(buffer + frame_len + pic_len, "\r\n", footer_len);
    
    return total_len;
  }

  void handle_camera_page_(AsyncWebServerRequest *request) {
    const char html[] = R"html(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32 Camera</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body {
      font-family: Arial, sans-serif;
      margin: 0;
      padding: 20px;
      background-color: #f0f0f0;
      text-align: center;
    }
    h1 {
      color: #333;
    }
    .container {
      max-width: 800px;
      margin: 0 auto;
      background: white;
      padding: 20px;
      border-radius: 10px;
      box-shadow: 0 2px 10px rgba(0,0,0,0.1);
    }
    img {
      max-width: 100%;
      height: auto;
      border: 2px solid #ddd;
      border-radius: 5px;
    }
    .buttons {
      margin-top: 20px;
    }
    button {
      padding: 10px 20px;
      margin: 5px;
      font-size: 16px;
      cursor: pointer;
      background-color: #4CAF50;
      color: white;
      border: none;
      border-radius: 5px;
    }
    button:hover {
      background-color: #45a049;
    }
    .mode-toggle {
      background-color: #2196F3;
    }
    .mode-toggle:hover {
      background-color: #0b7dda;
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>ESP32 Camera Web Server</h1>
    <img id="camera-img" src="/snapshot.jpg" alt="Camera Feed">
    <div class="buttons">
      <button onclick="toggleMode()">Toggle Stream/Snapshot</button>
      <button onclick="captureSnapshot()">Refresh Snapshot</button>
    </div>
  </div>
  <script>
    let streamMode = false;
    let refreshInterval;

    function toggleMode() {
      streamMode = !streamMode;
      const img = document.getElementById('camera-img');
      
      if (streamMode) {
        clearInterval(refreshInterval);
        img.src = '/stream';
      } else {
        img.src = '/snapshot.jpg?' + new Date().getTime();
        startAutoRefresh();
      }
    }

    function captureSnapshot() {
      const img = document.getElementById('camera-img');
      img.src = '/snapshot.jpg?' + new Date().getTime();
    }

    function startAutoRefresh() {
      refreshInterval = setInterval(() => {
        if (!streamMode) {
          captureSnapshot();
        }
      }, 1000);
    }

    startAutoRefresh();
  </script>
</body>
</html>
)html";

    request->send(200, "text/html", html);
  }

  void serve_placeholder_(AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse_P(
        200, "image/jpeg", PLACEHOLDER_JPEG, PLACEHOLDER_JPEG_SIZE);
    response->addHeader("Content-Disposition", "inline; filename=placeholder.jpg");
    response->addHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    request->send(response);
  }

  ESP32Camera *camera_;
};

}  // namespace esp32_camera
}  // namespace esphome
