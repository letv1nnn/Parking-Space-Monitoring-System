# Document Scanner

# Description
This project implements a document scanner using OpenCV in C++. It detects a document in an input image, identifies its corners, and applies a perspective warp to produce a flattened, top-down scanned version.

## Requirements

- CMake ≥ 3.10
- C++ compiler with C++20 support
- OpenCV (version 4 or higher)

Ensure OpenCV is installed and accessible via `pkg-config` or `CMAKE_PREFIX_PATH`.

## Build
Clone
```bash
git clone git@github.com:letv1nnn/Computer-Vision.git
cd Computer-Vision/Projects/Document-Scanner/
```
Build
```bash
mkdir -p build
cd build
cmake ..
make
./DocumentScanner
cd .. 
```

Running
```bash
./build/DocumentScanner # One additional argument, file path
```

Clean
```bash
rm -rf build
```

## Notes

- If no image path is provided, it defaults to `test/paper.jpg`, so you can primarily test the program.
- The build process copies the `test` folder into the build directory after compilation for ease of access.
- Press any key (except 'q') to close the OpenCV window. Press 'q' to quit from the terminal after viewing the image.

## Example
<img width="1920" height="990" alt="Screenshot from 2025-07-26 17-45-17" src="https://github.com/user-attachments/assets/510873fb-449a-468a-af04-d189643d7394" />


