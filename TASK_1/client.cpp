#include <bits/stdc++.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "Usage: ./client <port> <command>\n";
        return 0;
    }

    int port = stoi(argv[1]);
    string cmd = "";

    for (int i = 2; i < argc; i++) {
        cmd += string(argv[i]) + " ";
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    connect(sock, (sockaddr*)&addr, sizeof(addr));
    send(sock, cmd.c_str(), cmd.size(), 0);

    char buffer[1024];
    int n = recv(sock, buffer, sizeof(buffer), 0);
    if (n > 0) {
        cout << string(buffer, n);
    }

    close(sock);
}