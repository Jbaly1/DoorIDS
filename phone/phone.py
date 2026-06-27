import os
import time
import subprocess
import paho.mqtt.client as mqtt

BROKER = os.getenv("MQTT_HOST", "YOUR IP")
PORT = int(os.getenv("MQTT_PORT", "1883"))
TOPIC = os.getenv("MQTT_TOPIC", "lab/test/state")
CLIENT_ID = os.getenv("MQTT_CLIENT_ID", "moto-sub")

USER = os.getenv("MQTT_USER")
PASS = os.getenv("MQTT_PASS")

def notify(title: str, content: str):
    # Use subprocess args, not shell strings (prevents "too many arguments")
    subprocess.run(
        ["termux-notification", "--title", title, "--content", content, "--sound"],
        check=False
    )

def vibrate(ms: int = 300):
    subprocess.run(["termux-vibrate", "-d", str(ms)], check=False)

def on_connect(client, userdata, flags, rc, properties=None):
    if rc == 0:
        notify("MQTT", f"Connected to {BROKER}:{PORT}")
        client.subscribe(TOPIC, qos=1)
    else:
        notify("MQTT", f"Connect failed rc={rc}")

def on_message(client, userdata, msg):
    payload = msg.payload.decode(errors="replace").strip().upper()

    if payload == "OPEN":
        notify("Door Opened", "Door state: OPEN")
        vibrate(500)
    elif payload == "CLOSED":
        notify("Door Closed", "Door state: CLOSED")
        vibrate(250)
    elif payload == "HELD":
        notify("Door Held", "Door state: HELD")
        vibrate(800)
    else:
        # Unknown payload: still notify, but don't pretend it's one of the three
        notify("Door State", f"Unknown payload: {payload}")

def main():
    # Use the newer callback API version to avoid the deprecation warning
    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2, client_id=CLIENT_ID, clean_session=True)

    if USER and PASS:
        client.username_pw_set(USER, PASS)

    client.on_connect = on_connect
    client.on_message = on_message

    client.reconnect_delay_set(min_delay=1, max_delay=30)

    while True:
        try:
            client.connect(BROKER, PORT, keepalive=60)
            client.loop_forever()
        except Exception as e:
            notify("MQTT", f"Disconnected: {e}")
            time.sleep(3)

if __name__ == "__main__":
    main()
