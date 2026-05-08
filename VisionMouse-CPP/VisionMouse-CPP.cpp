#include <iostream>
#include <winsock2.h>
#include <string>
#include <windows.h> 
#include <deque>
#include <shellapi.h>
#include <sstream>
#include <math.h>
using namespace std;

#pragma comment(lib, "ws2_32.lib") 
  
int main() {
    float xtro, ytro,xgiua,ygiua,xcai,ycai;
    deque<float> queueY;
    string temp;
    float xlast, ylast, xnew, ynew, sensitive;
    xlast = -1;
    ylast = -1;
    bool clicktrai=true;
  
    POINT Current;
    deque<float> queueX; 

    //Chạy file python
    ShellExecuteA(NULL, "open", "cmd.exe", "/k python \"Python_Sensor/Python Sensor.py\"", NULL, SW_SHOW);
   
    // Khởi tạo winsock
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // Tạo Socket UDP
    SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5005);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));

    char buffer[1024];
    sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);

    //Lấy chiều dài, rộng của màn hình
    SetProcessDPIAware(); // Hàm giúp trả về tọa độ pixel thực tế
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    while (true) {
       
        int bytesRead = recvfrom(serverSocket, buffer, 1024, 0, (sockaddr*)&clientAddr, &clientAddrLen);

        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            string data(buffer);

            // Lấy tọa độ x,y
            stringstream ss(data);
            getline(ss, temp, '[');
            getline(ss, temp, ',');
            xtro = stof(temp);
            getline(ss, temp, ',');
            ytro = stof(temp);
            getline(ss, temp, ',');
            xgiua = stof(temp);
            getline(ss, temp, ',');
            ygiua = stof(temp);
            getline(ss, temp, ',');
            xcai = stof(temp);
            getline(ss, temp, ']');
            ycai = stof(temp);
           
            // Dùng hàng đợi tối ưu chuột
            queueX.push_back(xtro);
            queueY.push_back(ytro);
            if (queueX.size() > 5 && queueY.size() > 5) {
                queueX.pop_front();
                queueY.pop_front();
            }
            float sumX = 0;
            float sumY = 0;

            for (float i : queueX) {
                sumX += i;
            }
            for (float i : queueY) {
                sumY += i;
            }
            sumX /= queueX.size();
            sumY /= queueY.size();      


            // Tính toán độ di chuyển của tọa độ chuột
            sensitive = 2.5;
            GetCursorPos(&Current);
            xnew = sumX;
            ynew = sumY;
                // Frame đầu chưa di chuyển chuột
            if (xlast == -1 && ylast == -1) {
                xlast=xnew;
                ylast=ynew;
                continue;
            }

            Current.x += (xnew - xlast) * sensitive * screenWidth;
            Current.y += (ynew - ylast) * sensitive * screenHeight;

            // Click chuột
            if ((sqrt(pow(xtro - xgiua, 2) + (pow(ytro - ygiua, 2)))) > 0.02) {
                clicktrai = true;
            }
            if ((sqrt(pow(xtro - xgiua, 2) + (pow(ytro - ygiua, 2)))) <0.02 && clicktrai==true) {
                mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
                clicktrai = false;

            }

            // Cuộn chuột
            if ((sqrt(pow(xcai - xtro, 2) + (pow(ycai - ytro, 2)))) < 0.04 && ynew - ylast > 0.05) {
                mouse_event(MOUSEEVENTF_WHEEL, 0, 0, 50,0 );
            }
            else if ((sqrt(pow(xcai - xtro, 2) + (pow(ycai - ytro, 2)))) < 0.04 && ynew - ylast < -0.05) {
            mouse_event(MOUSEEVENTF_WHEEL, 0, 0, -50, 0);
            }

            // Điều khiển chuột
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