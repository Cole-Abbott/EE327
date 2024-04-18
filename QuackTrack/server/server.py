import tornado.ioloop
import tornado.web
import tornado.websocket
import socket

website_clients = []
esp_clients = []

class MainHandler(tornado.web.RequestHandler):
    def get(self):
        self.render("index.html")

class WebSocketHandler(tornado.websocket.WebSocketHandler):
    def open(self):
        print("WebSocket connection opened")
        website_clients.append(self)

    def on_message(self, message):
        self.write_message("Received message: " + message)
        #todo send message to esp clients
        print("Received website message: " + message)
        print(f"Sending message to { len(esp_clients)} ESP32 clients: ")

        #check if the clients are still connected and remove them if not
        for client in esp_clients:
            try:
                client.write_message(message)
            except:
                print("Client not connected")
                esp_clients.remove(client)


    def on_close(self):
        print("WebSocket connection closed")
        website_clients.remove(self)


class ESPHandler(tornado.websocket.WebSocketHandler):
    def check_origin(self, origin):
        return True
   
    def open(self):
        print("ESP32 webSocket opened")
        esp_clients.append(self)

    def on_message(self, message):
        print("Received ESP message: " + message)
        self.write_message(u"Hi ESP32! : " + message)

    def on_close(self):
        print("WebSocket connection closed")
        



def make_app():
    return tornado.web.Application([
        (r"/", MainHandler),
        (r"/websocket", WebSocketHandler),
        (r"/esp32", ESPHandler),
    ])

if __name__ == "__main__":
    app = make_app()
    app.listen(8888)
    print("Server started on http://localhost:8888")
    # print out the IP address of the server
    ip = socket.gethostbyname(socket.gethostname())
    print("Server started on http://" + ip + ":8888")
    tornado.ioloop.IOLoop.current().start()

