#include <iostream>

#include "../ClientSocket.h"
#include "../ServerSocket.h"

#define PORT 123456

using namespace std;
using namespace simplesocket;

int exampleTcp();

int exampleUdp();

int main(int argc, char *argv[]) {
    int eid;
    cout << "Which example would you like to execute? [tcp: 1, upd: 2] ";
    cin >> eid;

    switch (eid) {
        case 1:
            return exampleTcp();
        case 2:
            return exampleUdp();
        default:
            return 0;
    }
}

int exampleTcp() {
    int type;
    cout << "server of client? [server: 1, client: 2] ";
    cin >> type;

    if (type == 1) {
        char buff[1024] = {};
        ServerSocket server;
        if (server.openServer((uint16_t) PORT, IPType::TCP) < 0) {
            cout << "open error\n";
            return 0;
        }
        cout << "waiting for a client ...\n";

        ClientInfo clientInfo = {};
        server.acceptClient(&clientInfo);

        memset(buff, '\0', sizeof(buff));
        server.receiveFromClient(&clientInfo, buff, sizeof(buff) - 1);
        cout << buff;

        server.closeClient(&clientInfo);
        server.closeSocket();
        cout << "successful!\n";
        return 0;

    } else if (type == 2) {

        ClientSocket client;
        if (client.connectServer("127.0.0.1", (uint16_t) PORT, IPType::TCP) < 0) {
            cout << "not found server\n";
            return 0;
        }
        string message = "Hello!\n";
        if (client.sendToServer(message.c_str(), message.size()) == 0) {
            cout << "send error\n";
        }
        client.closeSocket();
        cout << "successful!\n";
        return 0;
    }
    return 0;
}

int exampleUdp() {
    int type;
    cout << "server of client? [server: 1, client: 2] ";
    cin >> type;

    if (type == 1) {
        char buff[1024] = {};
        ServerSocket server;
        if (server.openServer((uint16_t) PORT, IPType::UDP) < 0) {
            cout << "open error\n";
            return 0;
        }
        cout << "waiting for a client ...\n";

        ClientInfo clientInfo = {};
        memset(buff, '\0', sizeof(buff));
        server.receiveFromSomeone(&clientInfo, buff, sizeof(buff) - 1);
        cout << buff;

        server.closeSocket();
        cout << "successful!\n";
        return 0;

    } else if (type == 2) {

        ClientSocket client;
        if (client.connectServer("127.0.0.1", (uint16_t) PORT, IPType::UDP) < 0) {
            cout << "not found server\n";
            return 0;
        }
        string message = "Hello!\n";
        if (client.sendToServer(message.c_str(), message.size()) == 0) {
            cout << "send error\n";
        }
        client.closeSocket();
        cout << "successful!\n";
        return 0;
    }
    return 0;
}
