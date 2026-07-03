import socket
import threading
import time

class VindertechEmergencyHub:
    def __init__(self, listen_port=5555):
        self.listen_port = listen_port
        self.is_running = False
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.current_alert_level = 0

    def start_system(self):
        self.is_running = True
        self.sock.bind(("0.0.0.0", self.listen_port))
        
        # Background satellite monitor thread
        thread = threading.Thread(target=self._network_receiver, daemon=True)
        thread.start()
        print(f"[VINDERTECH CORE] Early Warning System Active on port {self.listen_port}...")

    def _network_receiver(self):
        while self.is_running:
            try:
                data, addr = self.sock.recvfrom(1024)
                payload = data.decode('utf-8').strip()

                if payload.startswith("WEATHER_SET:"):
                    level = int(payload.split(":")[1])
                    self.process_emergency_level(level, addr[0])
                else:
                    print(f"\n[COMMS FROM {addr[0]}]: {payload}\n> ", end="")
            except:
                break

    def process_emergency_level(self, level, sender_ip):
        self.current_alert_level = level
        
        # Level 0-3: Standard Updates
        if level < 4:
            print(f"\n[SATELLITE SYNC] Weather updated to Level {level} by node {sender_ip}.")
            print(f"Status: Normal Operations Standby.\n> ", end="")
        
        # Level 4-5: CRITICAL LIFE-SAVER OVERRIDE
        else:
            print("\n\n" + "!" * 60)
            print(f"⚠️  CRITICAL VINDERTECH EMERGENCY ALERT DETECTED  ⚠️")
            print(f"SOURCE NODE: {sender_ip}")
            if level == 4:
                print("STATUS: LEVEL 4 -> LOCAL SWARM EVACUATION PROTOCOLS ACTIVE")
            elif level == 5:
                print("STATUS: LEVEL 5 -> CATACLYSMIC STORM IMMINENT. SEEK SHELTER IMMEDIATELY.")
            print("!" * 60 + "\n> ", end="")

    def broadcast_alert(self, target_ip, target_port, level):
        """Broadcasts the emergency level to other users/C++ applications."""
        try:
            payload = f"WEATHER_SET:{level}"
            send_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            send_sock.sendto(payload.encode('utf-8'), (target_ip, target_port))
            print(f"[TX] Broadcasted Level {level} warning packet to {target_ip}:{target_port}")
        except Exception as e:
            print(f"[TX ERROR] Emergency transmission failed: {e}")

if __name__ == "__main__":
    hub = VindertechEmergencyHub(listen_port=5555)
    hub.start_system()

    target_ip = input("Enter target peer IP: ")
    target_port = int(input("Enter target peer Port: "))

    while True:
        user_input = input("> Enter message or alert level (0-5): ")
        if user_input.lower() == 'exit':
            break
        
        # If the user types a single digit between 0 and 5, broadcast it as a weather alert
        if user_input.isdigit() and 0 <= int(user_input) <= 5:
            hub.broadcast_alert(target_ip, target_port, int(user_input))
        else:
            # Otherwise, send it as a normal chat message
            try:
                send_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
                send_sock.sendto(user_input.encode('utf-8'), (target_ip, target_port))
            except Exception as e:
                print(f"Failed to send message: {e}")