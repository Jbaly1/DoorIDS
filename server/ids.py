import socket
from paho.mqtt import client as mqtt
from dotenv import load_dotenv
import os

load_dotenv()

HOST = "0.0.0.0"   # listen on all interfaces (including 192.168.1.3)
PORT = 5000       # must match ESP32 port

client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2, "jonathan")
client.connect("localhost", 1883)
client.loop_start()

def publish_door_state(state):
    client.publish("lab/test", state)
    client.publish("lab/test/state", state, retain=True)

def main():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.bind((HOST, PORT))
        s.listen(1)
        print(f"Listening on {HOST}:{PORT}")

        while True:
            conn, addr = s.accept()
            print(f"Connected by {addr}")

            with conn:
                while True:
                    data = conn.recv(1024)
                    if not data:
                        print("Client disconnected")
                        break

                    # ESP32 is sending single chars like 'O', 'C', 'H'
                    for b in data:
                        ch = chr(b)
                        if ch == 'O':
                            publish_door_state("OPEN")
                        elif ch == 'C':
                            publish_door_state("CLOSED")
                        elif ch == 'H':
                            publish_door_state("HELD")
                        else:
                            print(f"Unknown code: {repr(ch)}")

if __name__ == "__main__":
    main()
