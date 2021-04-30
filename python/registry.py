import time
import websocket
import json

class Registry:
    def __init__(self, ip="127.0.0.1"):
        try:
            self.ws = websocket.WebSocket()
            self.ws.connect(f"ws://{ip}:10101")
        except websocket.WebSocketException:
            print("HEADERS: " + str(self.ws.getheaders()))
            raise Exception("Failed to connect")

    def _time_in_seconds(self):
        return time.time() / 1000.0

    def _set_timeout(self, timeout):
        self.start_time = self._time_in_seconds()
        self.timeout = timeout

    def _time_passed(self):
        return self._time_in_seconds() - self.start_time
    
    def _assert_timeout(self):
        if (self.timeout > 0
            and self._time_passed() > self.timeout):
            raise Exception("Timeout")

    def get_address(self, name, topic, timeout):
        address = None
        self._set_timeout(timeout)
        while address == None:  
            address = self.try_get_address(name, topic)
            time.sleep(1)
            self._assert_timeout()
        return address
            
    def try_get_address(self, topic):
        address = None
        reply = self.send_get_topic(topic)
        if reply["success"]:
            return reply["address"]
        else:
            raise Exception(reply["message"])
    
    def send_get_topic(self, topic):
        request = { "request": req, "topic": topic }
        self.ws.send(json.dumps(request))
        reply = self.ws.recv()
        return json.loads(reply)

            

    
