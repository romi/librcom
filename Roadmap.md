


* Check

- when messagelink between rcregistry and proxy is broken, remove proxy from list.
  This happens, for example, when the application crashes.

- No double registration of messagehub or datahub or streamer with duplicate topic name 

- proxy: improve initialisation: PROXY_INITIALIZING might loop forever

- verify usage of json_isnull(): json_object_get returns undefined, not null!

- closing a messagelink is too slow. Example: camera_recorder start/stop

- registry socket goes in TIME_WAIT after quitting. why?


* Thing to do

- Data dump & data replay (see code old rcom)
- Python interface
- C++ wrapper
- rcom-to-mqtt bridge
- rcom-to-ros bridge
- use a thread pool
- keep HTTP connections open

