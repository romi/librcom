import asyncio
import signal
import json
import socket
import websockets
from websockets.asyncio.server import serve
from websockets.exceptions import ConnectionClosedError, ConnectionClosedOK
from rcom.rcom_registry import RcomRegistry

    
def get_local_ip():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.connect(("8.8.8.8", 80))
    ip = s.getsockname()[0]
    s.close()
    return ip


class RcomServer(object):

    def __init__(self, topic, handler, host, port, registry_ip=None):
        print('init')
        self.registry_ip = registry_ip
        self.topic = topic
        self.handler = handler
        self.host, self.port = host, port
        
        self.loop = asyncio.get_event_loop()
        self.stop = self.loop.create_future()
        self.loop.add_signal_handler(signal.SIGINT, self.stop.set_result, None)

        self._register()
        
        self.loop.run_until_complete(self._server())

    def _register(self):
        registry = RcomRegistry(self.registry_ip)
        registry.set(self.topic, f'{self.host}:{self.port}')

    async def _server(self):
        print('server')
        async with websockets.serve(self._handler, self.host, self.port):
            await self.stop

    async def _handler(self, websocket):
        while True:
            try:
                message = await websocket.recv()
                print(f"message: {message}")
                response = self._try_handle_message(message)
                print(f"response: {response}")
                payload = json.dumps(response)
                print(f"payload: {payload}")
                await websocket.send(payload)
                print(f"end handler")
            except ConnectionClosedError:
                break
            except ConnectionClosedOK:
                break
            
    def _try_handle_message(self, message):
        print('try_handle_message')
        method_name = None
        result = {}
        try:
            cmd = json.loads(message)
            if not 'method' in cmd:
                raise ValueError('Missing method')
            method_name = cmd['method']
            result = self._invoke(method_name, cmd)
        except AttributeError as ae:
            result = {'method': method_name,
                      'error': {'code': -1, 'message': 'Invalid method'}}
        except Exception as e:
            result = {'method': method_name, 'error': {'code': -1, 'message': repr(e)}}
        return result

    def _invoke(self, method_name, args):
        method = self.handler.mapping[method_name]
        return method(args)

        
class MonsterHandler(object):
    def __init__(self):
        self.mapping = {'jump-around': self._jump_around,
                        'gently-scare-someone': self._gently_scare_someone,
                        'get-energy-level': self._get_energy_level }
    
    def _jump_around(self, args):
        return {'method': 'jump-around'}
    
    def _gently_scare_someone(self, args):
        return {'method': 'gently-scare-someone'}

    def _get_energy_level(self, args):
        return {'method': 'get-energy-level', 'result': {'energy-level': 10.0}}
    
    
if __name__ == '__main__':
    my_ip = get_local_ip()
    my_port = 45678
    server = RcomServer('elmo', MonsterHandler(), my_ip, my_port)

    
