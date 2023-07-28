#include "SerialRotationReader.h"

int main()
{
    char com_port[] = "\\\\.\\COM7";
    DWORD COM_BAUD_RATE = CBR_115200;
    SerialRotationReader rotation_reader(com_port, COM_BAUD_RATE);

    if (rotation_reader.connected)
    {
        while (true)
        {
            if (GetAsyncKeyState('Q') & 0x8000)
            {
                break;
            }

            rotation_reader.ReadBuffer();

            if (rotation_reader.data_received)
            {
                rotation_reader.ProcessReceivedMessage();
                std::cout << rotation_reader.rpm << ' ' << rotation_reader.angle << std::endl;
            }
        }
        rotation_reader.CloseSerialPort();
    }
    else
    {
        std::cout << "Serial port not connected" << std::endl;
    }
}
