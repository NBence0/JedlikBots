from picamera2 import Picamera2
import cv2
import numpy as np
import time

# Inicializálás
picam2 = Picamera2()
picam2.preview_configuration.main.size = (1280, 720)
picam2.preview_configuration.main.format = "BGR888"
picam2.configure("preview")
picam2.start()
time.sleep(1)  # Rövid várakozás a kamera indulásához

while True:
    frame = picam2.capture_array()
    frame = cv2.cvtColor(frame, cv2.COLOR_RGB2BGR)
    hsv_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    height, width, _ = frame.shape

    center_x = int(width / 2)
    center_y = int(height / 2)

    region = hsv_frame[center_y-2:center_y+3, center_x-2:center_x+3]
    h_values = region[:, :, 0].flatten()

    unique, counts = np.unique(h_values, return_counts=True)
    mode_h = unique[np.argmax(counts)]

    color = "Unknown"
    if mode_h < 5 or mode_h > 165:
        color = "Red"
    elif 6 <= mode_h < 33:
        color = "Yellow"
    elif 33 <= mode_h < 96:
        color = "Green"
    elif 96 <= mode_h < 131:
        color = "Blue"
    elif 131 <= mode_h < 164:
        color = "Purple"

    cv2.putText(frame, color, (center_x, center_y + 200), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 255, 255), 2)
    cv2.circle(frame, (center_x, center_y), 5, (0, 255, 0), 3)

    cv2.imshow("Frame", frame)
    if cv2.waitKey(1) == 27:  # ESC
        break

cv2.destroyAllWindows()