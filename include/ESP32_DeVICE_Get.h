#ifndef ESP32_DeVICE_GET_H
#define ESP32_DeVICE_GET_H



//
strTextMultiple_t*
ESP32_DeVICE_Get(Common_Definition_t* Common_Definition
                ,uint8_t *getArgsText
                ,size_t getArgsTextLen);



Entry_String_t*
ESP32_DeVICE_GetV2(Entry_Definition_t* p_Definition
                ,char **argv
                ,int argc);


#endif // ESP32_DeVICE_GET_H
