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
#include "ESP32_DeVICE_State.h"







/**
 * -------------------------------------------------------------------------------------------------
 *  FName: ESP32_DeVICE_State
 *  Desc: FN is called when a Definition of this Module gets, and should set an state update. 
 *        Normally called from setstate cmd when executing state (=readings?) recovery from save .cfg
 *  Info: An FN for State in an Module is optional!
 *  Para: const Common_Definition_t *ESP32_DeVICE_Definition -> Definition that should execute an status update
 *        const time_t stateTiSt -> time stamp for status update / reading value
 *        const String_t stateNameString -> state Name for status update / reading value
 *        const String_t stateValueString -> state Value for the status update / reading value
 *        const String_t stateMimeString -> state Mime for the status update / reading value
 *  Rets: Entry_String_t* -> singly linked tail queue element which stores one xString_t string
 *                                  as response text in allocated memory. NULL=no response text
 * -------------------------------------------------------------------------------------------------
 */
Entry_String_t*
ESP32_DeVICE_State(Common_Definition_t *Common_Definition
	,const time_t stateTiSt
	,const String_t stateNameString
	,const String_t stateValueString
	,const String_t stateMimeString)
{
  // Fn return message. NULL = no return message
  Entry_String_t *retMsg_Entry_String = NULL;
	
  // make common ptr to modul specific ptr
  Entry_ESP32_DeVICE_Definition_t* ESP32_DeVICE_Definition =
		(Entry_ESP32_DeVICE_Definition_t*) Common_Definition;

// -------------------------------------------------------------------------------------------------

  #if ESP32_DeVICE_Module_DBG >= 5
  // prepare TiSt-string for LogFn
  string_t timeString =
  	SCDEFn_at_ESP32_DeVICE_M->get_formated_date_time_fn(stateTiSt);

  SCDEFn_at_ESP32_DeVICE_M->Log3Fn(Common_Definition->name
	,Common_Definition->nameLen
	,5
	,"Executing StateFn of Module '%.*s' for Definition '%.*s'. "
         "Should set State for '%.*s' with Value '%.*s' and Mime '%.*s'. TimeStamp '%.*s'."
	,ESP32_DeVICE_Definition->common.module->provided->typeNameLen
	,ESP32_DeVICE_Definition->common.module->provided->typeName
	,Common_Definition->nameLen
	,Common_Definition->name
	,stateNameString.len
	,stateNameString.p_char
	,stateValueString.len
	,stateValueString.p_char
	,stateMimeString.len
	,stateMimeString.p_char
	,timeString.len
	,timeString.p_char);

  // free TiSt-string from LogFn
  free(timeString.p_char);
  #endif

/*
// ------------------------------------------------------------------------------------------------

  // build an temp KEY=VALUE string
  xString_t argsString;

  // check if there is a KEY
  if (!stateNameString.length) {

	// alloc mem for the singly linked tail queue element that stores the Return Message
	retMsg_Entry_String = 
		malloc(sizeof(xMultipleStringSLTQE_t));

	// fill Return Message with error text
	retMsg_Entry_String->string.length = 
		asprintf(&retMsg_Entry_String->string.characters
			,"Error in StateFn! Fn called without State Name");

	return retMsg_Entry_String;
  }

  argsString.length = 
	asprintf(&argsString.characters
		,"%.*s=%.*s"
		,stateNameString.length
		,stateNameString.characters
		,stateValueString.length
		,stateValueString.characters);

// ------------------------------------------------------------------------------------------------

  // Parse State-Args (KEY=VALUE) protocol -> gets parsedKVInput in allocated mem, NULL = ERROR
  parsedKVInputArgs_t *parsedKVInput = 
	SCDEFn_at_ESP32_DeVICE_M->ParseKVInputArgsFn(ESP32_DeVICE_Set_NUMBER_OF_IK	// Num Implementated KEYs MAX
		,ESP32_DeVICE_Set_ImplementedKeys			// Implementated Keys
		,argsString.characters
		,argsString.length);

  // NULL? Parsing reports an problem. args contain: unknown keys, double keys, ...?
  if (!parsedKVInput) {

	// alloc mem for the singly linked tail queue element that stores the Return Message
	retMsg_Entry_String = 
		malloc(sizeof(xMultipleStringSLTQE_t));

	// fill Return Message with error text
	retMsg_Entry_String->string.length = 
		asprintf(&retMsg_Entry_String->string.characters
			,"Parsing Error! Args '%.*s' not taken! Check the KEYs!"
			,argsString.length
			,argsString.characters);

	// free the temp KEY=VALUE string
	if (argsString.characters) free (argsString.characters);

	return retMsg_Entry_String;
  }

// -------------------------------------------------------------------------------------------------

  // free allocated memory for query result key-field
  free(parsedKVInput);

  // free the temp KEY=VALUE string
  if (argsString.characters) free (argsString.characters);
*/
  return retMsg_Entry_String;
}






