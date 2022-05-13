import numpy as np
import cv2

def draw_rectangles(cv2, frame, rect_size, thickness, color):
    space = 20
    rectangles = []
    height, width, channels = frame.shape
    
    start_x = int(width/2 - 2*rect_size - 1.5*space)
    for i in range(4):
        start_point = (start_x + int(i*(rect_size + space)), int(height/2 - rect_size/2))
        end_point = (start_x + rect_size + int(i*(rect_size + space)), int(height/2 + rect_size/2))
        frame = cv2.rectangle(frame, start_point, end_point, color, thickness)
        rectangles.append([start_point, end_point])
    return frame, rectangles 

def check_rectangle(frame, hsv_frame, color_number, rectangle, rect_size, thickness, color):
    h_sensitivity = 20
    s_h = 255
    v_h = 255
    s_l = 50
    v_l = 50

    start_point = rectangle[0]
    end_point = rectangle[1]
    
    color_upper = np.array([color_number + h_sensitivity, s_h, v_h])
    color_lower = np.array([color_number - h_sensitivity, s_l, v_l])
    mask_frame = hsv_frame[start_point[1]:end_point[1] + 1, start_point[0]:end_point[0] + 1]
    mask_color = cv2.inRange(mask_frame, color_lower, color_upper)

    color_rate = np.count_nonzero(mask_color)/(rect_size*rect_size)

    h_space = 20
    org = (start_point[0], end_point[1] + h_space )
    font = cv2.FONT_HERSHEY_SIMPLEX
    fontScale = 0.5
	
    if color_rate > 0.9:
        valid = True
    else:
        frame = cv2.putText(frame, 'Wrong color', org, font, fontScale, color, thickness, cv2.LINE_AA)
        valid = False
    return frame, valid

def check_rectangles(frame, color_for_rectangles, rectangles, rect_size, thickness, color):
    hsv_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    all_valid = True
    for i in range(4):
        frame, valid = check_rectangle(frame, hsv_frame, color_for_rectangles[i], rectangles[i], rect_size, thickness, color)
        if not valid:
            all_valid = False
    return frame, all_valid

def process(frame):
    color_for_rectangles = [120, 120, 120, 120]
    validation = False
    rect_size = 100
    thickness = 2
    color = (255, 0 , 0)

    # Draw squares
    frame, rectangles = draw_rectangles(cv2, frame, rect_size, thickness, color)

    modified_frame, validation = check_rectangles(frame, color_for_rectangles, rectangles, rect_size, thickness, color)
    return modified_frame, validation


def main():

    print('Press Esc to Quit the Application\n')

    #Open Default Camera
    cap = cv2.VideoCapture(0) 

    while(cap.isOpened()):
        #Take each Frame
        ret, frame = cap.read()

        #Flip Video vertically (180 Degrees)
        frame = cv2.flip(frame, 180)

        modified_frame, validation = process(frame)
        if(validation):
            print("Validation passed! Welcome :)")
            break
        # Show video
        cv2.imshow('Login window', modified_frame)

        # Exit if "Esc" is pressed
        k = cv2.waitKey(1) & 0xFF
        if k == 27 : #Word Esc
            #Quit
            print ('Good Bye!')
            break

    #Release the Cap and Video
    cap.release()
    cv2.destroyAllWindows()

main()