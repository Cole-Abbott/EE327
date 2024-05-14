import tornado.ioloop
import tornado.web
import tornado.websocket
import socket

import image


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

    return image, x_avg, y_avg, count



# create two lists to store the clients
website_clients = []
esp_clients = []

# MainHandler class to handle the http request
class MainHandler(tornado.web.RequestHandler):
    def get(self):
        self.render("website/index.html")



# WebSocketHandler class to handle the websocket connection from the website
class WebSocketHandler(tornado.websocket.WebSocketHandler):
    # when the connection is opened, add the client to the list
    def open(self):
        print("WebSocket connection opened")
        website_clients.append(self)

    # on message received from the website, echo the message back to the website and send it to the ESP32 clients
    def on_message(self, message):
        self.write_message("Received message: " + message)
        # todo send message to esp clients
        print("Received website message: " + message)
        print(f"Sending message to { len(esp_clients)} ESP32 clients: ")

        # check if the clients are still connected and remove them if not
        for client in esp_clients:
            try:
                client.write_message(message)
            except:
                print("Client not connected")
                esp_clients.remove(client)

    

    # on close connection remove the client from the list
    def on_close(self):
        print("WebSocket connection closed")
        website_clients.remove(self)


# ESPHandler class to handle the websocket connection from the ESP32
class ESPHandler(tornado.websocket.WebSocketHandler):
    def check_origin(self, origin):
        return True

    # on open connection and add the client to the list
    def open(self):
        print("ESP32 webSocket opened")
        esp_clients.append(self)

    # on message received from the ESP32, echo the message back to the ESP32
    def on_message(self, message):
        # write the message to an image file

        # convert the message to an image



        nparr = np.frombuffer(message, np.uint8)
        image = cv2.imdecode(nparr, cv2.IMREAD_COLOR)
        # rotate the image 180 degrees
        image = cv2.rotate(image, cv2.ROTATE_180)

        image, x, y, count = detect_person(image)
        if count > 0:
            self.write_message(f"x: {x}, y: {y}")

        # convert the image back to bytes
        _, image = cv2.imencode('.jpg', image)

        # convert image to bytes that can be sent over the websocket
        image = image.tobytes()

        # send the image to the website
        for client in website_clients:
            try:
                client.write_message(image, binary=True)
                # client.write_message(message, binary=True)
            except:
                print("Client not connected")
                website_clients.remove(client)
       

    # on close connection remove the client from the list
    def on_close(self):
        print("ESP websocket connection closed")
        esp_clients.remove(self)


def make_app():
    return tornado.web.Application([
        (r"/", MainHandler),
        (r"/websocket", WebSocketHandler),
        (r"/esp32", ESPHandler),
        (r"/(.*)", tornado.web.StaticFileHandler, {"path": "website"}),
    ])


if __name__ == "__main__":
    app = make_app()
    app.listen(8888)
    # print out the IP address of the server
    ip = socket.gethostbyname(socket.gethostname())
    print("Server started on http://" + ip + ":8888")
    tornado.ioloop.IOLoop.current().start()
