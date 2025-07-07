import cv2
from PIL import Image

cap = cv2.VideoCapture(0)
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 1280)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 720)

while True:
    ret, frame = cap.read()
    if not ret:
        break

    hsvImage = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    # Színtartományok HSV-ben
    lower_green = (70, 170, 70)
    upper_green = (90, 235, 105)

    lower_blue = (105, 180, 55)
    upper_blue = (120, 235, 100)

    # Maszkok
    mask_green = cv2.inRange(hsvImage, lower_green, upper_green)
    mask_blue = cv2.inRange(hsvImage, lower_blue, upper_blue)

    # PIL Image-re alakítás és dobozok
    bbox_green = Image.fromarray(mask_green).getbbox()
    bbox_blue = Image.fromarray(mask_blue).getbbox()

    if bbox_green is not None:
        x1, y1, x2, y2 = bbox_green
        frame = cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 255, 0), 5)

    if bbox_blue is not None:
        x1, y1, x2, y2 = bbox_blue
        frame = cv2.rectangle(frame, (x1, y1), (x2, y2), (255, 0, 0), 5)

    cv2.imshow("Frame", frame)

    if cv2.waitKey(1) & 0xFF == 27:  # ESC kilépés
        break

cap.release()
cv2.destroyAllWindows()