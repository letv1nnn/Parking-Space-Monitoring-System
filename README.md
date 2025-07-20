# Face Recognition System

## Description
This project is a basic face detection system using OpenCV's Haar cascade classifiers. It detects faces and eyes from a video file and displays them with annotations.

---

## Features

- Detects **frontal cat faces** (can be swapped with human face classifiers).
- Detects **eyes** as nested features.
- Reads video input from a file.
- Draws circles or rectangles around detected faces.

---

## Requirements

- C++ Compiler (e.g., `g++`, MSVC)
- OpenCV (version 4.x or later)
- Haar Cascade XML files:
  - `haarcascade_frontalcatface.xml`
  - `haarcascade_eye_tree_eyeglasses.xml`

You can download these from the official [OpenCV GitHub](https://github.com/opencv/opencv/tree/master/data/haarcascades).

---

## How to Build and Run

### Using g++ (Linux/Mac):
```bash
g++ face_detection.cpp -o face_detection `pkg-config --cflags --libs opencv4`
./face_detection
