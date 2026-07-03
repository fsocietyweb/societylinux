#include <iostream>
#include <string>
#include <thread>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

class VindertechEmergencyHub {
private:
    int listenSocket;
    int port;
    bool isRunning;
    int currentAlertLevel;

    void processEmergencyLevel(int level, const std::string& senderIp) {
        currentAlertLevel = level;

        // Level 0-3: Standard Network Syncs
        if (level < 4) {
            std::cout << "\n\033[1;32m[SATELLITE SYNC]\033[0m Weather updated to Level " << level 
                      << " by node " << senderIp << ".\nStatus: Normal Operations Standby.\n> " << std::flush;
        } 
        // Level 4-5: CRITICAL LIFE-SAVER OVERRIDE (Flashes Bold Red / Backgrounds)
        else {
            std::cout << "\n\n";
            if (level == 4) {
                std::cout << "\033[1;35m"; // Magenta/Purple for level 4
                std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
                std::cout << "⚠️  CRITICAL VINDERTECH EMERGENCY ALERT DETECTED  ⚠️\n";
                std::cout << "SOURCE NODE: " << senderIp << "\n";
                std::cout << "STATUS: LEVEL 4 -> LOCAL SWARM EVACUATION PROTOCOLS ACTIVE\n";
                std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
            } else if (level == 5) {
                std::cout << "\033[1;31;41m"; // Bold Red text with Red Background for level 5
                std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
                std::cout << "⚠️  CRITICAL VINDERTECH EMERGENCY ALERT DETECTED  ⚠️\n";
                std::cout << "SOURCE NODE: " << senderIp << "\n";
                std::cout << "STATUS: LEVEL 5 -> CATACLYSMIC STORM IMMINENT. SEEK SHELTER.\n";
                std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
            }
            std::cout << "\033[0m\n> " << std::flush; // Reset terminal colors
        }
    }

    void listenLoop() {
        char buffer[1024];
        sockaddr_in clientAddr;
        socklen_t clientLength = sizeof(clientAddr);

        while (isRunning) {
            int bytesReceived = recvfrom(listenSocket, buffer, sizeof(buffer) - 1, 0, (sockaddr*)&clientAddr, &clientLength);
            if (bytesReceived > 0) {
                buffer[bytesReceived] = '\0';
                std::string payload(buffer);

                char ipStr[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &clientAddr.sin_addr, ipStr, INET_ADDRSTRLEN);

                if (payload.rfind("WEATHER_SET:", 0) == 0) {
                    try {
                        int level = std::stoi(payload.substr(12));
                        if (level >= 0 && level <= 5) {
                            processEmergencyLevel(level, ipStr);
                        }
                    } catch (...) {
                        // Suppress parsing errors to keep terminal stable
                    }
                } else {
                    std::cout << "\n\033[1;36m[COMMS FROM " << ipStr << "]:\033[0m " << payload << "\n> " << std::flush;
                }
            }
        }
    }

public:
    VindertechEmergencyHub(int listenPort) : port(listenPort), isRunning(false), listenSocket(-1), currentAlertLevel(0) {}

    bool startSystem() {
        listenSocket = socket(AF_INET, SOCK_DGRAM, 0); // Open UDP Socket
        if (listenSocket < 0) return false;

        sockaddr_in serverAddr;
        std::memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);
        serverAddr.sin_addr.s_addr = INADDR_ANY;

        if (bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
            close(listenSocket);
            return false;
        }

        isRunning = true;
        std::thread(&VindertechEmergencyHub::listenLoop, this).detach();
        std::cout << "\033[1;32m[VINDERTECH CORE]\033[0m Early Warning System Active on port " << port << "...\n";
        return true;
    }

    void broadcastAlert(const std::string& targetIp, int targetPort, int level) {
        int sendSocket = socket(AF_INET, SOCK_DGRAM, 0);
        if (sendSocket < 0) return;

        sockaddr_in targetAddr;
        std::memset(&targetAddr, 0, sizeof(targetAddr));
        targetAddr.sin_family = AF_INET;
        targetAddr.sin_port = htons(targetPort);
        inet_pton(AF_INET, targetIp.c_str(), &targetAddr.sin_addr);

        std::string payload = "WEATHER_SET:" + std::to_string(level);
        sendto(sendSocket, payload.c_str(), payload.length(), 0, (sockaddr*)&targetAddr, sizeof(targetAddr));
        close(sendSocket);
        std::cout << "[TX] Broadcasted Level " << level << " warning packet to " << targetIp << ":" << targetPort << "\n";
    }

    void sendChatMessage(const std::string& targetIp, int targetPort, const std::string& message) {
        int sendSocket = socket(AF_INET, SOCK_DGRAM, 0);
        if (sendSocket < 0) return;

        sockaddr_in targetAddr;
        std::memset(&targetAddr, 0, sizeof(targetAddr));
        targetAddr.sin_family = AF_INET;
        targetAddr.sin_port = htons(targetPort);
        inet_pton(AF_INET, targetIp.c_str(), &targetAddr.sin_addr);

        sendto(sendSocket, message.c_str(), message.length(), 0, (sockaddr*)&targetAddr, sizeof(targetAddr));
        close(sendSocket);
    }

    ~VindertechEmergencyHub() {
        isRunning = false;
        if (listenSocket >= 0) close(listenSocket);
    }
};

int main() {
    int listenPort = 5556; // C++ Default listen port
    VindertechEmergencyHub hub(listenPort);

    if (!hub.startSystem()) {
        std::cerr << "Initialization error.\n";
        return 1;
    }

    std::string targetIp;
    int targetPort;

    std::cout << "Enter target peer IP (e.g., 127.0.0.1): ";
    std::cin >> targetIp;
    std::cout << "Enter target peer Port (e.g., 5555 for Python): ";
    std::cin >> targetPort;
    std::cin.ignore(); // Clear newline buffer

    std::string userInput;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, userInput);

        if (userInput == "exit") break;
        if (userInput.empty()) continue;

        // Check if user is typing a single digit command (0-5)
        if (userInput.length() == 1 && userInput[0] >= '0' && userInput[0] <= '5') {
            int level = userInput[0] - '0';
            hub.broadcastAlert(targetIp, targetPort, level);
        } else {
            // Treat it as a standard live chat transmission
            hub.sendChatMessage(targetIp, targetPort, userInput);
        }
    }

    return 0;
}