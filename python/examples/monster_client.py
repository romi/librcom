from rcom.rcom_client import RcomClient
import socket

def get_local_ip():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.connect(("8.8.8.8", 80))
    ip = s.getsockname()[0]
    s.close()
    return ip
    
class RemoteMonster(RcomClient):
    
        def __init__(self, name, registry):
            super().__init__(name, registry)

        def jump_around(self):
            self.execute('jump-around')
        
        def gently_scare_someone(self, person_id):
            self.execute('gently-scare-someone', {'person-id': person_id})
        
        def get_energy_level(self):
            answer = self.execute('get-energy-level')
            return answer['energy-level']


if __name__ == '__main__':
    monster = RemoteMonster('elmo', get_local_ip())
    monster.jump_around()
    monster.gently_scare_someone('you')
    energy = monster.get_energy_level()
    print(f'energy level is {energy}')

