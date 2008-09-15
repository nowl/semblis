extern "C" {

#include "semblis.h"
#include "ForeignMacros.h"

#include "../../common/Buffer.h"

}

/*
FOREIGN_FUNCTION(Buffer_func_3_0) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to Buffer must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    FF_GET_NEXT_ARG;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 2 to Buffer must be a pointer (integer)");

    char* Buffer = (char *)data_get_integer(curData);

    FF_GET_NEXT_ARG;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 3 to Buffer must be an integer");

    unsigned int len = (unsigned int)data_get_integer(curData);

    data result = buffer->Buffer(Buffer, len);

    FF_RET_INT(result);
}

FOREIGN_FUNCTION(clearBuffer_func_1_1) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to clearBuffer must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    buffer->clearBuffer();

    FF_RET_TRUE;
}
*/

FOREIGN_FUNCTION(Buffer_func_0_0) {
    FF_START;

    Buffer* result = new Buffer();

    FF_RET_INT(result);
}

FOREIGN_FUNCTION(clearBuffer_func_1_1) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to clearBuffer must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    buffer->clearBuffer();

    FF_RET_TRUE;
}

FOREIGN_FUNCTION(printBuffer_func_3_2) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to printBuffer must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    FF_GET_NEXT_ARG;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 2 to printBuffer must be an integer");

    int begin = (int)data_get_integer(curData);

    FF_GET_NEXT_ARG;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 3 to printBuffer must be an integer");

    int end = (int)data_get_integer(curData);

    buffer->printBuffer(begin, end);

    FF_RET_TRUE;
}

FOREIGN_FUNCTION(printBuffer_func_1_3) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to printBuffer must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    buffer->printBuffer();

    FF_RET_TRUE;
}

FOREIGN_FUNCTION(setBuffer_func_3_4) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to setBuffer must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    FF_GET_NEXT_ARG;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 2 to setBuffer must be a pointer (integer)");

    char* pnt = (char *)data_get_integer(curData);

    FF_GET_NEXT_ARG;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 3 to setBuffer must be an integer");

    unsigned int len = (unsigned int)data_get_integer(curData);

    buffer->setBuffer(pnt, len);

    FF_RET_TRUE;
}

FOREIGN_FUNCTION(resetBuffer_func_2_5) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to resetBuffer must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    FF_GET_NEXT_ARG;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 2 to resetBuffer must be an integer");

    unsigned int frontBuffer = (unsigned int)data_get_integer(curData);

    unsigned char* result = buffer->resetBuffer(frontBuffer);

    FF_RET_INT(result);
}

FOREIGN_FUNCTION(resetBuffer_func_1_6) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to resetBuffer must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    unsigned char* result = buffer->resetBuffer();

    FF_RET_INT(result);
}

FOREIGN_FUNCTION(remainBackBuffer_func_1_7) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to remainBackBuffer must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    int result = buffer->remainBackBuffer();

    FF_RET_INT(result);
}

FOREIGN_FUNCTION(lockChiehPointerChar_func_3_8) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to lockChiehPointerChar must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    FF_GET_NEXT_ARG;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 2 to lockChiehPointerChar must be an integer");

    unsigned int shift = (unsigned int)data_get_integer(curData);

    FF_GET_NEXT_ARG;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 3 to lockChiehPointerChar must be an integer");

    unsigned char initValue = (unsigned char)data_get_integer(curData);

    unsigned char* result = buffer->lockChiehPointerChar(shift, initValue);

    FF_RET_INT(result);
}

FOREIGN_FUNCTION(lockChiehPointerShort_func_3_9) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to lockChiehPointerShort must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    FF_GET_NEXT_ARG;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 2 to lockChiehPointerShort must be an integer");

    unsigned int shift = (unsigned int)data_get_integer(curData);

    FF_GET_NEXT_ARG;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 3 to lockChiehPointerShort must be an integer");

    unsigned short initValue = (unsigned short)data_get_integer(curData);

    unsigned short* result = buffer->lockChiehPointerShort(shift, initValue);

    FF_RET_INT(result);
}

FOREIGN_FUNCTION(lockChiehPointerInt_func_3_10) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to lockChiehPointerInt must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    FF_GET_NEXT_ARG;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 2 to lockChiehPointerInt must be an integer");

    unsigned int shift = (unsigned int)data_get_integer(curData);

    FF_GET_NEXT_ARG;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 3 to lockChiehPointerInt must be an integer");

    unsigned int initValue = (unsigned int)data_get_integer(curData);

    unsigned long* result = buffer->lockChiehPointerInt(shift, initValue);

    FF_RET_INT(result);
}

FOREIGN_FUNCTION(getChiehPointerChar_func_1_11) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to getChiehPointerChar must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    unsigned char* result = buffer->getChiehPointerChar();

    FF_RET_INT(result);
}

FOREIGN_FUNCTION(getChiehPointerShort_func_1_12) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to getChiehPointerShort must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    unsigned short* result = buffer->getChiehPointerShort();

    FF_RET_INT(result);
}

FOREIGN_FUNCTION(getChiehPointerInt_func_1_13) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to getChiehPointerInt must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    unsigned long* result = buffer->getChiehPointerInt();

    FF_RET_INT(result);
}

FOREIGN_FUNCTION(setChiehPointer_func_2_14) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to setChiehPointer must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    FF_GET_NEXT_ARG;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 2 to setChiehPointer must be an integer");

    unsigned char value = (unsigned char)data_get_integer(curData);

    buffer->setChiehPointer(value);

    FF_RET_TRUE;
}

FOREIGN_FUNCTION(setChiehPointer_func_2_15) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to setChiehPointer must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    FF_GET_NEXT_ARG;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 2 to setChiehPointer must be an integer");

    unsigned short value = (unsigned short)data_get_integer(curData);

    buffer->setChiehPointer(value);

    FF_RET_TRUE;
}

FOREIGN_FUNCTION(setChiehPointer_func_2_16) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to setChiehPointer must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    FF_GET_NEXT_ARG;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 2 to setChiehPointer must be an integer");

    unsigned int value = (unsigned int)data_get_integer(curData);

    buffer->setChiehPointer(value);

    FF_RET_TRUE;
}

FOREIGN_FUNCTION(IncreChiehPointerChar_func_2_17) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to IncreChiehPointerChar must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    FF_GET_NEXT_ARG;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 2 to IncreChiehPointerChar must be an integer");

    unsigned char value = (unsigned char)data_get_integer(curData);

    buffer->IncreChiehPointerChar(value);

    FF_RET_TRUE;
}

FOREIGN_FUNCTION(IncreChiehPointerShort_func_2_18) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to IncreChiehPointerShort must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    FF_GET_NEXT_ARG;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 2 to IncreChiehPointerShort must be an integer");

    unsigned short value = (unsigned short)data_get_integer(curData);

    buffer->IncreChiehPointerShort(value);

    FF_RET_TRUE;
}

FOREIGN_FUNCTION(IncreChiehPointerInt_func_2_19) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to IncreChiehPointerInt must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    FF_GET_NEXT_ARG;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 2 to IncreChiehPointerInt must be an integer");

    unsigned int value = (unsigned int)data_get_integer(curData);

    buffer->IncreChiehPointerInt(value);

    FF_RET_TRUE;
}

FOREIGN_FUNCTION(getBufferLength_func_1_20) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to getBufferLength must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    unsigned int result = buffer->getBufferLength();

    FF_RET_INT(result);
}

FOREIGN_FUNCTION(incrementFirstTwoBytes_func_1_21) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to incrementFirstTwoBytes must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    unsigned char* result = buffer->incrementFirstTwoBytes();

    FF_RET_INT(result);
}

FOREIGN_FUNCTION(getFirstTwoBytes_func_1_22) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to getFirstTwoBytes must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    unsigned short result = buffer->getFirstTwoBytes();

    FF_RET_INT(result);
}

FOREIGN_FUNCTION(setFirstTwoBytes_func_2_23) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to setFirstTwoBytes must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    FF_GET_NEXT_ARG;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 2 to setFirstTwoBytes must be an integer");

    unsigned short value = (unsigned short)data_get_integer(curData);

    unsigned char* result = buffer->setFirstTwoBytes(value);

    FF_RET_INT(result);
}

FOREIGN_FUNCTION(getCurrentPointer_func_1_24) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to getCurrentPointer must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    unsigned char* result = buffer->getCurrentPointer();

    FF_RET_INT(result);
}

FOREIGN_FUNCTION(printCurrentCusor_func_1_25) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to printCurrentCusor must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    buffer->printCurrentCusor();

    FF_RET_TRUE;
}

FOREIGN_FUNCTION(printLength_func_1_26) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to printLength must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    buffer->printLength();

    FF_RET_TRUE;
}

FOREIGN_FUNCTION(setReadingCursor_func_2_27) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to setReadingCursor must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    FF_GET_NEXT_ARG;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 2 to setReadingCursor must be an integer");

    unsigned int location = (unsigned int)data_get_integer(curData);

    buffer->setReadingCursor(location);

    FF_RET_TRUE;
}

FOREIGN_FUNCTION(resetReadingCursor_func_1_28) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to resetReadingCursor must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    buffer->resetReadingCursor();

    FF_RET_TRUE;
}

FOREIGN_FUNCTION(checkBufferSize_func_2_29) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to checkBufferSize must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    FF_GET_NEXT_ARG;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 2 to checkBufferSize must be an integer");

    int bytes = (int)data_get_integer(curData);

    int result = buffer->checkBufferSize(bytes);

    FF_RET_INT(result);
}

FOREIGN_FUNCTION(pushBack_func_2_30) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to pushBack must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    FF_GET_NEXT_ARG;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 2 to pushBack must be a pointer (integer)");

    char* value = (char *)data_get_integer(curData);

    unsigned char* result = buffer->pushBack(value);

    FF_RET_INT(result);
}

FOREIGN_FUNCTION(pushBack_func_3_31) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to pushBack must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    FF_GET_NEXT_ARG;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 2 to pushBack must be a pointer (integer)");

    char* value = (char *)data_get_integer(curData);

    FF_GET_NEXT_ARG;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 3 to pushBack must be an integer");

    int length = (int)data_get_integer(curData);

    unsigned char* result = buffer->pushBack(value, length);

    FF_RET_INT(result);
}

FOREIGN_FUNCTION(pushBack_func_2_32) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to pushBack must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    FF_GET_NEXT_ARG;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 2 to pushBack must be an integer");

    unsigned char value = (unsigned char)data_get_integer(curData);

    unsigned char* result = buffer->pushBack(value);

    FF_RET_INT(result);
}

FOREIGN_FUNCTION(pushBack_func_2_33) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to pushBack must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    FF_GET_NEXT_ARG;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 2 to pushBack must be an integer");

    short value = (short)data_get_integer(curData);

    unsigned char* result = buffer->pushBack(value);

    FF_RET_INT(result);
}

FOREIGN_FUNCTION(pushBack_func_2_34) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to pushBack must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    FF_GET_NEXT_ARG;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 2 to pushBack must be an integer");

    int value = (int)data_get_integer(curData);

    unsigned char* result = buffer->pushBack(value);

    FF_RET_INT(result);
}

FOREIGN_FUNCTION(pushBack_func_2_35) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to pushBack must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    FF_GET_NEXT_ARG;

    if(!FF_IS_REAL)
	FF_RET_ERROR(L"arg 2 to pushBack must be a real");

    float value = (float)data_get_real(curData);

    unsigned char* result = buffer->pushBack(value);

    FF_RET_INT(result);
}

FOREIGN_FUNCTION(enoughFrontBuffer_func_2_36) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to enoughFrontBuffer must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    FF_GET_NEXT_ARG;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 2 to enoughFrontBuffer must be an integer");

    int bytes = (int)data_get_integer(curData);

    bool result = buffer->enoughFrontBuffer(bytes);

    FF_RET_INT(result);
}

FOREIGN_FUNCTION(pushFront_func_2_37) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to pushFront must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    FF_GET_NEXT_ARG;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 2 to pushFront must be an integer");

    unsigned char value = (unsigned char)data_get_integer(curData);

    unsigned char* result = buffer->pushFront(value);

    FF_RET_INT(result);
}

FOREIGN_FUNCTION(pushFront_func_2_38) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to pushFront must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    FF_GET_NEXT_ARG;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 2 to pushFront must be an integer");

    short value = (short)data_get_integer(curData);

    unsigned char* result = buffer->pushFront(value);

    FF_RET_INT(result);
}

FOREIGN_FUNCTION(pushFront_func_2_39) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to pushFront must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    FF_GET_NEXT_ARG;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 2 to pushFront must be an integer");

    int value = (int)data_get_integer(curData);

    unsigned char* result = buffer->pushFront(value);

    FF_RET_INT(result);
}

FOREIGN_FUNCTION(getChar_func_1_40) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to getChar must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    unsigned char result = buffer->getChar();

    FF_RET_INT(result);
}

FOREIGN_FUNCTION(getShort_func_1_41) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to getShort must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    unsigned short result = buffer->getShort();

    FF_RET_INT(result);
}

FOREIGN_FUNCTION(getInt_func_1_42) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to getInt must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    unsigned int result = buffer->getInt();

    FF_RET_INT(result);
}

FOREIGN_FUNCTION(getFloat_func_1_43) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to getFloat must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    float result = buffer->getFloat();

    FF_RET_INT(result);
}

FOREIGN_FUNCTION(getCurrentCursorPosition_func_1_44) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to getCurrentCursorPosition must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    unsigned int result = buffer->getCurrentCursorPosition();

    FF_RET_INT(result);
}

FOREIGN_FUNCTION(getString_func_1_45) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to getString must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    unsigned char* result = buffer->getString();

    FF_RET_INT(result);
}

FOREIGN_FUNCTION(getString_func_2_46) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to getString must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    FF_GET_NEXT_ARG;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 2 to getString must be an integer");

    unsigned int byte = (unsigned int)data_get_integer(curData);

    unsigned char* result = buffer->getString(byte);

    FF_RET_INT(result);
}

FOREIGN_FUNCTION(popBackChar_func_1_47) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to popBackChar must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    unsigned char result = buffer->popBackChar();

    FF_RET_INT(result);
}

FOREIGN_FUNCTION(popBackShort_func_1_48) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to popBackShort must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    short result = buffer->popBackShort();

    FF_RET_INT(result);
}

FOREIGN_FUNCTION(popBackInt_func_1_49) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to popBackInt must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    int result = buffer->popBackInt();

    FF_RET_INT(result);
}

FOREIGN_FUNCTION(popFrontChar_func_1_50) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to popFrontChar must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    unsigned char result = buffer->popFrontChar();

    FF_RET_INT(result);
}

FOREIGN_FUNCTION(popFrontShort_func_1_51) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to popFrontShort must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    short result = buffer->popFrontShort();

    FF_RET_INT(result);
}

FOREIGN_FUNCTION(popFrontInt_func_1_52) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to popFrontInt must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    int result = buffer->popFrontInt();

    FF_RET_INT(result);
}

/*
FOREIGN_FUNCTION(getMsg_func_1_53) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to getMsg must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    unsigned char* result = buffer->getMsg();

    FF_RET_INT(result);
}

FOREIGN_FUNCTION(memfill_func_3_54) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to memfill must be a Buffer* pointer (integer)");

    Buffer* buffer = (Buffer*)data_get_integer(curData);

    FF_GET_NEXT_ARG;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 2 to memfill must be an integer");

    char x = (char)data_get_integer(curData);

    FF_GET_NEXT_ARG;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 3 to memfill must be an integer");

    int n = (int)data_get_integer(curData);

    buffer->memfill(x, n);

    FF_RET_TRUE;
}
*/
