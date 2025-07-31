# Free Parking Slot Detection

This C++ project uses OpenCV to automatically detect free parking slots from a static camera feed or video footage. It highlights occupied and free parking spaces with colored rectangles and displays the number of available slots on screen.

---

## How It Works
1. **Input Source Selection**  
   The user can choose between using a video file or a real-time camera stream as the input source.

2. **One-Time Calibration**  
   During calibration mode, the user manually marks the top-left corner of each parking slot by clicking on the video feed. These coordinates are saved to a `points.txt` file for future sessions. Calibration is only required once.

3. **Slot Detection Process**  
   After loading the saved slot positions:
   - The program extracts each parking region as a rectangular ROI.
   - It applies a preprocessing pipeline: grayscale conversion, Gaussian blur, and Canny edge detection.
   - The system calculates the white pixel ratio of each preprocessed ROI and compares it to a predefined threshold to determine whether a spot is free or occupied.

---

## Requirements

- C++20
- OpenCV 4.x
- CMake 3.10+

---

## Clonning
git clone https://github.com/letv1nnn/Free-Parking-Slot-Detector.git
cd Free-Parking-Slot-Detector

## Building
```bash
mkdir build
cd build
cmake ..
make
cd ..
```

---

## Examples

Initially, I've created a program that detect free spots from the given immage, not from capturing a video.

---
### Free spots detection from image
<img width="1920" height="1080" alt="Screenshot from 2025-07-30 19-14-20" src="https://github.com/user-attachments/assets/7a615b45-fd34-4db6-8aec-a25bde2914fb" />

### Detection from video/camera
<img width="1920" height="1080" alt="Screenshot from 2025-07-31 17-09-06" src="https://github.com/user-attachments/assets/7eca30ca-1bcb-408d-ade3-e0d2ad0744ab" />

