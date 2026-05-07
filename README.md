# AI Virtual Mouse: Real-time Hand Gesture Control

**Overview**

A polyglot application that controls the Windows mouse cursor using hand gestures via a webcam. Python serves as the computer vision engine, while C++ acts as the high-performance controller, executing OS-level commands. The two processes communicate in real-time via UDP Sockets.

**Key Features**

Zero-Jitter Tracking: Implemented a Moving Average Filter using std::deque in C++ to smooth out webcam noise and stabilize cursor movement.

Precise Click Mechanism: Uses Euclidean distance and state-flag debouncing logic to simulate natural mouse clicks and prevent continuous "double-click" glitches.

Auto-Launch: C++ automatically triggers the Python background process via ShellExecuteA for a seamless 1-click startup.

**System Architecture**

Vision Module (Python): OpenCV reads frames; MediaPipe extracts hand landmarks and packages them into JSON.

IPC (Inter-Process Communication): Streams JSON data from Python to C++ via local UDP Socket (127.0.0.1:5005) for maximum throughput and minimum latency.

Control Module (C++): Parses the JSON stream, calculates the moving average, and uses Win32 API (SetCursorPos, mouse_event) to manipulate the system cursor dynamically.

**Challenges Solved**

Dependency Hell: Resolved C++ ABI conflicts between MediaPipe and NumPy 2.x by strictly isolating the environment to Python 3.10 and locking core dependencies.

I/O Bottlenecks: Eliminated synchronous console I/O operations in the C++ loop to keep up with the 30+ FPS UDP stream, instantly fixing severe cursor lag.

State Management: Built a robust debouncing algorithm to translate a continuous 30 FPS camera feed into distinct, single hardware clicks.

**Key Learnings**

Deep understanding of UDP Sockets for real-time Inter-Process Communication.

Direct OS manipulation and hardware event simulation using the Win32 API.

Cross-language debugging and handling runtime memory issues.
