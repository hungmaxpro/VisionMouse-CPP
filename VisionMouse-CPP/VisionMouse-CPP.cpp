#include <iostream>
#include <winsock2.h>
#include <string>
#include <windows.h> 

#pragma comment(lib, "ws2_32.lib") // Cách 2 để link thư viện mà không cần vào Properties

using namespace std;

int main() {
    float strx, stry, xcam, ycam;
    float xlast, ylast, xnew, ynew, sensitive;
    xlast = -1;
    ylast = -1;
    POINT Current;
    // Khởi tạo winsock
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // Tạo Socket UDP
    SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    

    //Lấy chiều dài, rộng của màn hình
    SetProcessDPIAware(); // Hàm giúp trả về tọa độ pixel thực tế
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    
    
    
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

            
            cout << data << endl;

            // Lấy tọa độ x,y
            strx = data.find("x");
            stry = data.find("y");
            xcam = stof(data.substr(strx + 3, stry - strx - 6));
            ycam = stof(data.substr(stry+3, data.find("}") - stry));
            
            // Tính toán độ di chuyển của tọa độ chuột
            sensitive = 2;
            GetCursorPos(&Current);
            xnew = xcam;
            ynew = ycam;
                // Frame đầu chưa di chuyển chuột
            if (xlast == -1 && ylast == -1) {
                xlast=xnew;
                ylast=ynew;
                continue;
            }
            Current.x += (xnew - xlast) * sensitive * screenWidth;
            Current.y += (ynew - ylast) * sensitive * screenHeight;

            // Điều khiển con chuột
            SetCursorPos(Current.x,Current.y);

            xlast = xnew;
            ylast = ynew;

        }

        // Thoát 
        if (GetKeyState('Q') & 0x8000) break;
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}