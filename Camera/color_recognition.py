from picamera2 import Picamera2
import cv2
import numpy as np
import time

class ColorRecognizer:
    def __init__(self, width=1280, height=720):
        self.picam2 = Picamera2()
        self.picam2.preview_configuration.main.size = (width, height)
        self.picam2.preview_configuration.main.format = "BGR888"
        self.picam2.configure("preview")
        self.picam2.start()
        time.sleep(1)

    def get_color(self, x, y):
        frame = self.picam2.capture_array()
        frame = cv2.cvtColor(frame, cv2.COLOR_RGB2BGR)
        hsv_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
        height, width, _ = hsv_frame.shape

        if x < 2 or y < 2 or x > width - 3 or y > height - 3:
            return "Out of Bounds"

        region = hsv_frame[y - 2:y + 3, x - 2:x + 3]
        h_values = region[:, :, 0].flatten()
        unique, counts = np.unique(h_values, return_counts=True)
        mode_h = unique[np.argmax(counts)]

        if mode_h < 5 or mode_h > 165:
            return "Red"
        elif 6 <= mode_h < 36:
            return "Yellow"
        elif 37 <= mode_h < 96:
            return "Green"
        elif 96 <= mode_h < 131:
            return "Blue"
        elif 131 <= mode_h < 164:
            return "Purple"
        else:
            return "Unknown"

    def stop(self):
        self.picam2.stop()
