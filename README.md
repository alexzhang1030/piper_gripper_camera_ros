# piper_gripper_camera

ROS 2 (Jazzy) node for the Piper gripper USB camera, publishing `sensor_msgs/Image` and `sensor_msgs/CameraInfo`.

## Build

```bash
colcon build --packages-select piper_gripper_camera
```

## Run

```bash
ros2 launch piper_gripper_camera piper_gripper_camera.launch.py device_id:=0
```

## Published Topics

| Topic | Type | Description |
|-------|------|-------------|
| `~/image_raw` | `sensor_msgs/Image` | Raw BGR8 frames |
| `~/image_raw/compressed` | `sensor_msgs/CompressedImage` | JPEG-compressed frames |
| `~/camera_info` | `sensor_msgs/CameraInfo` | Camera calibration info |

## Parameters

| Parameter | Default | Description |
|-----------|---------|-------------|
| `device_id` | `0` | `/dev/video<N>` index |
| `image_width` | `640` | Capture width (px) |
| `image_height` | `480` | Capture height (px) |
| `fps` | `30.0` | Target frame rate |
| `jpeg_quality` | `80` | JPEG quality for `~/image_raw/compressed` (1–100) |
| `camera_info_url` | `""` | `file:///path/to/camera_info.yaml` |
| `frame_id` | `piper_gripper_camera_frame` | TF frame id |

## Camera Calibration

```bash
ros2 run camera_calibration cameracalibrator \
  --size 8x6 --square 0.025 \
  image:=/piper_gripper_camera/image_raw
```

Save the result and pass the path via `camera_info_url:=file:///path/to/ost.yaml`.
