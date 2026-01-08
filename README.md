# ESP32 Camera Web Server with Placeholder

An ESPHome custom component that extends the standard `esp32_camera_web_server` to show a placeholder image when the camera is unavailable or powered down.

## Features

✅ **Automatic fallback** - Shows placeholder image when camera is unavailable  
✅ **Seamless switching** - Automatically switches back to live feed when camera recovers  
✅ **Stream & Snapshot modes** - Works in both modes  
✅ **Configurable** - Can enable/disable placeholder functionality  
✅ **Low timeout** - Quick detection of camera unavailability (1 second vs 5 seconds)  

## Installation

Add this as an external component in your ESPHome configuration:

```yaml
external_components:
  - source: github://nickoe/esphome-esp32-camera-web-server-placeholder
    components: [ esp32_camera_web_server_placeholder ]
```

## Configuration

```yaml
esp32_camera:
  id: my_camera
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
  resolution: 640x480
  jpeg_quality: 10

esp32_camera_web_server_placeholder:
  port: 8080
  mode: stream
  placeholder_enabled: true
```

## Configuration Variables

- **port** (*Required*, int): The port the web server should listen on
- **mode** (*Required*, string): Either `stream` or `snapshot`
- **placeholder_enabled** (*Optional*, boolean): Enable/disable placeholder image. Defaults to `true`

## Usage with Power Control

```yaml
switch:
  - platform: gpio
    pin: GPIO4
    name: "Camera Power"
    id: camera_power
    
button:
  - platform: template
    name: "Restart Camera"
    on_press:
      - switch.turn_off: camera_power
      - delay: 500ms
      - switch.turn_on: camera_power
      - logger.log: "Camera reinitialized"
```

## License

MIT License

## Credits

Based on the original `esp32_camera_web_server` component from ESPHome.