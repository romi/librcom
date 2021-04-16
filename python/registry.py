import time
import websocket
import json

class Registry:
    def __init__(self):
        try:
            self.ws = websocket.WebSocket()
            self.ws.connect("ws://127.0.0.1:10101")
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
            
    def try_get_address(self, name, topic):
        reply = self.send_request("list")
        return self.find_address(reply['list'], name, topic)
    
    def send_request(self, req):
        request = { "request": req }
        self.ws.send(json.dumps(request))
        reply = self.ws.recv()
        return json.loads(reply)
        
    def find_address(self, node_list, name, topic):
        for node in node_list:
            if node['name'] == name and node['topic'] == topic:
                return node['addr']
        return None

            

    
