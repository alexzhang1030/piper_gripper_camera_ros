#include "piper_gripper_camera/piper_gripper_camera_node.hpp"

#include <chrono>

#include "cv_bridge/cv_bridge.h"
#include "sensor_msgs/msg/image.hpp"

#include <opencv2/imgproc.hpp>

namespace piper_gripper_camera
{

PiperGripperCameraNode::PiperGripperCameraNode(const rclcpp::NodeOptions & options)
: rclcpp::Node("piper_gripper_camera", options)
{
  camera_name_     = declare_parameter<std::string>("camera_name", "piper_gripper_camera");
  frame_id_        = declare_parameter<std::string>("frame_id", "piper_gripper_camera_frame");
  device_path_     = declare_parameter<std::string>("device", "/dev/video0");
  image_width_     = declare_parameter<int>("image_width", 640);
  image_height_    = declare_parameter<int>("image_height", 480);
  fps_             = declare_parameter<double>("fps", 30.0);
  std::string camera_info_url = declare_parameter<std::string>("camera_info_url", "");

  // Support both /dev/videoX path and integer index
  cap_ = std::make_unique<cv::VideoCapture>(device_path_, cv::CAP_V4L2);
  if (!cap_->isOpened()) {
    RCLCPP_ERROR(get_logger(), "Cannot open camera device: %s", device_path_.c_str());
    throw std::runtime_error("Failed to open camera device");
  }
  cap_->set(cv::CAP_PROP_FRAME_WIDTH,  image_width_);
  cap_->set(cv::CAP_PROP_FRAME_HEIGHT, image_height_);
  cap_->set(cv::CAP_PROP_FPS, fps_);

  cinfo_manager_ = std::make_unique<camera_info_manager::CameraInfoManager>(
    this, camera_name_, camera_info_url);

  // Use image_transport — compressed topic (/image_raw/compressed) is provided
  // automatically by the image_transport_plugins at runtime (default JPEG quality: 95).
  // Tune at runtime: ros2 param set /piper_gripper_camera image_raw/compressed/jpeg_quality 95
  image_pub_       = image_transport::create_publisher(this, "~/image_raw");
  camera_info_pub_ = create_publisher<sensor_msgs::msg::CameraInfo>("~/camera_info", 10);

  running_ = true;
  capture_thread_ = std::thread(&PiperGripperCameraNode::capture_loop, this);

  RCLCPP_INFO(get_logger(),
    "Piper gripper camera started (device=%s, %dx%d @ %.0ffps)",
    device_path_.c_str(), image_width_, image_height_, fps_);
}

PiperGripperCameraNode::~PiperGripperCameraNode()
{
  running_ = false;
  if (capture_thread_.joinable()) {
    capture_thread_.join();
  }
}

void PiperGripperCameraNode::capture_loop()
{
  cv::Mat frame;
  while (running_ && rclcpp::ok()) {
    if (!cap_->read(frame) || frame.empty()) {
      RCLCPP_WARN_THROTTLE(get_logger(), *get_clock(), 2000, "Failed to grab frame");
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      continue;
    }

    auto stamp = now();

    auto img_msg = cv_bridge::CvImage(
      std_msgs::msg::Header{}, sensor_msgs::image_encodings::BGR8, frame).toImageMsg();
    img_msg->header.stamp    = stamp;
    img_msg->header.frame_id = frame_id_;
    image_pub_.publish(*img_msg);

    auto ci = cinfo_manager_->getCameraInfo();
    ci.header.stamp    = stamp;
    ci.header.frame_id = frame_id_;
    camera_info_pub_->publish(ci);
  }
}

}  // namespace piper_gripper_camera

#include "rclcpp_components/register_node_macro.hpp"
RCLCPP_COMPONENTS_REGISTER_NODE(piper_gripper_camera::PiperGripperCameraNode)

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<piper_gripper_camera::PiperGripperCameraNode>());
  rclcpp::shutdown();
  return 0;
}
