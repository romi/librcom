import json
import websocket

class RcomClient():

    def __init__(self, topic, registry='127.0.0.1'):
        registry = websocket.create_connection(f'ws://{registry}:10101')
        cmd = {'request': 'get', 'topic': topic}
        request = json.dumps(cmd)
        registry.send(request)
        data = registry.recv()
        response = json.loads(data)
        if 'address' in response:
            self.connection = websocket.create_connection(f"ws://{response['address']}")
        else:
            raise RuntimeError(f'Failed to obtain the address for "{topic}"')
            
    def execute(self, method, params=None):
        self._send_request(method, params)
        return self._read_response()
    
    def _send_request(self, method, params):
        if params != None:
            cmd = { 'method': method, 'params': params }
        else:
            cmd = { 'method': method }
        request = json.dumps(cmd)
        self.connection.send(request)
        
    def _read_response(self):
        response = json.loads(self.connection.recv())
        self._check_error(response)
        if 'result' in response:
            result = response['result']
        else: result = None
        return result

    def _check_error(self, response):
        if 'error' in response:
            error = response['error']
            print(f"Request failed: {error['message']}")
            raise RuntimeError(error['message'])
