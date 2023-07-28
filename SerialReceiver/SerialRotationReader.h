#pragma once

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <string.h>
#include <chrono>
#include <thread>
#include <time.h>
#include <fstream>

class SerialRotationReader
{
private:
    HANDLE io_handler;
    COMSTAT status;
    DWORD errors;

    std::string fully_received_message;
    std::string unprocessed_message;

public:
    SerialRotationReader(char* com_port, DWORD COM_BAUD_RATE);

    bool ReadBuffer();
    bool ProcessReceivedMessage();
    bool CloseSerialPort();
    ~SerialRotationReader();
    bool connected;
    bool data_received;
    double rpm;
    double angle;
};
