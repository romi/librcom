from abc import ABC, abstractmethod
import json
import socket
import websocket
import socket
from rcom.rcom_registry import RcomRegistry

class RcomClient(ABC):

    def __init__(self, topic, id = None):
        self.topic = topic
        if id: self.id = id
        else: self.id = topic

    @abstractmethod
    def _connect(self):
        pass

    @abstractmethod
    def disconnect(self):
        pass

    @abstractmethod
    def _send(self, data):
        pass

    @abstractmethod
    def _recv(self):
        pass

    @abstractmethod
    def binary(self, data):
        pass
    
    def execute(self, method, params=None):
        self._send_request(method, params)
        return self._read_response()
    
    def _send_request(self, method, params):
        if params != None:
            cmd = { 'id': self.id, 'method': method, 'params': params }
        else:
            cmd = { 'id': self.id, 'method': method }
        request = json.dumps(cmd)
        #print(f'request: {request}')
        self._send(request)
        
    def _read_response(self):
        data = self._recv()
        #print(f'data=/{data}/, type={type(data)}')
        response = json.loads(data)
        #print(response)
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
        
    
class RcomWSClient(RcomClient):

    def __init__(self, topic, id = None, registry_ip = None):
        super().__init__(topic, id)
        self.registry_ip = registry_ip
        self._connect()

    def _connect(self):
        registry = RcomRegistry(self.registry_ip)
        address = registry.get(self.topic)
        print(f"Connecting to '{self.topic}' at ws://{address}")
        self.connection = websocket.create_connection(f"ws://{address}")

    def disconnect(self):
        self.connection.close()

    def _send(self, data):
        self.connection.send(data)

    def _recv(self):
        return self.connection.recv()

    def binary(self, data):
        self.connection.send(data, websocket.ABNF.OPCODE_BINARY)
        data = self.connection.recv()
        if type(data) is str:
            raise ValueError(data)
        else:
            return data

        
class RcomTCPClient(RcomClient):

    def __init__(self, ip, port, topic, id = None):
        super().__init__(topic, id)
        self.connection = None
        self.ip = ip
        self.port = port
        self._connect()

    def _connect(self):
        self.connection = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.connection.connect((self.ip, self.port))

    def disconnect(self):
        self.connection.close()

    def _send(self, data):
        print(f'send {data}')
        self.connection.send(data.encode('utf-8'))
        self.connection.send('\n'.encode('utf-8'))
        print(f'send done')

    def _recv(self):
        print(f'recv')
        s = "";
        while True:
            data = self.connection.recv(1)
            c = data.decode('utf-8')
            print(f'received {c}')
            if c == "\n" or c == "\r":
                break
            else: s += c
        print(f'received {s}')
        return s

    def binary(self, data):
        raise ValueError(f"Binary data not implemented on TCP connections")
        
