# piper_gripper_camera

ROS 2 (Jazzy) node for the Piper gripper USB/V4L2 camera.

Follows the same `image_transport` pattern as [OrbbecSDK_ROS2](https://github.com/orbbec/OrbbecSDK_ROS2):
compressed transport (`~/image_raw/compressed`) is provided automatically by `image_transport_plugins` at runtime — no manual JPEG encoding. Default JPEG quality is **95** (image_transport default), tunable at runtime.

## Build

```bash
colcon build --packages-select piper_gripper_camera
```

## Bind camera to stable device path by serial number

USB cameras enumerate as `/dev/video0`, `/dev/video1`, etc. — the number changes on replug. Use udev to bind a fixed symlink:

```bash
# 1. Find your camera's serial number
scripts/find_camera_serial.sh

# 2. Edit udev/99-piper-gripper-camera.rules — replace YOUR_SERIAL_HERE

# 3. Install
sudo cp udev/99-piper-gripper-camera.rules /etc/udev/rules.d/
sudo udevadm control --reload-rules && sudo udevadm trigger
# Camera will appear as /dev/piper_gripper_cam
```

## Run

```bash
ros2 launch piper_gripper_camera piper_gripper_camera.launch.py
# or override device:
ros2 launch piper_gripper_camera piper_gripper_camera.launch.py device:=/dev/video0
```

## Topics

| Topic | Type | Description |
|-------|------|-------------|
| `~/image_raw` | `sensor_msgs/Image` | Raw BGR8 frames |
| `~/image_raw/compressed` | `sensor_msgs/CompressedImage` | JPEG (via image_transport, default quality 95) |
| `~/camera_info` | `sensor_msgs/CameraInfo` | Camera calibration info |

## Parameters

| Parameter | Default | Description |
|-----------|---------|-------------|
| `device` | `/dev/piper_gripper_cam` | V4L2 device path |
| `image_width` | `640` | Capture width (px) |
| `image_height` | `480` | Capture height (px) |
| `fps` | `30.0` | Target frame rate |
| `camera_info_url` | `""` | `file:///path/to/camera_info.yaml` |
| `frame_id` | `piper_gripper_camera_frame` | TF frame id |

### Tune JPEG quality at runtime

```bash
ros2 param set /piper_gripper_camera image_raw/compressed/jpeg_quality 95
```

## Camera calibration

```bash
ros2 run camera_calibration cameracalibrator \
  --size 8x6 --square 0.025 \
  image:=/piper_gripper_camera/image_raw
```
