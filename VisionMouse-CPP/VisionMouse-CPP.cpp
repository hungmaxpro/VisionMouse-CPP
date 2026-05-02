#include <iostream>
#include <winsock2.h>
#include <string>
#include <windows.h> 

#pragma comment(lib, "ws2_32.lib") // Cách 2 để link thư viện mà không cần vào Properties

using namespace std;

int main() {
    // Khởi tạo winsock
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // Tạo Socket UDP
    SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    //Lấy chiều dài, rộng của màn hình
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    float strx,stry,xcam,ycam;
    
    
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5005);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    
    bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));

    

    char buffer[1024];
    sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);

    while (true) {
       
        int bytesRead = recvfrom(serverSocket, buffer, 1024, 0, (sockaddr*)&clientAddr, &clientAddrLen);

        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            string data(buffer);

            // Tạm thời in ra màn hình để test xem "thông mạng" chưa
            cout << "Nhan duoc: " << data << endl;
            strx = data.find("x");
            stry = data.find("y");
            xcam = stof(data.substr(strx + 3, stry - strx - 6));
            ycam = stof(data.substr(stry+3, data.find("}") - stry));
            

            SetCursorPos(xcam * screenWidth, ycam * screenHeight);
        }
        // Thoát nếu bấm phím (tùy chọn)
        if (GetKeyState('Q') & 0x8000) break;
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}