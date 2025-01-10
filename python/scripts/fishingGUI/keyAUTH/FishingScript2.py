import cv2
import numpy as np
from PIL import ImageGrab
import time
import pyautogui

def capture_screen():
   # Capture the screen and convert it to a format OpenCV can use
   screenshot = ImageGrab.grab()
   img_np = np.array(screenshot)
   img_bgr = cv2.cvtColor(img_np, cv2.COLOR_RGB2BGR)
   return img_bgr

def detect_pink_bar_and_key_area(image):
   # Convert the image to HSV color space for better color detection
   hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)

   # Define the HSV range for the pink bar and the key area
   pink_lower = np.array([140, 50, 50])
   pink_upper = np.array([170, 255, 255])
   key_lower = np.array([0, 0, 50])
   key_upper = np.array([180, 50, 200])

   # Create masks for the pink bar and the key area
   pink_mask = cv2.inRange(hsv, pink_lower, pink_upper)
   key_mask = cv2.inRange(hsv, key_lower, key_upper)

   # Find contours in the masks
   pink_contours, _ = cv2.findContours(pink_mask, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
   key_contours, _ = cv2.findContours(key_mask, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

   # Get the largest contour if available
   pink_rect = max(pink_contours, key=cv2.contourArea, default=None) if pink_contours else None
   key_rect = max(key_contours, key=cv2.contourArea, default=None) if key_contours else None

   # Convert contours to bounding rectangles
   if pink_rect is not None:
       pink_rect = cv2.boundingRect(pink_rect)
   if key_rect is not None:
       key_rect = cv2.boundingRect(key_rect)

   return pink_rect, key_rect

def is_pink_inside_key(pink_rect, key_rect):
   if not pink_rect or not key_rect:
       return False
   pink_x, pink_y, pink_w, pink_h = pink_rect
   key_x, key_y, key_w, key_h = key_rect
   # Check if the pink bar is overlapping the key area horizontally
   overlap = pink_x + pink_w > key_x and pink_x < key_x + key_w
   return overlap

def main():
   previous_pink_rect = None
   inside_key = False  # Flag to track if the pink bar is inside the key area
   inside_key_time = 0  # Timestamp of when the pink bar first enters the key area

   while True:
       screen = capture_screen()
       pink_rect, key_rect = detect_pink_bar_and_key_area(screen)

       # Optional: Display screen capture only if needed for debugging
       # Uncomment the following lines if you need to visually check the capture
       # if pink_rect:
       #     cv2.rectangle(screen, (pink_rect[0], pink_rect[1]),
       #                   (pink_rect[0] + pink_rect[2], pink_rect[1] + pink_rect[3]),
       #                   (255, 0, 255), 2)
       # if key_rect:
       #     cv2.rectangle(screen, (key_rect[0], key_rect[1]),
       #                   (key_rect[0] + key_rect[2], key_rect[1] + key_rect[3]),
       #                   (0, 255, 0), 2)
       # cv2.imshow('Screen Capture', screen)

       # Check if the pink bar is increasing in size
       if previous_pink_rect and pink_rect:
           if pink_rect[2] > previous_pink_rect[2]:
               print("Pink bar is increasing!")

       # Check if the pink bar is inside the key area
       if is_pink_inside_key(pink_rect, key_rect):
           if not inside_key:
               inside_key = True
               inside_key_time = time.time()  # Record the time when overlap first detected
               print("Pink is inside the E area!")

           # If already inside and 0.25 seconds have passed since first detection
           elif time.time() - inside_key_time >= 0.25:
               print("Pressing 'E' after 0.25 seconds.")
               pyautogui.press('e')
               time.sleep(1)  # Wait 1 second before pressing 'G'
               print("Pressing 'G' after 1 second delay.")
               pyautogui.press('g')
               inside_key = False  # Reset the flag after pressing
       else:
           inside_key = False  # Reset the flag if the pink bar is not inside the key area

       previous_pink_rect = pink_rect

       # Break the loop when 'q' is pressed
       if cv2.waitKey(1) & 0xFF == ord('q'):
           break

       # Delay to control the screen capture rate
       time.sleep(0.1)

   # Clean up windows
   cv2.destroyAllWindows()

if __name__ == "__main__":
   main()
