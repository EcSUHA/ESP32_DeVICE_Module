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
#include "ESP32_DeVICE_Initialize.h"







/**
 * -------------------------------------------------------------------------------------------------
 *  FName: Initialize Fn
 *  Desc: Initializion of SCDE Function Callbacks of an new loaded module
 *  Info: Stores Module-Information (Function Callbacks) to SCDE-Root
 *  Para: SCDERoot_t* SCDERootptr -> ptr to SCDE Data Root
 *  Rets: ? unused
 *--------------------------------------------------------------------------------------------------
 */
int 
ESP32_DeVICE_Initialize(SCDERoot_t* SCDERootptr)
{
  // make data root locally available
  SCDERoot_at_ESP32_DeVICE_M = SCDERootptr;

  // make locally available from data-root: SCDEFn (Functions / callbacks) for faster operation
  SCDEFn_at_ESP32_DeVICE_M = SCDERootptr->SCDEFn;

// -------------------------------------------------------------------------------------------------

  #if ESP32_DeVICE_Module_DBG >= 3
  SCDEFn_at_ESP32_DeVICE_M->Log3Fn(
       ESP32_DeVICE_ProvidedByModule.typeName
	  ,ESP32_DeVICE_ProvidedByModule.typeNameLen
	  ,3
	  ,"Executing InitializeFn of Module '%.*s' to make it useable."
	  ,ESP32_DeVICE_ProvidedByModule.typeNameLen
	  ,ESP32_DeVICE_ProvidedByModule.typeName);
  #endif

// -------------------------------------------------------------------------------------------------

  return 0;
}




