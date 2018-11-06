// SCDE-Module ESP32_Control

#ifndef ESP32_CONTROL_MODULE_H
#define ESP32_CONTROL_MODULE_H



#include "SCDE_s.h"


#include "WebIf_Module global types.h"



/* 
 * ESP32Control_Definition stores values for operation valid only for the defined instance of an
 * loaded module. Values are initialized by HCTRLD an the loaded module itself.
 */
typedef struct ESP32_Control_Definition_s {

  Common_Definition_t common;		// ... the common part of the definition

  WebIf_Provided_t WebIf_Provided;	// provided data for WebIf

  uint32_t demo;			// custom data

  } ESP32_Control_Definition_t;



/*
 *  provided by module
 */
strTextMultiple_t* ESP32_Control_Attribute(Common_Definition_t* Common_Definition, const uint8_t *attrCmdText, const size_t attrCmdTextLen, const uint8_t *attrNameText, const size_t attrNameTextLen, uint8_t **attrValTextPtr, size_t *attrValTextLenPtr);
strTextMultiple_t* ESP32_Control_Define(Common_Definition_t *Common_Definition);
int ESP32_Control_Initialize(SCDERoot_t* SCDERoot);
strTextMultiple_t* ESP32_Control_Rename(Common_Definition_t *Common_Definition, uint8_t *newName, size_t newNameLen, uint8_t *oldName, size_t oldNameLen);
strTextMultiple_t* ESP32_Control_Set(Common_Definition_t* Common_Definition, uint8_t *setArgs, size_t setArgsLen);
strTextMultiple_t* ESP32_Control_Undefine(Common_Definition_t* Common_Definition);
strTextMultiple_t* ESP32_Control_State(Common_Definition_t *Common_Definition, time_t readingTiSt, uint8_t *readingName, size_t readingNameLen, uint8_t *readingValue, size_t readingValueLen);
strTextMultiple_t* ESP32_Control_Shutdown(Common_Definition_t* Common_Definition);

/*
 *  helpers
 */



#endif /*ESP32_CONTROL_MODULE_H*/
