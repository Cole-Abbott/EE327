import tornado.ioloop
import tornado.web
import tornado.websocket

website_clients = []

class MainHandler(tornado.web.RequestHandler):
    def get(self):
        self.render("index.html")

class WebSocketHandler(tornado.websocket.WebSocketHandler):
    def open(self):
        print("WebSocket connection opened")
        website_clients.append(self)

    def on_message(self, message):
        # self.write_message("Received message: " + message)
        for client in website_clients:
            client.write_message("Broadcast message: " + message)

        print("Received message: " + message)

    def on_close(self):
        print("WebSocket connection closed")
        website_clients.remove(self)

def make_app():
    return tornado.web.Application([
        (r"/", MainHandler),
        (r"/websocket", WebSocketHandler),
    ])

if __name__ == "__main__":
    app = make_app()
    app.listen(8888)
    print("Server started on http://localhost:8888")
    tornado.ioloop.IOLoop.current().start()