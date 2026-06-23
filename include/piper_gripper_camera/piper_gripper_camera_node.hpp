#ifndef PIPER_GRIPPER_CAMERA__PIPER_GRIPPER_CAMERA_NODE_HPP_
#define PIPER_GRIPPER_CAMERA__PIPER_GRIPPER_CAMERA_NODE_HPP_

#include <atomic>
#include <memory>
#include <string>
#include <thread>

#include "camera_info_manager/camera_info_manager.hpp"
#include "image_transport/image_transport.hpp"
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/camera_info.hpp"
#include "sensor_msgs/msg/image.hpp"

#include <opencv2/videoio.hpp>

namespace piper_gripper_camera
{

class PiperGripperCameraNode : public rclcpp::Node
{
public:
  explicit PiperGripperCameraNode(const rclcpp::NodeOptions & options = rclcpp::NodeOptions{});
  ~PiperGripperCameraNode() override;

private:
  void capture_loop();

  std::string camera_name_;
  std::string frame_id_;
  std::string device_path_;
  int image_width_;
  int image_height_;
  double fps_;

  std::unique_ptr<cv::VideoCapture> cap_;
  std::unique_ptr<camera_info_manager::CameraInfoManager> cinfo_manager_;

  image_transport::Publisher image_pub_;
  rclcpp::Publisher<sensor_msgs::msg::CameraInfo>::SharedPtr camera_info_pub_;

  std::thread capture_thread_;
  std::atomic<bool> running_{false};
};

}  // namespace piper_gripper_camera

#endif  // PIPER_GRIPPER_CAMERA__PIPER_GRIPPER_CAMERA_NODE_HPP_
