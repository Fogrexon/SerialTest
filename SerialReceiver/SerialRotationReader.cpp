#include "SerialRotationReader.h"

#define READ_BUFF_SIZE 2048

SerialRotationReader::SerialRotationReader(char* com_port, DWORD COM_BAUD_RATE)
{
    connected = false;

    io_handler = CreateFileA(com_port,
                             GENERIC_READ | GENERIC_WRITE,
                             FILE_SHARE_READ | FILE_SHARE_WRITE,
                             nullptr,
                             OPEN_EXISTING,
                             FILE_ATTRIBUTE_NORMAL,
                             nullptr);

    if (io_handler == INVALID_HANDLE_VALUE)
    {
        if (GetLastError() == ERROR_FILE_NOT_FOUND)
            printf("Warning: Handle was not attached. Reason: %s not available\n", com_port);
    }
    else
    {
        DCB dcbSerialParams = {0};

        if (!GetCommState(io_handler, &dcbSerialParams))
        {
            printf("Warning: Failed to get current serial params");
        }

        else
        {
            dcbSerialParams.BaudRate = COM_BAUD_RATE;
            dcbSerialParams.ByteSize = 8;
            dcbSerialParams.StopBits = ONESTOPBIT;
            dcbSerialParams.Parity = NOPARITY;
            dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;

            if (!SetCommState(io_handler, &dcbSerialParams))
                printf("Warning: could not set serial port params\n");
            else
            {
                connected = true;
                PurgeComm(io_handler, PURGE_RXCLEAR | PURGE_TXCLEAR);
            }
        }
    }
}

bool SerialRotationReader::ReadBuffer()
{
    data_received = false;
    DWORD bytes_read;
    char message_buffer[READ_BUFF_SIZE];

    ClearCommError(io_handler, &errors, &status);

    if (status.cbInQue <= 0) return true;

    if (!ReadFile(io_handler, &message_buffer[0], min(status.cbInQue, READ_BUFF_SIZE), &bytes_read, nullptr))
    {
        std::cout << GetLastError() << std::endl;
        printf("Warning: Failed to read from serial port\n");
        return false;
    }

    // 各文字をunprocessed_messageに格納していきつつ区切り文字があれば処理
    for (auto i = 0; i < bytes_read; i++)
    {
        unprocessed_message += message_buffer[i];

        if (message_buffer[i] == '\n')
        {
            // 区切り文字が観測されたら
            data_received = true;
            fully_received_message = unprocessed_message;
            unprocessed_message = "";
        }
    }
    data_received = true;

    return true;
}

bool SerialRotationReader::ProcessReceivedMessage()
{
    if (!data_received) return false;
    if (fully_received_message == "") return false;
    auto index = 0;

    std::string temp = "";

    while (fully_received_message[index] != ',')
    {
        temp += fully_received_message[index];
        index++;
    }

    rpm = std::stod(temp);
    temp = "";
    index++;

    while (fully_received_message[index] != '\n')
    {
        temp += fully_received_message[index];
        index++;
    }

    angle = std::stod(temp);

    return true;
}

bool SerialRotationReader::CloseSerialPort()
{
    if (connected)
    {
        connected = false;
        CloseHandle(io_handler);
        return true;
    }
    return false;
}

SerialRotationReader::~SerialRotationReader()
{
    if (connected)
    {
        connected = false;
        CloseHandle(io_handler);
    }
}
