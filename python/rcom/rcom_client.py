import json
import websocket
import socket

class RcomClient():

    def __init__(self, topic, id, registry_ip = None):
        self.id = id
        if registry_ip == None:
            registry_address = self._lookup_registry()
        else:
            registry_address = f'{registry_ip}:10101'
        print(f'Using registry address {registry_address}')
        registry = websocket.create_connection(f'ws://{registry_address}')
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
            cmd = { 'id': self.id, 'method': method, 'params': params }
        else:
            cmd = { 'id': self.id, 'method': method }
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
