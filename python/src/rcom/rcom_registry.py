import json
import websocket
import socket

class RcomRegistry():
    
    def __init__(self, ip = None):
        self.id = id
        if ip == None:
            self.address = self._lookup_registry()
        else:
            self.address = f'{ip}:10101'
        print(f'Using registry address {self.address}')

    def get(self, topic):
        response = self._execute({'request': 'get', 'topic': topic})
        if 'address' in response:
            print(f"Connecting to '{topic}' at ws://{response['address']}")
            self.connection = websocket.create_connection(f"ws://{response['address']}")
        else:
            raise RuntimeError(f'Failed to obtain the address for "{topic}"')
        return response['address']
        
    def set(self, topic, address):
        response = self._execute({'request': 'register',
                                  'topic': topic,
                                  'address': address})
        
    def _execute(self, cmd):
        result = None
        self._connect()
        self._send_request(cmd)
        result = self._read_response()
        self._disconnect()
        return result

    def _connect(self):
        self.connection = websocket.create_connection(f'ws://{self.address}')
        
    def _disconnect(self):
        self.connection.close()
        self.connection = None
    
    def _send_request(self, cmd):
        request = json.dumps(cmd)
        self.connection.send(request)
        
    def _read_response(self):
        data = self.connection.recv()
        print(data)
        response = json.loads(data)
        print(response)
        self._check_error(response)
        return response

    def _check_error(self, response):
        if 'success' in response:
            success = response['success']
            if not success:
                print(f"Request failed: {response['message']}")
                raise RuntimeError(response['message'])

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
