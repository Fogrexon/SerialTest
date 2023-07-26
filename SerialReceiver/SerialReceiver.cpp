#include <Windows.h>
#include <WinUsb.h>
#include <iostream>

#define BUFFER_SIZE 1024

int main()
{
    // WinUSB関数を使用して、USBデバイスにアクセスするための変数を宣言します。
    HANDLE deviceHandle;
    WINUSB_INTERFACE_HANDLE interfaceHandle;
    UCHAR buffer[BUFFER_SIZE];
    ULONG lengthTransferred;

    // USBデバイスに接続します。
    deviceHandle = CreateFile(L"\\\\.\\COM7", GENERIC_WRITE | GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
    if (deviceHandle == INVALID_HANDLE_VALUE) {
    std::cout << GetLastError() << std::endl;
        std::cout << "Failed to connect to USB device." << std::endl;
        return 1;
    }


    // WinUSBインターフェースハンドルを取得します。
    if (WinUsb_Initialize(deviceHandle, &interfaceHandle) == FALSE) {
    std::cout << GetLastError() << std::endl;
        std::cout << "Failed to get WinUSB interface handle." << std::endl;
        CloseHandle(deviceHandle);
        return 1;
    }

    // バイナリを受信するためのループを開始します。
    while (true) {
        if (WinUsb_ReadPipe(interfaceHandle, 0x81, buffer, BUFFER_SIZE, &lengthTransferred, NULL) == FALSE) {
            std::cout << "Failed to read data from USB device." << std::endl;
            break;
        }

        // 受信したバイナリを処理します。
        // ...

        // 受信したバイナリを表示します。
        for (ULONG i = 0; i < lengthTransferred; i++) {
            printf("%02X ", buffer[i]);
        }
        printf("\n");
    }

    // WinUSBインターフェースハンドルを解放します。
    WinUsb_Free(interfaceHandle);

    // USBデバイスとの接続を解除します。
    CloseHandle(deviceHandle);

    return 0;
}
