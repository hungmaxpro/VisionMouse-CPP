# VisionMouse-CPP: AI-Powered Hand Gesture Controller

## Overview
VisionMouse is a high-performance computer vision project built with C++  . It enables users to control their PC's cursor and execute commands using real-time hand gestures. Unlike common Python implementations, this project focuses on low-latency execution and memory efficiency , making it suitable for Edge AI environments.

## Key Features
- Real-time Hand Tracking: Leveraging MediaPipe's hand landmark detection.
- Precision Cursor Mapping:Smoothly maps camera coordinates to screen resolution.
- Advanced Smoothing: Implements a Moving Average Filter (DSA-based) to eliminate hand jitter.
- Gesture Shortcuts: - Index Finger Move: Move Cursor.
    -  Pinch (Thumb + Index) : Left Click.
    -  Two Fingers Up : Scroll Mode.

## Tech Stack
- Language: C++17
- Vision: OpenCV
- AI Framework:MediaPipe (C++ API)
- OS Interface: Windows API (User32.lib)

 Developed as part of my journey to master Edge AI and Computer Vision. 
