import socket
from PIL import Image
import argparse
from rcom.rcom_server import RcomServer

def get_local_ip():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.connect(("8.8.8.8", 80))
    ip = s.getsockname()[0]
    s.close()
    return ip


class FileCamera(object):
    def __init__(self, path):
        self.image = Image.open(path)
        self.mapping = {'camera:grab-jpeg-binary': self._grab,
                        'camera:set-value': self._set_value,
                        'camera:select-option': self._select_option,
                        'power-up': self._power_up,
                        'power-down': self._power_down }
    
    def _grab(self, args):
        return self.image

    def _set_value(self, args):
        print(f"camera:set-value: {args['name']}={args['value']}")

    def _select_option(self, args):
        print(f"camera:select-option: {args['name']}={args['value']}")
       
    def _power_up(self):
        print(f'power-up')
        
    def _power_down(self):
        print(f'power-down')

    
if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--topic', type=str, nargs='?', default="camera",
                    help='The registry topic')
    parser.add_argument('--file', type=str, nargs='?', default="test.jpg",
                    help='The file for the fake camera')
    args = parser.parse_args()
    
    my_ip = get_local_ip()
    my_port = 45678
    server = RcomServer(args.topic, FileCamera(args.file), my_ip, my_port)

    
