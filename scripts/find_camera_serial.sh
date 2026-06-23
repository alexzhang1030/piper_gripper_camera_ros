#!/usr/bin/env bash
# List all V4L2 video devices with their USB serial numbers.
set -euo pipefail

echo "V4L2 devices and USB serial numbers:"
echo "-------------------------------------"
for dev in /dev/video*; do
  serial=$(udevadm info --query=property --name="$dev" 2>/dev/null | grep '^ID_SERIAL=' | cut -d= -f2)
  model=$(udevadm info --query=property --name="$dev" 2>/dev/null | grep '^ID_MODEL=' | cut -d= -f2)
  vendor=$(udevadm info --query=property --name="$dev" 2>/dev/null | grep '^ID_VENDOR=' | cut -d= -f2)
  if [ -n "$serial" ]; then
    echo "  $dev  vendor=$vendor  model=$model  serial=$serial"
  fi
done

echo ""
echo "Copy the serial into udev/99-piper-gripper-camera.rules, then:"
echo "  sudo cp udev/99-piper-gripper-camera.rules /etc/udev/rules.d/"
echo "  sudo udevadm control --reload-rules && sudo udevadm trigger"
echo "  # Camera will be available at /dev/piper_gripper_cam"
