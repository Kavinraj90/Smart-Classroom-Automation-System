import cv2
import serial

# Update to your port, e.g., 'COM3' or '/dev/ttyUSB0'
ser = serial.Serial('COM3', 115200, timeout=1)
face_cascade = cv2.CascadeClassifier(cv2.data.haarcascades + 'haarcascade_frontalface_default.xml')
cap = cv2.VideoCapture(0)

while True:
    ret, frame = cap.read()
    if not ret: break
    
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    faces = face_cascade.detectMultiScale(gray, 1.1, 4)
    
    l_count, r_count = 0, 0
    mid = frame.shape[1] // 2
    
    for (x, y, w, h) in faces:
        if x < mid: l_count += 1
        else: r_count += 1
        cv2.rectangle(frame, (x, y), (x+w, y+h), (0, 255, 0), 2)

    # Format: L#R# (e.g., L2R1)
    msg = f"L{l_count}R{r_count}\n"
    ser.write(msg.encode())
    
    cv2.imshow('Detection', frame)
    if cv2.waitKey(1) == ord('q'): break

cap.release()
cv2.destroyAllWindows()