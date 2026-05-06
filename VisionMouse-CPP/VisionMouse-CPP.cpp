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
    float xcam, ycam,xgiua,ygiua;
    deque<float> queueY;
    string temp;
    float xlast, ylast, xnew, ynew, sensitive;
    xlast = -1;
    ylast = -1;
    bool chamtay=false;
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

            //cout << data << endl;

            // Lấy tọa độ x,y
            stringstream ss(data);
            getline(ss, temp, '[');
            getline(ss, temp, ',');
            xcam = stof(temp);
            getline(ss, temp, ',');
            ycam = stof(temp);
            getline(ss, temp, ',');
            xgiua = stof(temp);
            getline(ss, temp, ']');
            ygiua = stof(temp);
           
            // Dùng hàng đợi tối ưu chuột
            queueX.push_back(xcam);
            queueY.push_back(ycam);
            if (queueX.size() > 5 && queueY.size() > 5) {
                queueX.pop_front();
                queueY.pop_front();
            }
            float sumX = 0;
            float sumY = 0;
            //cout << "\n hang doi X:";
            for (float i : queueX) {
                sumX += i;
                //cout << i << ", ";
            }
            //cout << "\n hang doi Y";
            for (float i : queueY) {
                sumY += i;
                //cout << i << ", ";
            }
            sumX /= queueX.size();
            sumY /= queueY.size();
            //cout << "\nsumX: " << sumX << "sumY: " << sumY << endl;


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
            if ((sqrt(pow(xcam - xgiua, 2) + (pow(ycam - ygiua, 2)))) > 0.04) {
                chamtay = false;
            }
            if ((sqrt(pow(xcam - xgiua, 2) + (pow(ycam - ygiua, 2)))) <0.04 && chamtay==false) {
                mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
                chamtay = true;

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