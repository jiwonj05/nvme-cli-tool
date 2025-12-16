#include <Windows.h>
#include <winioctl.h>
#include <ntddstor.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

// NVMe Identify Controller Data Structure
typedef struct _NVME_IDENTIFY_CONTROLLER_DATA {
    USHORT VID;                     // PCI Vendor ID (0-1)
    USHORT SSVID;                   // PCI Subsystem Vendor ID (2-3)
    CHAR   SN[20];                  // Serial Number (4-23)
    CHAR   MN[40];                  // Model Number (24-63)
    CHAR   FR[8];                   // Firmware Revision (64-71)
    UCHAR  RAB;                     // Recommended Arbitration Burst (72)
    UCHAR  IEEE[3];                 // IEEE OUI Identifier (73-75)
    // ... rest of structure omitted
} NVME_IDENTIFY_CONTROLLER_DATA, * PNVME_IDENTIFY_CONTROLLER_DATA;

int main(int argc, char** argv) {
    // define symbolic link 
    LPCSTR symlnk = "\\\\.\\PhysicalDrive0";
    // createfile 
    HANDLE hDevice;
    hDevice = CreateFile(symlnk, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hDevice == INVALID_HANDLE_VALUE) {
        printf("CreateFile function failed\n");
        return 1;
    }
    // structure storage protocol command
    BOOL result;
    ULONG bufferSize = sizeof(STORAGE_PROPERTY_QUERY) + sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA) + 4096;

    // Allocate buffer for use
    BYTE* buffer = (BYTE*)malloc(bufferSize);

    if (buffer == NULL) {
        printf("[-] Memory allocation failed. Out of RAM?\n");
        CloseHandle(hDevice);
        return -1;
    }
    ZeroMemory(buffer, bufferSize);

    STORAGE_PROPERTY_QUERY* query = (STORAGE_PROPERTY_QUERY*)buffer;
    query->PropertyId = StorageAdapterProtocolSpecificProperty;
    query->QueryType = PropertyStandardQuery;

    STORAGE_PROTOCOL_SPECIFIC_DATA* protocolData = (STORAGE_PROTOCOL_SPECIFIC_DATA*)query->AdditionalParameters;
    protocolData->ProtocolType = ProtocolTypeNvme;
    protocolData->DataType = NVMeDataTypeIdentify;
    protocolData->ProtocolDataRequestValue = 1;
    protocolData->ProtocolDataRequestSubValue = 0;
    protocolData->ProtocolDataOffset = sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA);
    protocolData->ProtocolDataLength = 4096;

    //in buffer
    //out buffer
    BYTE* outputBuffer = (BYTE*)malloc(bufferSize);
    if (outputBuffer == NULL) {
        printf("[-] Memory allocation failed. Out of RAM?\n");
        free(buffer);
        CloseHandle(hDevice);
        return -1;
    }

    DWORD byteReturned = 0;
    result = DeviceIoControl(
        hDevice,
        IOCTL_STORAGE_QUERY_PROPERTY,
        buffer,
        bufferSize,
        outputBuffer,
        bufferSize,
        &byteReturned,
        NULL
    );
    // define ioctl command 
    if (result) {
        printf("Data recieved.\n");

        STORAGE_PROTOCOL_DATA_DESCRIPTOR* dataDescriptor = (STORAGE_PROTOCOL_DATA_DESCRIPTOR*)outputBuffer;
        BYTE* pData = (BYTE*)&dataDescriptor->ProtocolSpecificData;
        pData += dataDescriptor->ProtocolSpecificData.ProtocolDataOffset;

        PNVME_IDENTIFY_CONTROLLER_DATA nvmeData = (PNVME_IDENTIFY_CONTROLLER_DATA)pData;

        char model[41];
        memcpy(model, nvmeData->MN, 40);
        model[40] = '\0';

        char serial[21];
        memcpy(serial, nvmeData->SN, 20);
        serial[20] = '\0';

        printf("Model:  %s\n", model);
        printf("Serial: %s\n", serial);
    }
    else {
        printf("[-] Failed. Error: %d\n", GetLastError());
    }
    free(buffer);
    free(outputBuffer);
    CloseHandle(hDevice);
    return 0;
}