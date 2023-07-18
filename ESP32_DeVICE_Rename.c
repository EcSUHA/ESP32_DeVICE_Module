/* #################################################################################################
 *
 *  Function: ESP32_DeVICE Module for SCDE (Smart Connected Device Engine)
 *
 *  DeVICE -> DeVICE Feature
 *
 *  It is used to control an IoT Device (ESP32 - SoC and OS) that runs the SCDE 
 *  (Smart Connected Device Engine)
 *
 *  ESP 8266EX & ESP32 SOC Activities ...
 *  Copyright by EcSUHA
 *
 *  Created by Maik Schulze, Sandfuhren 4, 38448 Wolfsburg, Germany for EcSUHA.de 
 *
 *  MSchulze780@GMAIL.COM
 *  Copyright by EcSUHA
 *
 *  EcSUHA - ECONOMIC SURVEILLANCE AND HOME AUTOMATION - WWW.EcSUHA.DE
 * #################################################################################################
 */



#include <ProjectConfig.h>
#include <esp8266.h>
#include <Platform.h>



// the Smart-Connected-Device-Engine structures & types ...
#include <SCDE_s.h>

// provides WebIf, need the structures & types ...
#include "WebIf_Module_s.h"
#include "WebIf_Module.h"

// this Modules structures & types ...
#include "ESP32_DeVICE_Module.h"
#include "ESP32_DeVICE_Rename.h"








/**
 * -------------------------------------------------------------------------------------------------
 *  FName: ESP32_DeVICE_Rename
 *  Desc: FN is called when a Definition of this Module was renamed. Module can do further things here. 
 *  Info: An FN for Rename is optional!
 *  Para: Entry_ESP32_DeVICE_Definition_t *ESP32_DeVICE_Definition -> ESP32_DeVICE Definition that was renamed
 *        uint8_t *newName -> ptr to the new name (do we need it???)
 *        size_t newNameLen -> length of the new name
 *        uint8_t *oldName -> ptr to old name
 *        size_t oldNameLen -> length of the old name
 *  Rets: strTextMultiple_t* -> response text in allocated memory, NULL=no text
 * -------------------------------------------------------------------------------------------------
 */
strTextMultiple_t*
ESP32_DeVICE_Rename(Common_Definition_t *Common_Definition
	,uint8_t *newName
	,size_t newNameLen
	,uint8_t *oldName
	,size_t oldNameLen)
{
	
  // for Fn response msg
  strTextMultiple_t *multipleRetMsg = NULL;
	
  // make common ptr to modul specific ptr
  Entry_ESP32_DeVICE_Definition_t* ESP32_DeVICE_Definition =
		(Entry_ESP32_DeVICE_Definition_t*) Common_Definition;

// -------------------------------------------------------------------------------------------------

  #if ESP32_DeVICE_Module_DBG >= 5
  SCDEFn_at_ESP32_DeVICE_M->Log3Fn(Common_Definition->name
		,Common_Definition->nameLen
		,5
		,"Executing RenameFn of Module '%.*s'. "
		 "An definition is renamed from old name '%.*s' to new name '%.*s'."
		,ESP32_DeVICE_Definition->common.module->provided->typeNameLen
		,ESP32_DeVICE_Definition->common.module->provided->typeName
		,oldName
		,oldNameLen
		,newName
		,newNameLen);
  #endif
	
// -------------------------------------------------------------------------------------------------

  return multipleRetMsg;
}
	






