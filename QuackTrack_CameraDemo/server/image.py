import cv2
import numpy as np

# Load YOLO
net = cv2.dnn.readNet('yolov3.weights', 'yolov3.cfg')
# net = cv2.dnn.readNetFromDarknet("yolo/yolov3.cfg" ,"yolo/yolov3.weights" )
layer_names = net.getLayerNames()
output_layers = [layer_names[i-1] for i in net.getUnconnectedOutLayers()]

# detects a person in the image and draws a rectangle around them
def detect_person(image):
    # Get image shape
    height, width, channels = image.shape

    # Create blob and do forward pass
    blob = cv2.dnn.blobFromImage(
        image, 0.00392, (416, 416), (0, 0, 0), True, crop=False)
    net.setInput(blob)
    outs = net.forward(output_layers)

    # average all the boxes and draw a dot in the center
    x_avg = 0
    y_avg = 0
    count = 0

    # Information for each object detected
    for out in outs:
        for detection in out:
            scores = detection[5:]
            class_id = np.argmax(scores)
            confidence = scores[class_id]
            if confidence > 0.5 and class_id == 0:  # Class ID 0 is human
                # Object detected
                center_x = int(detection[0] * width)
                center_y = int(detection[1] * height)
                w = int(detection[2] * width)
                h = int(detection[3] * height)

                # Rectangle coordinates
                x = int(center_x - w / 2)
                y = int(center_y - h / 2)
                x_avg += center_x
                y_avg += center_y
                count += 1

                cv2.rectangle(image, (x, y), (x + w, y + h), (0, 255, 255), 2)

    # Draw a dot in the center
    if count > 0:
        x_avg = int(x_avg / count)
        y_avg = int(y_avg / count)
        cv2.circle(image, (x_avg, y_avg), 10, (0, 0, 255), -1)

    return image, x_avg, y_avg