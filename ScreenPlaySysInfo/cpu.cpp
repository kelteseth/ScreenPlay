#include "cpu.h"
#include <QtQml/qqml.h>

#define STATUS_SUCCESS					0
#define STATUS_INFO_LENGTH_MISMATCH	0xC0000004

CPU::CPU(QObject *parent) : QObject(parent)
{

}


void CPU::update()
{
//    long status = 0;
//    unsigned long bufSize = c_BufferSize;
//    byte* buf = (bufSize > 0) ? new BYTE[bufSize] : nullptr;



//    status = c_NtQuerySystemInformation(SystemProcessorPerformanceInformation, buf, bufSize, &size);

//    switch (status) {
//    case STATUS_INFO_LENGTH_MISMATCH:
//        qWarning() << "Warning: Status info length mismatch!";
//        break;
//    case STATUS_SUCCESS:

//        break;
//    default:
//        break;
//    }
}
