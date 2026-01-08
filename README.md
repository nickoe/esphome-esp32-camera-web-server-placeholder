# ESP32 Camera Web Server with Placeholder

An ESPHome component that extends the standard ESP32 camera web server functionality with placeholder image support. This component displays a placeholder image when the camera is powered off or unavailable, providing a seamless user experience and preventing broken image links.

## Features

- 🎥 **Full ESP32 Camera Web Server**: All standard ESPHome camera web server features
- 🖼️ **Placeholder Image Support**: Display a custom placeholder when camera is unavailable
- ⚡ **Power Control Integration**: Automatically show placeholder when camera power is off
- 🔄 **Seamless Switching**: Smooth transition between live feed and placeholder
- 🌐 **Web Server Integration**: Works with ESPHome's built-in web server
- 📱 **Home Assistant Compatible**: Fully compatible with Home Assistant camera entities
- 🔧 **Easy Configuration**: Simple YAML configuration
- 💾 **Memory Efficient**: Optimized for ESP32 memory constraints

## Requirements

- ESP32 board with camera support (ESP32-CAM, M5Stack Camera, etc.)
- ESPHome 2023.4.0 or later
- Camera module (OV2640, OV3660, etc.)

## Installation

### Method 1: External Component (Recommended)

Add this component as an external component in your ESPHome configuration:

```yaml
external_components:
  - source: github://nickoe/esphome-esp32-camera-web-server-placeholder
    components: [ esp32_camera_web_server_placeholder ]
```

### Method 2: Local Component

1. Clone this repository into your ESPHome `custom_components` directory:
   ```bash
   cd /config/esphome
   mkdir -p custom_components
   cd custom_components
   git clone https://github.com/nickoe/esphome-esp32-camera-web-server-placeholder.git
   ```

2. Reference it in your configuration:
   ```yaml
   external_components:
     - source: custom_components/esphome-esp32-camera-web-server-placeholder
       components: [ esp32_camera_web_server_placeholder ]
   ```

## Configuration

### Basic Configuration

```yaml
esphome:
  name: esp32-cam
  platform: ESP32
  board: esp32cam

wifi:
  ssid: !secret wifi_ssid
  password: !secret wifi_password

# Enable logging
logger:

# Enable Home Assistant API
api:

# Enable OTA updates
ota:

# Enable web server
web_server:
  port: 80

# Configure the camera
esp32_camera:
  id: esp_camera
  name: "ESP32 Camera"
  external_clock:
    pin: GPIO0
    frequency: 20MHz
  i2c_pins:
    sda: GPIO26
    scl: GPIO27
  data_pins: [GPIO5, GPIO18, GPIO19, GPIO21, GPIO36, GPIO39, GPIO34, GPIO35]
  vsync_pin: GPIO25
  href_pin: GPIO23
  pixel_clock_pin: GPIO22
  power_down_pin: GPIO32
  resolution: 1024x768
  jpeg_quality: 10

# Camera web server with placeholder
esp32_camera_web_server_placeholder:
  camera_id: esp_camera
  port: 8080
  placeholder_image: "/config/esphome/placeholder.jpg"  # Optional
```

### Advanced Configuration with Power Control

This example shows how to use the component with GPIO-controlled camera power:

```yaml
# Power control switch
switch:
  - platform: gpio
    pin: GPIO33
    name: "Camera Power"
    id: camera_power
    restore_mode: ALWAYS_ON
    on_turn_on:
      - delay: 100ms
      - lambda: |-
          id(esp_camera).update();
    on_turn_off:
      - lambda: |-
          // Camera will show placeholder when powered off

# Camera configuration with power control
esp32_camera:
  id: esp_camera
  name: "ESP32 Camera"
  external_clock:
    pin: GPIO0
    frequency: 20MHz
  i2c_pins:
    sda: GPIO26
    scl: GPIO27
  data_pins: [GPIO5, GPIO18, GPIO19, GPIO21, GPIO36, GPIO39, GPIO34, GPIO35]
  vsync_pin: GPIO25
  href_pin: GPIO23
  pixel_clock_pin: GPIO22
  power_down_pin: GPIO32
  resolution: 1280x1024
  jpeg_quality: 10
  max_framerate: 10 fps
  idle_framerate: 0.1 fps

# Camera web server with placeholder
esp32_camera_web_server_placeholder:
  camera_id: esp_camera
  port: 8080
  power_switch_id: camera_power  # Link to power switch
  placeholder_image: "/config/esphome/camera_offline.jpg"
  placeholder_text: "Camera is powered off"  # Optional text overlay
```

### Configuration with Automation

```yaml
# Automate camera power based on motion or time
binary_sensor:
  - platform: gpio
    pin: GPIO13
    name: "Motion Sensor"
    device_class: motion
    on_press:
      - switch.turn_on: camera_power
      - delay: 5min
      - switch.turn_off: camera_power

# Time-based automation
time:
  - platform: homeassistant
    id: homeassistant_time

interval:
  - interval: 1h
    then:
      - if:
          condition:
            lambda: 'return id(homeassistant_time).now().hour >= 22 || id(homeassistant_time).now().hour < 6;'
          then:
            - switch.turn_off: camera_power
          else:
            - switch.turn_on: camera_power
```

## Configuration Options

| Option | Type | Required | Default | Description |
|--------|------|----------|---------|-------------|
| `camera_id` | ID | Yes | - | ID of the ESP32 camera component |
| `port` | int | No | 8080 | Port for the camera web server |
| `placeholder_image` | string | No | Built-in | Path to custom placeholder image (JPEG) |
| `placeholder_text` | string | No | "" | Text to display on placeholder |
| `power_switch_id` | ID | No | - | ID of switch controlling camera power |
| `update_interval` | time | No | 100ms | Camera update interval when active |
| `idle_update_interval` | time | No | 1s | Camera update interval when idle |

## Usage

### Accessing the Camera Feed

Once configured and flashed to your ESP32:

1. **Web Browser**: Navigate to `http://<device-ip>:8080` (or your configured port)
2. **Home Assistant**: The camera entity will automatically appear and work with the placeholder feature
3. **RTSP Clients**: Compatible with VLC, FFmpeg, and other RTSP clients

### Power Control Integration

When integrated with a power switch:
- **Camera ON**: Live camera feed is displayed
- **Camera OFF**: Placeholder image is displayed automatically
- **Seamless Transition**: No broken images or error messages

### Example Home Assistant Automation

```yaml
automation:
  - alias: "Camera Power Save"
    trigger:
      - platform: state
        entity_id: binary_sensor.living_room_motion
        to: "off"
        for:
          minutes: 10
    action:
      - service: switch.turn_off
        entity_id: switch.camera_power

  - alias: "Camera Power On Motion"
    trigger:
      - platform: state
        entity_id: binary_sensor.living_room_motion
        to: "on"
    action:
      - service: switch.turn_on
        entity_id: switch.camera_power
```

## Troubleshooting

### Camera Not Showing Image

**Problem**: Web server shows error or blank page

**Solutions**:
1. Check camera is properly connected:
   ```bash
   # In ESPHome logs
   [C][esp32_camera:xxx]: ESP32 Camera:
   [C][esp32_camera:xxx]:   State: STREAMING
   ```
2. Verify GPIO pins match your board's camera pinout
3. Check power supply provides sufficient current (min 500mA recommended)
4. Reduce resolution or JPEG quality if experiencing memory issues

### Placeholder Not Displaying

**Problem**: Placeholder image doesn't show when camera is off

**Solutions**:
1. Verify placeholder image path is correct
2. Ensure image is in JPEG format
3. Check image size is appropriate (recommended < 50KB)
4. Verify `power_switch_id` is correctly linked

### Memory Issues

**Problem**: ESP32 crashes or restarts during operation

**Solutions**:
1. Reduce camera resolution:
   ```yaml
   resolution: 800x600  # or lower
   ```
2. Increase JPEG quality number (lower quality, less memory):
   ```yaml
   jpeg_quality: 15  # Higher number = lower quality
   ```
3. Reduce framerate:
   ```yaml
   max_framerate: 5 fps
   ```
4. Enable PSRAM if your board supports it:
   ```yaml
   esphome:
     platformio_options:
       board_build.arduino.memory_type: qio_qspi
   ```

### Connection Issues

**Problem**: Cannot connect to web server

**Solutions**:
1. Verify device is connected to WiFi:
   ```bash
   # Check ESPHome logs for IP address
   [I][wifi:xxx]: WiFi connected!
   [C][wifi:xxx]:   IP Address: 192.168.1.xxx
   ```
2. Check firewall settings allow access to configured port
3. Ensure port isn't already in use by another service
4. Try accessing via IP address instead of hostname

### Image Quality Issues

**Problem**: Image is blurry or has artifacts

**Solutions**:
1. Adjust JPEG quality (lower number = better quality):
   ```yaml
   jpeg_quality: 8
   ```
2. Check camera lens focus
3. Verify camera is receiving stable power
4. Adjust brightness and contrast settings:
   ```yaml
   esp32_camera:
     # ... other settings ...
     brightness: 1
     contrast: 1
     saturation: 0
   ```

### Performance Optimization

**Problem**: Slow frame rate or laggy stream

**Solutions**:
1. Reduce resolution for better performance
2. Adjust framerate settings:
   ```yaml
   max_framerate: 10 fps
   idle_framerate: 0.2 fps
   ```
3. Use wired Ethernet if possible (with Ethernet-capable boards)
4. Ensure strong WiFi signal
5. Disable unnecessary ESPHome components

### Boot Loop After Flash

**Problem**: Device continuously restarts

**Solutions**:
1. Check power supply is adequate (2A recommended)
2. Verify all GPIO pins are correctly configured
3. Try reducing initial resolution
4. Check for GPIO conflicts with other components
5. Erase flash and re-flash:
   ```bash
   esphome run <config>.yaml --no-logs
   ```

## Hardware Recommendations

### Recommended ESP32-CAM Boards
- **AI-Thinker ESP32-CAM**: Most common, well-supported
- **M5Stack ESP32 Camera**: Built-in PSRAM, easier power management
- **TTGO T-Camera**: Integrated battery, display options
- **Freenove ESP32-WROVER CAM**: Built-in battery management

### Camera Modules
- **OV2640**: Standard choice, good quality, wide support
- **OV3660**: Higher resolution option
- **OV5640**: Best quality, requires more resources

### Power Supply
- Minimum: 5V 500mA
- Recommended: 5V 2A (especially for WiFi stability)
- Use quality USB cable (short length recommended)

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Support

- **Issues**: [GitHub Issues](https://github.com/nickoe/esphome-esp32-camera-web-server-placeholder/issues)
- **Discussions**: [GitHub Discussions](https://github.com/nickoe/esphome-esp32-camera-web-server-placeholder/discussions)
- **ESPHome**: [ESPHome Documentation](https://esphome.io)

## Credits

Built on top of ESPHome's excellent ESP32 camera integration.

## Changelog

### Version 1.0.0 (2026-01-08)
- Initial release
- Placeholder image support
- Power control integration
- Home Assistant compatibility

---

**Note**: This component is under active development. Please report any issues or feature requests on GitHub.