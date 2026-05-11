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
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

struct Toadotay {
    float xtro, ytro, xgiua, ygiua, xcai, ycai;
};
struct Trangthaichuot {
    bool chamtay, clicktrai, dichuyen;
};

struct Toadochuot {
    float xlast, ylast, xnew, ynew;
};

void clickchuot(Toadotay tay,Trangthaichuot &trangthai) {
    if ((sqrt(pow(tay.xtro - tay.xgiua, 2) + (pow(tay.ytro - tay.ygiua, 2)))) > 0.03) {
        trangthai.chamtay = false;
    }
    if ((sqrt(pow(tay.xtro - tay.xgiua, 2) + (pow(tay.ytro - tay.ygiua, 2)))) < 0.03 && trangthai.chamtay == false && trangthai.clicktrai == true) {
        mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
        mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
        trangthai.chamtay = true;

    }
}

void cuonchuot(Toadotay tay, Trangthaichuot &trangthai, Toadochuot toado) {
    if ((sqrt(pow(tay.xcai - tay.xtro, 2) + (pow(tay.ycai - tay.ytro, 2)))) < 0.04) {
        trangthai.clicktrai = false;
        trangthai.dichuyen = false;
        if (toado.ynew - toado.ylast > 0.01) {
            mouse_event(MOUSEEVENTF_WHEEL, 0, 0, 100, 0);
        }
        else if (toado.ynew - toado.ylast < -0.01) {
            mouse_event(MOUSEEVENTF_WHEEL, 0, 0, -100, 0);
        }
    }
    else {
        trangthai.dichuyen = true;
        trangthai.clicktrai = true;
    }
}

float xulidulieudauvao(stringstream &ss) {
    string temp;
    getline(ss, temp, ',');
    return stof(temp);
}
int main() {
    Toadotay tay;
    Trangthaichuot trangthai{};
    Toadochuot toado{};
    deque<float> queueY{};
    string temp;
    float sensitive;
    toado.xlast = -1;
    toado.ylast = -1;
    trangthai.clicktrai = true;
    trangthai.dichuyen = true;
    POINT Current;
    deque<float> queueX; 

    //Chạy file python
    STARTUPINFOA si = { sizeof(STARTUPINFOA) };
    PROCESS_INFORMATION pi;
    string cmd = "\"Python Sensor.exe\"";
    CreateProcessA(NULL, (LPSTR)cmd.c_str(), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
   
    // Khởi tạo winsock
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);

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
    SetProcessDPIAware(); 
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    while (true) {
       
        int bytesRead = recvfrom(serverSocket, buffer, 1024, 0, (sockaddr*)&clientAddr, &clientAddrLen);

        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            string data(buffer);
            data.erase(0, 1);
            data.pop_back();
            stringstream ss(data);

            tay.xtro = xulidulieudauvao(ss);
            tay.ytro = xulidulieudauvao(ss);
            tay.xgiua = xulidulieudauvao(ss);
            tay.ygiua = xulidulieudauvao(ss);
            tay.xcai = xulidulieudauvao(ss);
            tay.ycai = xulidulieudauvao(ss);
           
            // Dùng hàng đợi tối ưu chuột
            queueX.push_back(tay.xtro);
            queueY.push_back(tay.ytro);
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
            toado.xnew = sumX;
            toado.ynew = sumY;
                // Frame đầu chưa di chuyển chuột
            if (toado.xlast == -1 && toado.ylast == -1) {
                toado.xlast=toado.xnew;
                toado.ylast=toado.ynew;
                continue;
            }
            
            if (trangthai.dichuyen) {
                Current.x += (toado.xnew - toado.xlast) * sensitive * screenWidth;
                Current.y += (toado.ynew - toado.ylast) * sensitive * screenHeight;
            }
            
            clickchuot(tay, trangthai);
            
            cuonchuot(tay, trangthai, toado);

            // Điều khiển chuột
            if (trangthai.dichuyen) {
                SetCursorPos(Current.x, Current.y);
            }
            toado.xlast = toado.xnew;
            toado.ylast = toado.ynew;

        }

        // Thoát 
        if (GetKeyState('Q') & 0x8000) {
            system("taskkill /F /IM \"Python Sensor.exe\" /T >nul 2>&1");
            break;
        }
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}