import json
import websocket
import socket
from rcom.rcom_registry import RcomRegistry

class RcomClient():

    def __init__(self, topic, id, registry_ip = None):
        self.topic = topic
        self.id = id
        self.registry_ip = registry_ip
        self._connect()

    def _connect(self):
        registry = RcomRegistry(self.registry_ip)
        address = registry.get(self.topic)
        print(f"Connecting to '{self.topic}' at ws://{address}")
        self.connection = websocket.create_connection(f"ws://{address}")

    def leave(self):
        self.connection.close()
        
    def execute(self, method, params=None):
        self._send_request(method, params)
        return self._read_response()
    
    def _send_request(self, method, params):
        if params != None:
            cmd = { 'id': self.id, 'method': method, 'params': params }
        else:
            cmd = { 'id': self.id, 'method': method }
        request = json.dumps(cmd)
        print(f'request: {request}')
        self.connection.send(request)
        
    def _read_response(self):
        data = self.connection.recv()
        print(f'data=/{data}/, type={type(data)}')
        response = json.loads(data)
        print(response)
        self._check_error(response)
        if 'result' in response:
            result = response['result']
        else: result = None
        return result

    def _check_error(self, response):
        if 'error' in response:
            error = response['error']
            print(error)
            if 'message' in error:
                print(f"Request failed: {error['message']}")
                raise RuntimeError(error['message'])
            else:
                print(f"Request failed: {error}")
                raise RuntimeError('Unknown error')
            
    def _lookup_registry(self, port = 10101):
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        client_socket.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
        
        server_address = ('<broadcast>', port)

        # Send the broadcast message
        broadcast_message = 'who-has-the-registry-ip'
        client_socket.sendto(broadcast_message.encode(), server_address)

        # Wait for the response
        client_socket.settimeout(60)  # Set a timeout to avoid blocking indefinitely
        try:
            response, server = client_socket.recvfrom(1024)
            return response.decode()
        except socket.timeout:
            raise RuntimeError('Registry lookup timed-out.')

        finally:
            client_socket.close()
