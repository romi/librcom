import time
import websocket
import json

from registry import *

class Client:
    
    def __init__(self, topic, timeout=10):
        self.__connect(topic, timeout)

    def __connect(self, topic, timeout):
        try:
            registry = Registry()
            address = registry.get_address(topic, timeout)
            self.ws = websocket.WebSocket()
            self.ws.connect("ws://%s" % address)
        except websocket.WebSocketException:
            print("HEADERS: " + str(self.ws.getheaders()))
            raise Exception("Failed to connect")
        except Exception:
            raise Exception("Failed to obtain the address")

    def execute(self, method, **kwargs):
        request = { 'method': method, 'params': kwargs }
        self.send(request)
        return self.recv()

    def send(self, request):
        self.ws.send(json.dumps(request))
    
    def recv(self):
        reply = self.ws.recv()
        return json.loads(reply)
    
