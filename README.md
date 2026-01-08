# ESP32 Camera Web Server with Placeholder

An ESPHome external component that extends the standard ESP32 camera web server to show a placeholder image when the camera is unavailable or powered down.

## Features

- ✅ Streams placeholder image when camera is unavailable
- ✅ Automatically switches to real camera feed when available
- ✅ Works in both STREAM and SNAPSHOT modes
- ✅ Configurable placeholder enable/disable
- ✅ Reduced timeout for faster placeholder display

## Installation

Add this to your ESPHome YAML configuration:

```yaml
external_components:
  - source: github://nickoe/esphome-esp32-camera-web-server-placeholder
    components: [ esp32_camera_web_server_placeholder ]
```

## Usage

```yaml
esp32_camera:
  id: my_camera
  name: "ESP32 Camera"
  # your camera configuration here
  
esp32_camera_web_server_placeholder:
  port: 8080
  mode: stream  # or snapshot
  placeholder_enabled: true  # optional, default is true
```

## Configuration Variables

- **port** (*Required*, int): Port number for the web server
- **mode** (*Required*, string): Either `stream` or `snapshot`
- **placeholder_enabled** (*Optional*, boolean): Enable/disable placeholder image. Defaults to `true`

## How It Works

When the camera is unavailable (powered down, failed, or not responding), the web server will:
- In **stream mode**: Continuously send a gray placeholder image at ~10fps
- In **snapshot mode**: Return the placeholder image for any request

When the camera becomes available again, it automatically switches back to the real camera feed.

## Use Cases

Perfect for:
- Camera power management scenarios
- Debugging camera issues
- Maintaining stream availability during camera reinitialization
- Providing visual feedback when camera is offline

## License

MIT License