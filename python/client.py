import time
import websocket
import json

from registry import *

class Client:
    
    def __init__(self, name, interface, timeout=10):
        self.__connect(name, interface, timeout)

    def __connect(self, name, interface, timeout):
        try:
            registry = Registry()
            address = registry.get_address('oquam', 'cnc', timeout)
            self.ws = websocket.WebSocket()
            self.ws.connect("ws://%s" % address)
        except websocket.WebSocketException:
            print("HEADERS: " + str(self.ws.getheaders()))
            raise Exception("Failed to connect")
        except Exception:
            raise Exception("Failed to obtain the address")

    def execute(self, method, **kwargs):
        request = { 'method': method, 'params': kwargs }
        self.ws.send(json.dumps(request))
        reply = self.ws.recv()
        return json.loads(reply)
    
