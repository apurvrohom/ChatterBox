#include <bits/stdc++.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <mutex>
using namespace std;

unordered_map<string,string> kvStore;
mutex kvMutex;

// Peers of server1
vector<int> peers = {6101, 6102};

void sendToPeers(const string &msg) {
    for (int port : peers) {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) continue;

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

        if (connect(sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
            close(sock);
            continue;
        }
        send(sock, msg.c_str(), msg.size(), 0);
        close(sock);
    }
}

void handleClient(int clientSock) {
    char buffer[1024];
    int n = recv(clientSock, buffer, sizeof(buffer), 0);
    if (n <= 0) {
        close(clientSock);
        return;
    }

    string cmd(buffer, n);
    stringstream ss(cmd);
    string op, key, value;
    ss >> op;

    // Replication command
    if (op == "SYNC") {
        ss >> op;
        lock_guard<mutex> lock(kvMutex);
        if (op == "SET") {
            ss >> key >> value;
            kvStore[key] = value;
        } else if (op == "DELETE") {
            ss >> key;
            kvStore.erase(key);
        }
        close(clientSock);
        return;
    }

    // Client commands
    if (op == "SET") {
        ss >> key >> value;
        {
            lock_guard<mutex> lock(kvMutex);
            kvStore[key] = value;
        }
        sendToPeers("SYNC SET " + key + " " + value);
        send(clientSock, "OK\n", 3, 0);
    }
    else if (op == "GET") {
        ss >> key;
        lock_guard<mutex> lock(kvMutex);
        if (kvStore.count(key))
            send(clientSock, (kvStore[key] + "\n").c_str(), kvStore[key].size()+1, 0);
        else
            send(clientSock, "NOT_FOUND\n", 10, 0);
    }
    else if (op == "DELETE") {
        ss >> key;
        {
            lock_guard<mutex> lock(kvMutex);
            kvStore.erase(key);
        }
        sendToPeers("SYNC DELETE " + key);
        send(clientSock, "OK\n", 3, 0);
    }
    else {
        send(clientSock, "ERROR\n", 6, 0);
    }

    close(clientSock);
}

int main() {
    int serverSock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(6100);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSock, (sockaddr*)&addr, sizeof(addr));
    listen(serverSock, 10);

    cout << "Server 1 running on port 6100\n";

    while (true) {
        int clientSock = accept(serverSock, nullptr, nullptr);
        thread t(handleClient, clientSock);
        t.detach();
    }
}