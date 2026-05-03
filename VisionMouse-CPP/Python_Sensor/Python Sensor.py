import cv2
import mediapipe as mp
import socket
import json

# Cấu hình Socket UDP để gửi data sang C++
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
server_address = ('127.0.0.1', 5005)

mp_hands = mp.solutions.hands
hands = mp_hands.Hands(static_image_mode=False, max_num_hands=1, min_detection_confidence=0.7)

cap = cv2.VideoCapture(0)

while cap.isOpened():
    success, image = cap.read()
    if not success: break

    image = cv2.flip(image, 1)
    results = hands.process(cv2.cvtColor(image, cv2.COLOR_BGR2RGB))
    
    if results.multi_hand_landmarks:
        for hand_landmarks in results.multi_hand_landmarks:
            # Lấy tọa độ ngón 
            index_finger = hand_landmarks.landmark[8]
            below_finger = hand_landmarks.landmark[6]
            data = {"x": index_finger.x, "y": index_finger.y}
            
            
            # Ném tọa độ sang C++
            sock.sendto(json.dumps(data).encode(), server_address)
            mp.solutions.drawing_utils.draw_landmarks(image, hand_landmarks, mp_hands.HAND_CONNECTIONS)
    cv2.imshow('AI Sensor (Python)', image)
    if cv2.waitKey(1) & 0xFF == ord('q'): break

cap.release()