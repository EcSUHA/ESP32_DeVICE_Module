/* #################################################################################################
 *
 *  ESP32_Control Module for SCDE (Smart Connected Device Engine)
 *  ESP32_SoCControl Module for SCDE (Smart Connected Device Engine)
 *
 *  SoC -> SoC Feature.
 *  It is used to Control the SoC (WiFi, ...).
 *
 *
 *  Created by Maik Schulze, Sandfuhren 4, 38448 Wolfsburg, Germany for EcSUHA.de 
 *
 *  MSchulze780@GMAIL.COM
 *  Copyright by EcSUHA
 *
 *  ESP 8266EX & ESP32 SOC Activities ...
 *
 *  EcSUHA - ECONOMIC SURVEILLANCE AND HOME AUTOMATION - WWW.EcSUHA.DE
 * #################################################################################################
 */



#include <ProjectConfig.h>
#include <esp8266.h>
#include <Platform.h>

#include <SCDE_s.h>

#include "ESP32_Control_Module.h"

#include "SCDE_Main.h"


//#include "lwip/sockets.h"


static char tag[] = "ESP32Control";




// -------------------------------------------------------------------------------------------------

// developers debug information 0 = off 5 max?
#define SCDED_DBG 5
#define SCDEH_DBG 5

// -------------------------------------------------------------------------------------------------



// -------------------------------------------------------------------------------------------------

// set default build verbose - if no external override
#ifndef ESP32_Control_Module_DBG  
#define ESP32_Control_Module_DBG  5	// 5 is default
#endif 

// -------------------------------------------------------------------------------------------------



// make data root locally available
static SCDERoot_t* SCDERoot;

// make locally available from data-root: the SCDEFn (Functions / callbacks) for operation
static SCDEFn_t* SCDEFn;

// -------------------------------------------------------------------------------------------------

#include <WebIf_EspFSStdFileTX.h>
#include <WebIf_EspFSAdvFileTX.h>

#include "HTools_cgi.h"
#include <ServAPCfg_tpl.h>
#include <CGI_Redirect.h>











/*
 * Implemented Values for Keys
 */

SelectAData DisEn[] = {  //ID, Text MAX CGI LEN BEACHTEN!!!
  {0,"Disabled"},
  {1,"Enabled"}, 
  {0, NULL}
};

SelectAData MaxConn[] = {  //ID, Text MAX CGI LEN BEACHTEN!!!
  {0,"1"},
  {1,"2"},
  {2,"3"},
  {3,"4"},
  {4,"5"},
  {0, NULL}
};

SelectAData Channel[] = {  //ID, Text MAX CGI LEN BEACHTEN!!!
  {1,"CH_1"},
  {2,"CH_2"},
  {3,"CH_3"},
  {4,"CH_4"},
  {5,"CH_5"},
  {6,"CH_6"},
  {7,"CH_7"},
  {8,"CH_8"},
  {9,"CH_9"},
  {10,"CH_10"},
  {11,"CH_11"},
  {12,"CH_12"},
  {13,"CH_13"},
  {14,"CH_14"},
  {0, NULL}
};

SelectAData SSID_H[] = {  //ID, Text MAX CGI LEN BEACHTEN!!!
  {0,"Visible"},
  {1,"Hidden"},
  {0, NULL}
};

SelectAData NrAP[] = {  //ID, Text MAX CGI LEN BEACHTEN!!!
  {0,"0"},
  {1,"1"},
  {2,"2"},
  {3,"3"},
  {4,"4"},
  {5,"5"},
  {0, NULL}
};

SelectAData PhMode[] = {  //ID, Text MAX CGI LEN BEACHTEN!!!
  {1,"802.11b"},
  {2,"802.11g"},
  {3,"802.11n"},
  {0, NULL}
};

SelectAData wifiCountry[] = {  //ID, Text MAX CGI LEN BEACHTEN!!!
  {0,"CN"},
  {1,"JP"},
  {2,"US"},
  {3,"EU"},
  {0, NULL}
};

SelectAData Auth_M[] = {  //authMode[] = {  //ID, Text MAX CGI LEN BEACHTEN!!!
  {0,"OPEN"},
  {1,"WEP"},
  {2,"WPA_PSK"},
  {3,"WPA2_PSK"},
  {4,"WPA_WPA2_PSK"},
  {5,"MAX"},
  {0, NULL}
};

SelectAData wifiBandwidth[] = {  //ID, Text MAX CGI LEN BEACHTEN!!!
  {0,"HT20"},
  {1,"HT40"},
  {2,"HT??"},
  {0, NULL}
};









/**
 * -------------------------------------------------------------------------------------------------
 *  DName: ESP32_Control_ActiveResourcesDataA_forWebIf
 *  Desc: Resource-Content-structure of active Directory - PART A (Resource-Data-Row)
 *  Data: WebIf_ActiveResourcesDataA_t[X] from HttpD.h
 * -------------------------------------------------------------------------------------------------
 */
// Content:   AllowedMethodBF          | AllowedDocMimeBF  | AllowedSchemeBF |free|CgiNo| EnaByBit | Url
const WebIf_ActiveResourcesDataA_t ESP32_Control_ActiveResourcesDataA_forWebIf[] = {  //ICACHE_RODATA_ATTR = 

  // SOC Hardware Cfg
  { 0b00000000000000000000000000001010, 0b0000000000001110, 0b0000000000000001,  0,  0, 0b00000000, "/SoCHWCfg"}

  // WiFi Stations Cfg
 ,{ 0b00000000000000000000000000001010, 0b0000000000001110, 0b0000000000000001,  0,  1, 0b00000000, "/WiFi/StationCfg"}

  // WIFI Q-Connect PAGES
 ,{ 0b00000000000000000000000000001010, 0b0000000000000010, 0b0000000000000001,  0,  2, 0b00000000, "/WiFi/QConnect"}
 ,{ 0b00000000000000000000000000001010, 0b0000000000001000, 0b0000000000000001,  0,  3, 0b00000000, "/WiFi/WiFiScan"}
 ,{ 0b00000000000000000000000000001010, 0b0000000000000100, 0b0000000000000001,  0,  4, 0b00000000, "/WiFi/Connect"}
 ,{ 0b00000000000000000000000000001010, 0b0000000000000100, 0b0000000000000001,  0,  5, 0b00000000, "/WiFi/Setmode"}

  // Service Access Point Cfg
 ,{ 0b00000000000000000000000000001010, 0b0000000000001110, 0b0000000000000001,  0,  6, 0b00000000, "/WiFi/ServAPCfg"}

  // TimeStamp Cfg
 ,{ 0b00000000000000000000000000001010, 0b0000000000001110, 0b0000000000000001,  0,  7, 0b00000000, "/TiStCfg"}

  // Firmware Update
 ,{ 0b00000000000000000000000000001010, 0b0000000000001110, 0b0000000000000001,  0,  8, 0b00000000, "/Firmware"}

  // Redirects
 ,{ 0b00000000000000000000000000001010, 0b0000000000000001, 0b0000000000000001,  0,  9, 0b00000000, "/WiFi"}	// nomime
 ,{ 0b00000000000000000000000000001010, 0b0000000000000001, 0b0000000000000001,  0,  9, 0b00000000, "/WiFi/"}	// nomime
 ,{ 0b00000000000000000000000000001010, 0b0000000000000001, 0b0000000000000001,  0, 10, 0b00000000, "/"}	// nomime
 ,{ 0b00000000000000000000000000001010, 0b0000000000000010, 0b0000000000000001,  0, 10, 0b00000000, "/index"}	// .htm

  // Secret services
 ,{ 0b00000000000000000000000000001010, 0b0000000000100000, 0b0000000000000001,  0, 11, 0b00000000, "/32MBitFlash"}
 ,{ 0b00000000000000000000000000001010, 0b0000000000100000, 0b0000000000000001,  0, 12, 0b00000000, "/wfs"}

 ,{0,0,0,0,0,0,"*"}

  };



/**
 * -------------------------------------------------------------------------------------------------
 *  DName: ESP32_Control_ActiveResourcesDataB_forWebIf
 *  Desc: Resource-Content-structure of active Directory - PART B (Procedure-Call-Data-Row) 
 *  Data: WebIf_ActiveResourcesDataB_t[X] from HttpD.h
 * -------------------------------------------------------------------------------------------------
 */
// CgiFucID=(No.<<16)+AllowedSchemeBF |      cgi            |     cgi_data
const WebIf_ActiveResourcesDataB_t ESP32_Control_ActiveResourcesDataB_forWebIf[] =  {  //ICACHE_RODATA_ATTR =

  // ### SOC Hardware Cfg ###
  {( 0<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	SoCHWCfg_tpl	}
 ,{( 0<<16) +	0b0000000000000100,	NULL,NULL}//	SoCHWCfg_cgi,		NULL		}
 ,{( 0<<16) +	0b0000000000001000,	NULL,NULL}//	SoCHWCfg_jso,		NULL		}

  // ### WiFi Station Cfg ###
 ,{( 1<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	StationCfg_tpl	}
 ,{( 1<<16) +	0b0000000000000100,	NULL,NULL}//	StationCfg_cgi,		NULL		}
 ,{( 1<<16) +	0b0000000000001000,	NULL,NULL}//	StationCfg_jso,		NULL		}

  // ### WIFI Q-Connect PAGES ###
 ,{( 2<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	WifiQConnect_tpl}
 ,{( 3<<16) +	0b0000000000001000,	NULL,NULL}//	WiFiScan_jso,		NULL		}
 ,{( 4<<16) +	0b0000000000000100,	NULL,NULL}//	WiFiConnect_cgi,	NULL		}
 ,{( 5<<16) +	0b0000000000000100,	NULL,NULL}//	WifiSetMode_cgi,	NULL		}	

  // ### Service Access Point Cfg ###
 ,{( 6<<16) +	0b0000000000000010,	WebIf_EspFSAdvFileTX,	ServAPCfg_tpl	}
 ,{( 6<<16) +	0b0000000000000100,	NULL,NULL}//	ServAPCfg_cgi,		NULL		}
 ,{( 6<<16) +	0b0000000000001000,	NULL,NULL}//	ServAPCfg_jso,		NULL		}

  // ### TimeStamp Cfg ###
 ,{( 7<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	TiStCfg_tpl	}
 ,{( 7<<16) +	0b0000000000000100,	NULL,NULL}//	TiStCfg_cgi,		NULL		}
 ,{( 7<<16) +	0b0000000000001000,	NULL,NULL}//	TiStCfg_jso,		NULL		}

  // ### Firmware Update ###
 ,{( 8<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	FirmwareUd_tpl	}
 ,{( 8<<16) +	0b0000000000000100,	NULL,NULL}//	FirmwareUd_cgi,		NULL		}
 ,{( 8<<16) +	0b0000000000001000,	NULL,NULL}//	FirmwareUd_jso,		NULL		}

  // ### Redirects ###
 ,{( 9<<16) +	0b0000000000000001,	cgiRedirect,	"/WiFi/QConnect.htm"		}
 ,{(10<<16) +	0b0000000000000001,	cgiRedirect,	"/CoNTROL_8S-1C-1ADC.htm"	}
 ,{(10<<16) +	0b0000000000000010,	cgiRedirect,	"/CoNTROL_8S-1C-1ADC.htm"	}

  // ### Secret services ###
 ,{(11<<16) +	0b0000000000100000,	ReadFullFlash_cgi,	NULL		}		//.bin
 ,{(12<<16) +	0b0000000000100000,	NULL,NULL}//	WriteFileSystem_cgi,	NULL		}		//.bin

  };



/**
 * -------------------------------------------------------------------------------------------------
 *  DName: ESP32_Control_Module
 *  Desc: Data 'Provided By Module' for the ESP32Control module (functions + infos this module provides
 *        to SCDE)
 *  Data: 
 * -------------------------------------------------------------------------------------------------
 */
ProvidedByModule_t
ESP32_Control_ProvidedByModule = {
// A-Z order

  "ESP32_Control"			// Type-Name of module -> should be same name as libfilename.so !
  ,13				   	// size of Type-Name

  ,NULL					// Add

  ,ESP32_Control_Attribute		// Attribute

  ,ESP32_Control_Define			// Define

  ,NULL					// Delete

  ,NULL					// Except

  ,NULL					// Get

  ,NULL					// IdleCb

  ,ESP32_Control_Initialize		// Initialize

  ,NULL					// Notify

  ,NULL					// Parse

  ,NULL					// Read

  ,NULL					// Ready

  ,ESP32_Control_Rename			// Rename

  ,ESP32_Control_Set			// Set

  ,ESP32_Control_Shutdown		// Shutdown

  ,ESP32_Control_State			// State

  ,NULL					// Sub

  ,ESP32_Control_Undefine		// Undefine

  ,NULL					// DirectRead

  ,NULL					// DirectWrite

//  ,NULL		 		// FnProvided

  ,sizeof(ESP32_Control_Definition_t)	// Size of modul specific definition structure (Common_Definition_t + X)
};



/**
 * -------------------------------------------------------------------------------------------------
 *  FName: ESP32_Control_Initialize
 *  Desc: Initializion of SCDE Function Callbacks of an new loaded module
 *  Info: Stores Module-Information (Function Callbacks) to SCDE-Root
 *  Para: SCDERoot_t* SCDERootptr -> ptr to SCDE Data Root
 *  Rets: ? unused
 *--------------------------------------------------------------------------------------------------
 */
int 
ESP32_Control_Initialize(SCDERoot_t* SCDERootptr)
  {

  // make data root locally available
  SCDERoot = SCDERootptr;

  // make locally available from data-root: SCDEFn (Functions / callbacks) for faster operation
  SCDEFn = SCDERootptr->SCDEFn;

  SCDEFn->Log3Fn(ESP32_Control_ProvidedByModule.typeName
		  ,ESP32_Control_ProvidedByModule.typeNameLen
		  ,3
		  ,"InitializeFn called. Type '%.*s' now useable.\n"
		  ,ESP32_Control_ProvidedByModule.typeNameLen
		  ,ESP32_Control_ProvidedByModule.typeName);

  return 0;

  }



/**
 * -------------------------------------------------------------------------------------------------
 *  FName: ESP32_Control_Define
 *  Desc: Finalizes the defines a new "device" of 'ESP32Control' type. Contains devicespecific init code.
 *  Info: 
 *  Para: Common_Definition_t *Common_Definition -> prefilled ESP32Control Definition
 *        char *Definition -> the last part of the CommandDefine arg* 
 *  Rets: strTextMultiple_t* -> response text NULL=no text
 * -------------------------------------------------------------------------------------------------
 */
strTextMultiple_t* ICACHE_FLASH_ATTR 
ESP32_Control_Define(Common_Definition_t *Common_Definition)//, const char *Definition)
  {

  // for Fn response msg
  strTextMultiple_t *retMsg = NULL;

  // make common ptr to modul specific ptr
  ESP32_Control_Definition_t* ESP32_Control_Definition =
		  (ESP32_Control_Definition_t*) Common_Definition;

  #if SCDEH_DBG >= 5
  printf("\n|ESP32_Control_Def, Def:%.*s>"
	,ESP32_Control_Definition->common.definitionLen
	,ESP32_Control_Definition->common.definition);
  #endif

// -------------------------------------------------------------------------------------------

  // store FD to Definition. Will than be processed in global loop ... -> THIS MODULE USES NO FD
  ESP32_Control_Definition->common.fd = -1;

// -------------------------------------------------------------------------------------------

  // init WebIf_Provided offset
//  ESP32_Control_Definition->common.link =
//	&ESP32_Control_Definition->WebIf_Provided;

  // check for loaded Module 'WebIf' -> get provided Fn
  ESP32_Control_Definition->WebIf_Provided.WebIf_FnProvided =
	NULL;//(WebIf_FnProvided_t *) SCDEFn->GetFnProvidedByModule("WebIf");

 // Providing data for WebIf? Initialise data provided for WebIf
  if (ESP32_Control_Definition->WebIf_Provided.WebIf_FnProvided) {

	ESP32_Control_Definition->WebIf_Provided.ActiveResourcesDataA =
		(WebIf_ActiveResourcesDataA_t *) &ESP32_Control_ActiveResourcesDataA_forWebIf;

	ESP32_Control_Definition->WebIf_Provided.ActiveResourcesDataB =
		(WebIf_ActiveResourcesDataB_t *) &ESP32_Control_ActiveResourcesDataB_forWebIf;

	}

  else	{

	SCDEFn->Log3Fn(Common_Definition->name
		,Common_Definition->nameLen
		,1
		,"Could not enable WebIf support for '%.*s'. Type '%.*s' detects Type 'WebIf' is NOT loaded!"
		,ESP32_Control_Definition->common.nameLen
		,ESP32_Control_Definition->common.name
		,ESP32_Control_Definition->common.module->ProvidedByModule->typeNameLen
		,ESP32_Control_Definition->common.module->ProvidedByModule->typeName);
	}

// -------------------------------------------------------------------------------------------

  return retMsg;

  }













/**
 *   Data structure for implemented query keys and affected readings
 *   used as input for Key=Value parsing, proc SCDEH_ParseStrToparsedKVInput
 */
typedef struct ESP32_Control_kvParseImplementedKeys_s {

  const uint32_t affectedReadings;

  const char *implementedKey;

  } ESP32_Control_kvParseImplementedKeys_t;



/*
 *   Result structure for http_parser_parse_url().
 *   Callers should index into field_data[] with UF_* values if field_set
 *   has the relevant (1 << UF_*) bit set. As a courtesy to clients (and
 *   because we probably have padding left over), we convert any port to
 *   a uint32_t.
 */
typedef struct ESP32_Control_parsedKVInput_s {

  uint64_t keysFoundBF;		// Bit-Field of found keys (1 << XX_IK_*)

  struct ESP32_Control_keyData_s {

	uint16_t off;		// Offset into buffer in which value-text starts

	uint16_t len;		// Length of value-text in buffer

	uint32_t affectedReadings;

	} ESP32_Control_keyData_t[];		// XX_IK_MAX

  } ESP32_Control_parsedKVInput_t;



/*
 * Find the first occurrence of find in s, where the search is limited to the
 * first slen characters of s.
 */
char * ICACHE_FLASH_ATTR
ESP32_Control_strnstr(s, find, slen)
	const char *s;
	const char *find;
	size_t slen;
{
	char c, sc;
	size_t len;

	if ((c = *find++) != '\0') {
		len = strlen(find);
		do {
			do {
				if ((sc = *s++) == '\0' || slen-- < 1)
					return (NULL);
			} while (sc != c);
			if (len > slen)
				return (NULL);
		} while (strncmp(s, find, len) != 0);
		s--;
	}
	return ((char *)s);
}



/**   // query keys dürfen keine encoded zeichen enthalten!
 *--------------------------------------------------------------------------------------------------
 *FName: ESP32_Control_ParseKVInputArgs Parse Key=Value@ input Arguments
 * Desc: Hepler routine that searches and extracts an SPECIAL string value matching the given key. 
 *       Source data is an key=value pair string, seperated by an "&" from an query string
 *       WARNING: FOR FIRST and LAST CHAR ONLY ALPHA OR NUMMERICALAL IS ALLOWED !	  
 * Para: int num -> number of implemented KEYs for this query-type
 *       const implementedKeys *XX_IK -> list of implemented KEYs for this query-type
 *       const char *QueryStr -> ptr to zero-teriminated KEY=VALUE string from query, seperated by '&'
 * Rets: struct SCDE_XX_parsedKVInput *parsedKVInput -> allocad struct, filled with data
 *       DONT FORGET TO FREE MEMORY !
 *--------------------------------------------------------------------------------------------------
 */
ESP32_Control_parsedKVInput_t* ICACHE_FLASH_ATTR
ESP32_Control_ParseKVInputArgs(int numImplementedKeys
			,const ESP32_Control_kvParseImplementedKeys_t *XX_IK
			,const uint8_t *kvArgs
			,const size_t kvArgsLen)
  {

  // alloc memory for ESP32_Control_parsedKVInput_t -> length is variable !
  int memlen =  8 + (numImplementedKeys * (2+2+4));

  #if SCDEH_DBG >= 5
  printf("|PKVInput-malloc:%d, input:\"%.*s\">"
	,memlen
	,kvArgsLen
	,kvArgs);
  #endif

  ESP32_Control_parsedKVInput_t *parsedKVInput =
	(struct ESP32_Control_parsedKVInput_s *) malloc(memlen);

  // clear keys-found bitfield
  parsedKVInput->keysFoundBF = 0;

  // implemented keys loop
  int i;
  for ( i = 0 ; i < numImplementedKeys ; i++ ) {

	// get ptr to current key
	const char *key = XX_IK[i].implementedKey;

	#if SCDEH_DBG >= 5
	printf("|chk for KEY:%d-\"%s\", "
		,i
		,key);
	#endif

	const uint8_t *p = kvArgs;		// kv-args start ptr
	const uint8_t *ep = kvArgs + kvArgsLen;	// kv-args end ptr
	const uint8_t *e;			// end ptr 
	int keylen = strlen(key);		// lenght of implemented key

//	while ( (p != NULL) && (*p != '\n') && (*p != '\r') && (*p != 0) )

	while ( (p != NULL) && (p < ep) ) {

		# if SCDEH_DBG >= 5
		printf("cmp:\"%.*s\""
			,keylen
			,p);
		# endif

		// matches value? and is "=" after value?
		if ( (strncasecmp((char*) p, key, keylen) == 0 ) && (p[keylen] == '=') ) {

			// move p to start of the VALUE (from this KEY)
			p += keylen + 1;

			// move e to end of the VALUE (from this KEY)
			e = (uint8_t *) ESP32_Control_strnstr((char*) p, "&", ep - p);

			// or is it last Key-Value pair?
			if (e == NULL) e = ep;

 			# if SCDEH_DBG >= 5
			printf(", next cmp:\"%.*s\""
				,(e-p)
				,p);
			# endif

			// store positive result -> do not forget to url-encode !
			parsedKVInput->keysFoundBF |= (uint64_t) 1<<i;
			parsedKVInput->ESP32_Control_keyData_t[i].off = (p-kvArgs); // or better store ptr?
			parsedKVInput->ESP32_Control_keyData_t[i].len = (e-p);
			parsedKVInput->ESP32_Control_keyData_t[i].affectedReadings = 
				XX_IK[i].affectedReadings;

			// KEY found
			# if SCDEH_DBG >= 5
			printf(", found!>");
			# endif

			break;

			}

		// jump after next "&"
		p = (uint8_t*) ESP32_Control_strnstr((char*) p, "&", ep - p);

		if (p != NULL) p += 1;

		}

	// KEY not found
	# if SCDEH_DBG >= 5
	printf(">");
	# endif

	}

  // Query parsed complete
  # if SCDEH_DBG >= 5
  SCDEFn->HexDumpOutFn ("|parsedKVInput-HEX",
	parsedKVInput,
	memlen);
  # endif

  return parsedKVInput;

  }













/**
 * For Set-Input: enum of implemented keys
 * KEYs-Field-Set for Feature specific query. Will be generated by SCDEH_ParseStrToparsedKVInput()
 * parsed result is stored in ESP32_Control_parsedKVInput_t
 */
enum ESP32_Control_Set_IK {				// Bit #XX for debugging

   // Block 1 - func
    ESP32_Control_Set_IK_NAME		= 0		// Bit #00 'NAME'			-> 
  , ESP32_Control_Set_IK_WSAP_Password			// Bit #01 'WSAP_Password'		-> 
  , ESP32_Control_Set_IK_WSAP_RF_Channel		// Bit #02 'WSAP_RF_Channel' 		-> 
  , ESP32_Control_Set_IK_WSAP_Maximal_Connections	// Bit #03 'WSAP_Maximal_Connections'  	-> 
  , ESP32_Control_Set_IK_WSAP_Authentication_Method	// Bit #04 'WSAP_Authentication_Method' -> 
  , ESP32_Control_Set_IK_WSAP_SSID			// Bit #05 'WSAP_SSID'  		-> 
  , ESP32_Control_Set_IK_WSAP_Beacon_Interval		// Bit #06 'WSAP_Beacon_Interval'  	-> 

   // Block 2 - func
  , ESP32_Control_Set_IK_WSAP_IP_Adress			// Bit #07 'WSAP_DHCPS'  		-> 
  , ESP32_Control_Set_IK_WSAP_Netmask			// Bit #08 'WSAP_Netmask'  		-> 
  , ESP32_Control_Set_IK_WSAP_Gateway_Adress		// Bit #09 'WSAP_Gateway_Adress'  	-> 

   // Block 3 - func
  , ESP32_Control_Set_IK_WSAP_MAC_Adress		// Bit #10 'WSAP_MAC_Adress'  		-> 

   // Block 4 - func
  , ESP32_Control_Set_IK_WSAP_WiFi_Bandwidth		// Bit #11 'WSAP_WiFi_Bandwidth'	-> 

   // Block 5 - func
  , ESP32_Control_Set_IK_WiFi_Country			// Bit #12 'WiFi_Country'		-> 

   // Block 6 - func
  , ESP32_Control_Set_IK_Station_IP_Adress		// Bit #13 'Station_IP_Adress'  	-> 
  , ESP32_Control_Set_IK_Station_Netmask		// Bit #14 'Station_Netmask'  		-> 
  , ESP32_Control_Set_IK_Station_Gateway_Adress		// Bit #15 'Station_Gateway_Adress'  	-> 

   // Block 7 - func
  , ESP32_Control_Set_IK_Station_MAC_Adress		// Bit #16 'Station_MAC_Adress'		-> 

   // Block 8 - func
  , ESP32_Control_Set_IK_Station_Auto_Connect		// Bit #17 'Station_Auto_Connect'	-> 

   // Block 9 - func
  , ESP32_Control_Set_IK_Station_WiFi_Bandwidth		// Bit #18 'Station_WiFi_Bandwidth'	-> 












   // Block 3 - func
  , ESP32_Control_Set_IK_WSAP_DHCPS_Lease_Start_IP	// Bit #10 'WSAP_DHCPS_Lease_Start_IP'  -> 
  , ESP32_Control_Set_IK_WSAP_DHCPS_Lease_End_IP	// Bit #11 'WSAP_DHCPS_Lease_End_IP'  	-> 

   // Block 4 - func
  , ESP32_Control_Set_IK_WSAP_DHCPS			// Bit #12 'WSAP_DHCPS'  		-> 


   // Block 6 - func
  , ESP32_Control_Set_IK_WSAP				// Bit #14 'WSAP'  			-> 


   // Block 8 - func
  , ESP32_Control_Set_IK_Station			// Bit #18 'Station'  			-> 

   // Block 9 - func
  , ESP32_Control_Set_IK_Station_DHCP			// Bit #19 'Station_DHCP'		-> 


   // Block 11 - func
  , ESP32_Control_Set_IK_Station_Physical_Mode		// Bit #21 'Station_Physical_Mode'	-> 

   // Block 12 - func
  , ESP32_Control_Set_IK_Station_Get_RSSI		// Bit #22 'Station_Get_RSSI'		-> 




   // Block 14 - func
  , ESP32_Control_Set_IK_TiSt_NTP_Server_Main		// Bit #24 'TiSt_NTP_Server_Main'	-> 

   // Block 15 - func
  , ESP32_Control_Set_IK_TiSt_NTP_Server_Backup_1	// Bit #25 'TiSt_NTP_Server_Backup_1'	-> 

   // Block 16 - func
  , ESP32_Control_Set_IK_TiSt_NTP_Server_Backup_2	// Bit #26 'TiSt_NTP_Server_Backup_2'	-> 

   // Block 17 - func
  , ESP32_Control_Set_IK_TiSt_Time_Zone			// Bit #27 'TiSt_Time_Zone'		-> 

   // Block 18 - func
  , ESP32_Control_Set_IK_TiSt_Get_Time			// Bit #28 'TiSt_Get_Time'		-> 

   // Block xx - func
  , ESP32_Control_Set_IK_SoCHW_Restart			// Bit #29 'SoCHW_Restart'		-> 

   // Block xx - func
  , ESP32_Control_Set_IK_SoCHW_Factory_Reset_SCDE	// Bit #30 'SoCHW_Factory_Reset_SCDE'	-> 

   // Block xx - func
  , ESP32_Control_Set_IK_SoCHW_Factory_Reset_CAPI	// Bit #31 'SoCHW_Factory_Reset_CAPI'	-> 

   // Block xx - func
  , ESP32_Control_Set_IK_SoCHW_Factory_Reset		// Bit #32 'SoCHW_Factory_Reset'	-> 

   // helper to get number of set-field-positions
  , ESP32_Control_Set_IK_Number_of_keys			// Bit #34 MAX 64 IMPLEMENTED !
};



/* For Feature: SCDE
 * Implemented Feature specific readings. Can be assigned to Query Keys if affected
 */
enum SCDE_Readings
  {						 // Bit #XX for debugging
  // Internals - TiSt-Sharing - Block 1 - func
   SCDE_R_NAME				= (1<<0) // Bit #01 'NAME'			-> 
  ,SCDE_R_WSAP_Password			= (1<<0) // Bit #01 'WSAP_Password'		-> 
  ,SCDE_R_WSAP_RF_Channel		= (1<<0) // Bit #01 'WSAP_RF_Channel'		-> 
  ,SCDE_R_WSAP_Maximal_Connections	= (1<<0) // Bit #01 'WSAP_Maximal_Connections'	-> 
  ,SCDE_R_WSAP_Authentication_Method	= (1<<0) // Bit #01 'WSAP_Authentication_Method'-> 
  ,SCDE_R_WSAP_SSID			= (1<<0) // Bit #01 'WSAP_SSID'			->
  ,SCDE_R_WSAP_Beacon_Interval		= (1<<0) // Bit #01 'WSAP_Beacon_Interval'	-> 

  // Internals - TiSt-Sharing - Block 2 - func
  ,SCDE_R_WSAP_IP_Adress		= (1<<1) // Bit #02 'WSAP_IP_Adress'		-> 
  ,SCDE_R_WSAP_Netmask			= (1<<1) // Bit #02 'WSAP_Netmask'		-> 
  ,SCDE_R_WSAP_Gateway_Adress		= (1<<1) // Bit #02 'WSAP_Gateway_Adress'	-> 

  // Internals - TiSt-Sharing - Block 3 - func
  ,SCDE_R_WSAP_MAC_Adress		= (1<<2) // Bit #04 'WSAP_MAC_Adress'		-> 

  // Internals - TiSt-Sharing - Block 4 - func
  ,SCDE_R_WSAP_WiFi_Bandwidth		= (1<<3) // Bit #07 'WSAP_WiFi_Bandwidth'	-> 

  // Internals - TiSt-Sharing - Block 5 - func
  ,SCDE_R_WiFi_Country			= (1<<4) // Bit #07 'WiFi_Country'		-> 

  // Internals - TiSt-Sharing - Block 6 - func
  ,SCDE_R_Station_IP_Adress		= (1<<5) // Bit #06 'Station_IP_Adress'	-> 
  ,SCDE_R_Station_Netmask		= (1<<5) // Bit #06 'Station_Netmask'		-> 
  ,SCDE_R_Station_Gateway_Adress	= (1<<5) // Bit #06 'Station_Gateway_Adress'	-> 

  // Internals - TiSt-Sharing - Block 7 - func
  ,SCDE_R_Station_MAC_Adress		= (1<<6) // Bit #11 'Station_MAC_Adress'	-> 

  // Internals - TiSt-Sharing - Block 8 - func
  ,SCDE_R_Station_Auto_Connect		= (1<<7) // Bit #09 'Station_Auto_Connect'	-> 

  // Internals - TiSt-Sharing - Block 9 - func
  ,SCDE_R_Station_WiFi_Bandwidth	= (1<<8) // Bit #07 'Station_WiFi_Bandwidth'	-> 














  // Internals - TiSt-Sharing - Block 3 - func
  ,SCDE_R_WSAP_DHCPS_Lease_Start_IP	= (1<<2) // Bit #03 'WSAP_DHCPS_Lease_Start_IP'	-> 
  ,SCDE_R_WSAP_DHCPS_Lease_End_IP	= (1<<2) // Bit #03 'WSAP_DHCPS_Lease_End_IP'	-> 
  ,SCDE_R_WSAP_DHCPS			= (1<<2) // Bit #03 'WSAP_DHCPS'		-> 


  // Internals - TiSt-Sharing - Block 5 - func
  ,SCDE_R_WSAP				= (1<<4) // Bit #05 'WSAP'			-> 


  // Internals - TiSt-Sharing - Block 7 - func
  ,SCDE_R_Station			= (1<<6) // Bit #07 'Station'			-> 

  // Internals - TiSt-Sharing - Block 8 - func
  ,SCDE_R_Station_DHCP			= (1<<7) // Bit #08 'Station_DHCP'		-> 


  // Internals - TiSt-Sharing - Block 10 - func
  ,SCDE_R_Station_Physical_Mode		= (1<<9) // Bit #10 'Station_Physical_Mode'	-> 


  // Internals - TiSt-Sharing - Block 12 - func
  ,SCDE_R_TiSt_NTP_Server_Main		= (1<<11) // Bit #12 'TiSt_NTP_Server_Main'	-> 
  ,SCDE_R_TiSt_NTP_Server_Backup_1	= (1<<11) // Bit #12 'TiSt_NTP_Server_Backup_1'	-> 
  ,SCDE_R_TiSt_NTP_Server_Backup_2	= (1<<11) // Bit #12 'TiSt_NTP_Server_Backup_2'	-> 
  ,SCDE_R_TiSt_Time_Zone		= (1<<11) // Bit #12 'TiSt_Time_Zone'		-> 


  // Readings - TiSt-Sharing - Block 1 - func
  ,SCDE_R_Station_RSSI			= (1<<12) // Bit #13 'Station_RSSI'		-> 
  ,SCDE_R_TiSt_Time			= (1<<12) // Bit #13 'TiSt_Time'		-> 

  ,SCDE_R_name				= (1<<13) // Bit #02 'name' + caps + ufid -> 







  };



/* For: SCDE_cgi
 * Implemented KEYS and assigned readings for this query - analyzed by by http_parser_parse_url()
 * Num should meet enum XX_QueryFields
 * Result is stored in struct SCDE_XX_parsedKVInput
 */
ESP32_Control_kvParseImplementedKeys_t ESP32_Control_Set_ImplementedKeys[] =
  {//affected_reading | affected_reading			|  CMD
  // Block 1 - func
   { SCDE_R_NAME						, "name" }			// #00
  ,{ SCDE_R_WSAP_Password					, "WSAP_Password" }		// #01
  ,{ SCDE_R_WSAP_RF_Channel					, "WSAP_RF_Channel" }		// #02
  ,{ SCDE_R_WSAP_Maximal_Connections				, "WSAP_Maximal_Connections" }	// #03
  ,{ SCDE_R_WSAP_Authentication_Method				, "WSAP_Authentication_Method" }// #04
  ,{ SCDE_R_WSAP_SSID						, "WSAP_SSID" }			// #05
  ,{ SCDE_R_WSAP_Beacon_Interval				, "WSAP_Beacon_Interval" }	// #06

  // Block 2 - func
  ,{ SCDE_R_WSAP_IP_Adress					, "WSAP_IP_Adress" }		// #07
  ,{ SCDE_R_WSAP_Netmask					, "WSAP_Netmask" }		// #08
  ,{ SCDE_R_WSAP_Gateway_Adress					, "WSAP_Gateway_Adress" }	// #09

  // Block 3 - func
  ,{ SCDE_R_WSAP_MAC_Adress					, "WSAP_MAC_Adress" }		// #10

  // Block 4 - func
  ,{ SCDE_R_WSAP_WiFi_Bandwidth					, "WSAP_WiFi_Bandwidth" }	// #11

  // Block 5 - func
  ,{ SCDE_R_WiFi_Country					, "WiFi-Country" }		// #12

  // Block 6 - func
  ,{ SCDE_R_Station_IP_Adress					, "Station_IP_Adress" }		// #13
  ,{ SCDE_R_Station_Netmask					, "Station_Netmask" }		// #14
  ,{ SCDE_R_Station_Gateway_Adress				, "Station_Gateway_Adress" }	// #15

  // Block 7 - func
  ,{ SCDE_R_Station_MAC_Adress					, "Station_MAC_Adress" }	// #16

  // Block 8 - func
  ,{ SCDE_R_Station_Auto_Connect				, "Station_Auto_Connect" }	// #17

  // Block 9 - func
  ,{ SCDE_R_Station_WiFi_Bandwidth				, "Station_WiFi_Bandwidth" }	// #18



















  // Block 3 - func
  ,{ SCDE_R_WSAP_DHCPS_Lease_Start_IP				, "WSAP_DHCPS_Lease_Start_IP" }	// #10
  ,{ SCDE_R_WSAP_DHCPS_Lease_End_IP				, "WSAP_DHCPS_Lease_End_IP" }	// #11

  // Block 4 - func
  ,{ SCDE_R_WSAP_DHCPS						, "WSAP_DHCPS" }		// #12

  // Block 6 - func
  ,{ SCDE_R_WSAP						, "WSAP" }			// #14

  // Block 8 - func
  ,{ SCDE_R_Station						, "Station" }			// #18

  // Block 9 - func
  ,{ SCDE_R_Station_DHCP					, "Station_DHCP" }		// #19

  // Block 11 - func
  ,{ SCDE_R_Station_Physical_Mode				, "Station_Physical_Mode" }	// #21

  // Block 12 - func
  ,{ SCDE_R_Station_RSSI					, "Station_Get_RSSI" }		// #22

   // Block 14 - func
  ,{ SCDE_R_TiSt_NTP_Server_Main				, "TiSt_NTP_Server_Main" }	// #24

  // Block 15 - func
  ,{ SCDE_R_TiSt_NTP_Server_Backup_1				, "TiSt_NTP_Server_Backup_1" }	// #25

  // Block 16 - func
  ,{ SCDE_R_TiSt_NTP_Server_Backup_2				, "TiSt_NTP_Server_Backup_2" }	// #26

  // Block 17 - func
  ,{ SCDE_R_TiSt_Time_Zone					, "TiSt_Time_Zone" }		// #27

  // Block 18 - func
  ,{ SCDE_R_TiSt_Time						, "TiSt_Get_Time" }		// #28

  // Block 18 - func
  ,{ 0								, "SoCHW_Restart" }		// #29

  // Block 18 - func
  ,{ 0								, "SoCHW_Factory_Reset_SCDE" }	// #30

  // Block 18 - func
  ,{ 0								, "SoCHW_Factory_Reset_CAPI" }	// #31

 // Block 18 - func
  ,{ 0								, "SoCHW_Factory_Reset" }	// #32

  }; // !!! number of implemented keys should be equal with XX_IK_Number_of_keys, LIMIT IS 64 ELEMENTS !!!




/*"WSAP_Password ";
	$list .= "WSAP_RF_Channel:uzsuSelectRadio,1,2,3,4,5,6,7,8,9,10,11,12,13 ";

"WSAP_Beacon_Interval:slider,100,10,60000 ";

uint8Slider,a,b,c
uint16Slider,a,b,c
uint32Slider,a,b,c
int8Slider,a,b,c
int16Slider,a,b,c
int32Slider,a,b,c


*/









/**
 * -------------------------------------------------------------------------------------------------
 *  FName: ESP32_Control_Rename
 *  Desc: FN is called when a Definition of this Module was renamed. Module can do further things here. 
 *  Info: An FN for Rename is optional!
 *  Para: ESP32_Control_Definition_t *ESP32_Control_Definition -> ESP32_Control Definition that was renamed
 *        uint8_t *newName -> ptr to the new name (do we need it???)
 *        size_t newNameLen -> length of the new name
 *        uint8_t *oldName -> ptr to old name
 *        size_t oldNameLen -> length of the old name
 *  Rets: strTextMultiple_t* -> response text in allocated memory, NULL=no text
 * -------------------------------------------------------------------------------------------------
 */
strTextMultiple_t*
ESP32_Control_Rename(Common_Definition_t *Common_Definition
	,uint8_t *newName
	,size_t newNameLen
	,uint8_t *oldName
	,size_t oldNameLen)
{
	
	// for Fn response msg
  strTextMultiple_t *multipleRetMsg = NULL;
	
  // make common ptr to modul specific ptr
  ESP32_Control_Definition_t* ESP32_Control_Definition =
		(ESP32_Control_Definition_t*) Common_Definition;

	SCDEFn->Log3Fn(Common_Definition->name
		,Common_Definition->nameLen
		,5
		,"RenameFn of Module '%.*s' is called for Definition '%.*s'. Definition got new name. Old name was '%.*s'."
		,ESP32_Control_Definition->common.module->ProvidedByModule->typeNameLen
		,ESP32_Control_Definition->common.module->ProvidedByModule->typeName
		,newName
		,newNameLen
		,oldName
		,oldNameLen);
	
	return multipleRetMsg;
}
	
	

/**
 * -------------------------------------------------------------------------------------------------
 *  FName: ESP32_Control_Set
 *  Desc: Processes the device-specific command line arguments from the set command
 *  Info: Invoked by cmd-line 'Set ESP32_Control_Definition.common.Name setArgs'
 *  Para: ESP32_Control_Definition_t *ESP32_Control_Definition -> WebIF Definition that should get a set cmd
 *        uint8_t *setArgs -> the setArgs
 *        size_t setArgsLen -> length of the setArgs
 *  Rets: strTextMultiple_t* -> response text in allocated memory, NULL=no text
 * -------------------------------------------------------------------------------------------------
 */
strTextMultiple_t* ICACHE_FLASH_ATTR
ESP32_Control_Set(Common_Definition_t* Common_Definition
	,uint8_t *setArgs
	,size_t setArgsLen)
  {

  // for Fn response msg
  strTextMultiple_t *retMsg = NULL;

  // beginning with '?' ?
  if ( (setArgsLen > 0) && (*setArgs == '?') ) {

	// set start of possible Type-Name
	const uint8_t *tempTxt = setArgs + 1;

	// a seek-counter
	int i = 1;

	// skip spaces after '?' (search for more ...)
	while( (i < setArgsLen) && (*tempTxt == ' ') ) {i++;tempTxt++;}

	// only spaces after '?' -> answer with Set-Capabilities
	if (i == setArgsLen) {

		// response with error text
		// alloc mem for retMsg
		retMsg = malloc(sizeof(strTextMultiple_t));

		// response with error text
		retMsg->strTextLen = asprintf(&retMsg->strText
			,"requested ? '%.*s' !"
			,setArgsLen
			,setArgs);

		return retMsg;

		}
	// '? + X' here !!! -> normal parse
	}

  // Parse set-args (key=value@) protocol) and get parsedKVInput in allocated mem
  ESP32_Control_parsedKVInput_t *parsedKVInput = 
	ESP32_Control_ParseKVInputArgs(ESP32_Control_Set_IK_Number_of_keys	// Num Implementated KEYs MAX for Set Fn
	,ESP32_Control_Set_ImplementedKeys	// Implementated Keys for Set Fn
	,setArgs				// our args text
	,setArgsLen);				// our args text len

// -------------------------------------------------------------------------------------------------
// Prepare structures with current values from SDK
// -------------------------------------------------------------------------------------------------

  // block #1 get current Service-AP settings
  wifi_config_t ap_wifi_config;
  esp_wifi_get_config (WIFI_IF_AP, &ap_wifi_config);

  // block #2  get current Service AP IP-Settings
  tcpip_adapter_ip_info_t ap_ip_info;
  tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_AP, &ap_ip_info);

  // block #3 get current Service AP MAC Adress
  uint8_t ap_mac_addr[8];
  esp_wifi_get_mac(WIFI_IF_AP,(uint8_t *) &ap_mac_addr);

  // block #4 get the bandwith of the Service Access Point
  wifi_bandwidth_t ap_wifi_bandwidth;
  esp_wifi_get_bandwidth(WIFI_IF_AP, &ap_wifi_bandwidth);

  // block #5 get the WiFi Country
  wifi_country_t country;
  esp_wifi_get_country(&country);

  // block #6 get current Station IP Settings
  tcpip_adapter_ip_info_t sta_ip_info;
  tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &sta_ip_info);

  // block #7 get current Station MAC Adress
  uint8_t sta_mac_addr[8];
  esp_wifi_get_mac(WIFI_IF_STA, (uint8_t *) &sta_mac_addr);

  // block #8 get current Station Auto-Connect-Status
  bool sta_autoconnect_status;
  esp_wifi_get_auto_connect(&sta_autoconnect_status);

  // block #9 get the bandwith of Station
  wifi_bandwidth_t sta_wifi_bandwidth;
  esp_wifi_get_bandwidth(WIFI_IF_STA, &sta_wifi_bandwidth);











//#3
  // block #6 get current Station Settings
  wifi_config_t sta_wifi_config;
  esp_wifi_get_config (WIFI_IF_STA, &sta_wifi_config);

  // block #3 get current Service AP DHCP-Server-Status
  tcpip_adapter_dhcp_status_t ap_dhcp_status;
  tcpip_adapter_dhcps_get_status(TCPIP_ADAPTER_IF_AP,&ap_dhcp_status); 
//esp_err_t tcpip_adapter_dhcps_stop(tcpip_adapter_if_t tcpip_if);
//esp_err_t tcpip_adapter_dhcps_start(tcpip_adapter_if_t tcpip_if);

  // block #8 get current Station DHCP-Client-Status
  tcpip_adapter_dhcp_status_t sta_dhcp_status;
  tcpip_adapter_dhcpc_get_status(TCPIP_ADAPTER_IF_STA,&sta_dhcp_status); 
//esp_err_t tcpip_adapter_dhcpc_start(tcpip_adapter_if_t tcpip_if);
//esp_err_t tcpip_adapter_dhcpc_stop(tcpip_adapter_if_t tcpip_if);

  // block #13 get current Wifi operating mode (Station + Service AP)
  wifi_mode_t wifi_mode;
  esp_wifi_get_mode(&wifi_mode);

  // block #14 get current Wifi operating mode (Station + Service AP)
  wifi_ps_type_t wifi_ps_type;
  esp_wifi_get_ps(&wifi_ps_type);


/*
  // block #3 get current Service AP DHCP-Settings
//spz    struct dhcps_lease dhcps_lease;
//spz   wifi_softap_get_dhcps_lease(&dhcps_lease);

//spz  // block #11 get current Station Physical Mode
//spz  int WifiStationPhyMode = wifi_get_phy_mode();

*/


  // remember the readings affected by the parsing process
  uint32_t affectedReadings = 0;

// -------------------------------------------------------------------------------------------------

  // name=[a-f-A-F0-9] -> Setze Farbe für RGB Kanal (uint24) (0-x)
  // name=[a-f-A-F0-9] -> Set color for RGB channel (uint24) (0-x)
  if (parsedKVInput->keysFoundBF == (uint64_t) 1 << ESP32_Control_Set_IK_NAME) {

	// valid input happened ?
	if (SCDEH_GetSpecialStrVal(setArgs+parsedKVInput->ESP32_Control_keyData_t[ESP32_Control_Set_IK_NAME].off
		,parsedKVInput->ESP32_Control_keyData_t[ESP32_Control_Set_IK_NAME].len
		,(char*) &ap_wifi_config.ap.ssid
		,sizeof (ap_wifi_config.ap.ssid)
		,2) ) {	

		// mark affected readings for TX
		affectedReadings |= parsedKVInput->ESP32_Control_keyData_t[ESP32_Control_Set_IK_NAME].affectedReadings;

//		// New Timestamp
//		SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[ADID].IB01_X_TiSt =
//			GetUniqueTIST();

//		// Push processed CMD to to Response ...
//		RespArgsWPos += sprintf( RespArgsWPos,"cmd=name");

	}
  }

// -------------------------------------------------------------------------------------------------


  // rval=[0-9] -> Setze Wert für den roten Anteil vom RGB Kanal (uint8) (0-x)
  // rval=[0-9] -> Set value for the red component of the RGB channel (uint8) (0-x)

  if (parsedKVInput->keysFoundBF == (uint64_t) 1 << ESP32_Control_Set_IK_WSAP_Password) {

	// valid input happened ?
	if (SCDEH_GetSpecialStrVal(setArgs+parsedKVInput->ESP32_Control_keyData_t[ESP32_Control_Set_IK_WSAP_Password].off
		,parsedKVInput->ESP32_Control_keyData_t[ESP32_Control_Set_IK_WSAP_Password].len
		,(char*) &ap_wifi_config.ap.password
		,sizeof (ap_wifi_config.ap.password)
		,2) ) {	

		// mark affected readings for TX
		affectedReadings |= parsedKVInput->ESP32_Control_keyData_t[ESP32_Control_Set_IK_WSAP_Password].affectedReadings;

		// New Timestamp
//		SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[ADID].IB01_X_TiSt =
//			GetUniqueTIST();

//		// Push processed CMD to to Response ...
//		RespArgsWPos += sprintf( RespArgsWPos,"cmd=WSAP_Password");

	}
  }

// -------------------------------------------------------------------------------------------------

  // WSAP_RF_Channel=[1-13] -> Setze Wireless Service Access Point Kanal (Station Kanal hat Priorität)
  // WSAP_RF_Channel=[1-13] -> Set Wireless Service Access Point channel (Station Channel has priority)

  if (parsedKVInput->keysFoundBF == (uint64_t) 1 << ESP32_Control_Set_IK_WSAP_RF_Channel) {

	uint8_t NewChan;

	// valid input happened ?
	if (SCDEH_GetDecUInt8Val(setArgs+parsedKVInput->ESP32_Control_keyData_t[ESP32_Control_Set_IK_WSAP_RF_Channel].off
		,parsedKVInput->ESP32_Control_keyData_t[ESP32_Control_Set_IK_WSAP_RF_Channel].len, &NewChan)) {

		if ( (NewChan >= 1) && (NewChan <= 13) ) {

			// save new channel
			ap_wifi_config.ap.channel = NewChan;

			// mark affected readings for TX
			affectedReadings |= parsedKVInput->ESP32_Control_keyData_t[ESP32_Control_Set_IK_WSAP_RF_Channel].affectedReadings;

//			// New Timestamp
//			SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[ADID].IB01_X_TiSt =
//				GetUniqueTIST();

			// Push processed CMD to to Response ...
//			RespArgsWPos += sprintf( RespArgsWPos,"cmd=WSAP_RF_Channel");

		}
	}

  }

// -------------------------------------------------------------------------------------------------

  // rmax=[ ] -> Setze vom RGB Kanal den roten Anteil maximal (0-x)
  // rmax=[ ] -> Set red component of the RGB channel to maximum (0-x)

  if (parsedKVInput->keysFoundBF == (uint64_t) 1 << ESP32_Control_Set_IK_WSAP_Maximal_Connections)

	{

	uint8_t NewMaxConn;

	// valid input happened ?
	if (SCDEH_GetDecUInt8Val(setArgs+parsedKVInput->ESP32_Control_keyData_t[ESP32_Control_Set_IK_WSAP_Maximal_Connections].off
		,parsedKVInput->ESP32_Control_keyData_t[ESP32_Control_Set_IK_WSAP_Maximal_Connections].len, &NewMaxConn)) {

		if ( (NewMaxConn >= 0) && (NewMaxConn <= 4) ) {

			// save new max conn
			ap_wifi_config.ap.max_connection = MaxConn;

			// mark affected readings for TX
			affectedReadings |= parsedKVInput->ESP32_Control_keyData_t[ESP32_Control_Set_IK_WSAP_Maximal_Connections].affectedReadings;

			// New Timestamp
//			SysCfgRamNoMirror->MyLiGHT_FeatCfgRamNoMirror_M1[ADID].RB01_X_TiSt =
//				GetUniqueTIST();

			// Push processed CMD to to Response ...
//			RespArgsWPos += os_sprintf( RespArgsWPos,"cmd=WSAP_RF_Channel");
 
		}
	}
  }

// --------------------------------------------------------------------------------------------------

  // rmin=[ ] -> Setze vom RGB Kanal den roten Anteil minimal (0-x)
  // rmin=[ ] -> Set red component of the RGB channel to minimum (0-x)

  if (parsedKVInput->keysFoundBF == (uint64_t) 1 << ESP32_Control_Set_IK_WSAP_Authentication_Method) {

	uint8_t NewAuthMode;

	// valid input happened ?
	if (SCDEH_GetQueryKeyID(setArgs+parsedKVInput->ESP32_Control_keyData_t[ESP32_Control_Set_IK_WSAP_Authentication_Method].off
		,parsedKVInput->ESP32_Control_keyData_t[ESP32_Control_Set_IK_WSAP_Authentication_Method].len, &NewAuthMode, Auth_M)) {

		// save auth mode
		ap_wifi_config.ap.authmode = NewAuthMode;

		// mark affected readings for TX
		affectedReadings |= parsedKVInput->ESP32_Control_keyData_t[ESP32_Control_Set_IK_WSAP_Authentication_Method].affectedReadings;

		// New Timestamp
//		SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[ADID].IB01_X_TiSt =
//			GetUniqueTIST();

		// Push processed CMD to to Response ...
//		RespArgsWPos += sprintf( RespArgsWPos,"cmd=WSAP_Authentication_Method");

	}
  }

// --------------------------------------------------------------------------------------------------

  // gval=[0-9] -> Setze Wert für den gruenen Anteil vom RGB Kanal (uint8) (0-x)
  // gval=[0-9] -> Set value for the gree component of the RGB channel (uint8) (0-x)

  if (parsedKVInput->keysFoundBF == (uint64_t) 1 << ESP32_Control_Set_IK_WSAP_SSID) {

	uint8_t NewSSIDSetting;

	// valid input happened ?
	if (SCDEH_GetQueryKeyID(setArgs+parsedKVInput->ESP32_Control_keyData_t[ESP32_Control_Set_IK_WSAP_SSID].off
		,parsedKVInput->ESP32_Control_keyData_t[ESP32_Control_Set_IK_WSAP_SSID].len, &NewSSIDSetting, SSID_H)) {

		// save auth mode
		ap_wifi_config.ap.ssid_hidden = NewSSIDSetting;

		// mark affected readings for TX
		affectedReadings |= parsedKVInput->ESP32_Control_keyData_t[ESP32_Control_Set_IK_WSAP_SSID].affectedReadings;

		// New Timestamp
//		SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[ADID].IB01_X_TiSt =
//			GetUniqueTIST();

		// Push processed CMD to to Response ...
//		RespArgsWPos += sprintf( RespArgsWPos,"cmd=WSAP_SSID");

	}
  }

// --------------------------------------------------------------------------------------------------

  // WSAP-Beacon-Interval=[100-60000] -> Setze den Beacon-Sendeinterval des Wireless Service Access Points
  // WSAP-Beacon-Interval=[100-60000] -> Set the beacon interval of the Wireless Service Access Point

  if (parsedKVInput->keysFoundBF == (uint64_t) 1 << ESP32_Control_Set_IK_WSAP_Beacon_Interval) {

	uint16_t NewBI;

	// valid input happened ?
	if (SCDEH_GetDecUInt16Val(setArgs+parsedKVInput->ESP32_Control_keyData_t[ESP32_Control_Set_IK_WSAP_Beacon_Interval].off
		,parsedKVInput->ESP32_Control_keyData_t[ESP32_Control_Set_IK_WSAP_Beacon_Interval].len, &NewBI)) {

		if ( (NewBI >= 100) && (NewBI <= 60000) ) {

			// save beacon interval
			ap_wifi_config.ap.beacon_interval = (uint16_t) NewBI;

			// mark affected readings for TX
			affectedReadings |= parsedKVInput->ESP32_Control_keyData_t[ESP32_Control_Set_IK_WSAP_Beacon_Interval].affectedReadings;

			// New Timestamp
//			SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[ADID].IB01_X_TiSt =
//				GetUniqueTIST();

			// Push processed CMD to to Response ...
//			RespArgsWPos += sprintf( RespArgsWPos,"cmd=WSAP-Beacon-Interval");

		}
	}

  }

// --------------------------------------------------------------------------------------------------

  // gmax=[ ] -> Setze vom RGB Kanal den gruenen Anteil maximal (0-x)
  // gmax=[ ] -> Set green component of the RGB channel to maximum (0-x)

  else if (parsedKVInput->keysFoundBF == (uint64_t) 1 << ESP32_Control_Set_IK_WSAP_IP_Adress) {

  }

// --------------------------------------------------------------------------------------------------

  // gmin=[ ] -> Setze vom RGB Kanal den gruenen Anteil minimal (0-x)
  // gmin=[ ] -> Set green component of the RGB channel to minimum (0-x)

  else if (parsedKVInput->keysFoundBF == (uint64_t) 1 << ESP32_Control_Set_IK_WSAP_Netmask) {

  }

// --------------------------------------------------------------------------------------------------

  // bval=[0-9] -> Setze Wert für den blauen Anteil vom RGB Kanal (uint8) (0-x)
  // bval=[0-9] -> Set value for the blue component of the RGB channel (uint8) (0-x)

  else if (parsedKVInput->keysFoundBF == (uint64_t) 1 << ESP32_Control_Set_IK_WSAP_Gateway_Adress) {

  }

// --------------------------------------------------------------------------------------------------


  // red=[ ] -> Setze RGB Kanal auf Leuchtfarbe rot (0-x)
  // red=[ ] -> Set RGB Channel SCDEing red (0-x)

  else if (parsedKVInput->keysFoundBF == (uint64_t) 1 << ESP32_Control_Set_IK_WSAP_MAC_Adress) {

  }

// --------------------------------------------------------------------------------------------------

  // yell=[ ] -> Setze RGB Kanal auf Leuchtfarbe gelb (0-x)
  // yell=[ ] -> Set RGB Channel SCDEing yellow (0-x)

  else if (parsedKVInput->keysFoundBF == (uint64_t) 1 << ESP32_Control_Set_IK_WiFi_Country) {

	}

// --------------------------------------------------------------------------------------------------

  // yell=[ ] -> Setze RGB Kanal auf Leuchtfarbe gelb (0-x)
  // yell=[ ] -> Set RGB Channel SCDEing yellow (0-x)

  else if (parsedKVInput->keysFoundBF == (uint64_t) 1 << ESP32_Control_Set_IK_WSAP_WiFi_Bandwidth) {

	}

// --------------------------------------------------------------------------------------------------

  // blue=[ ] -> Setze RGB Kanal auf Leuchtfarbe blau (0-x)
  // blue=[ ] -> Set RGB Channel SCDEing blue (0-x)

  else if (parsedKVInput->keysFoundBF == (uint64_t) 1 << ESP32_Control_Set_IK_Station_IP_Adress) {

	}

// --------------------------------------------------------------------------------------------------

  // oran=[ ] -> Setze RGB Kanal auf Leuchtfarbe orange (0-x)
  // oran=[ ] -> Set RGB Channel SCDEing orange (0-x)

  else if (parsedKVInput->keysFoundBF == (uint64_t) 1 << ESP32_Control_Set_IK_Station_Netmask) {

	}

// --------------------------------------------------------------------------------------------------

  // yell=[ ] -> Setze RGB Kanal auf Leuchtfarbe gelb (0-x)
  // yell=[ ] -> Set RGB Channel SCDEing yellow (0-x)

  else if (parsedKVInput->keysFoundBF == (uint64_t) 1 << ESP32_Control_Set_IK_Station_Gateway_Adress) {

	}

// --------------------------------------------------------------------------------------------------

  // yell=[ ] -> Setze RGB Kanal auf Leuchtfarbe gelb (0-x)
  // yell=[ ] -> Set RGB Channel SCDEing yellow (0-x)

  else if (parsedKVInput->keysFoundBF == (uint64_t) 1 << ESP32_Control_Set_IK_Station_MAC_Adress) {

	}

// --------------------------------------------------------------------------------------------------

  // wval=[0-9] -> Setze Wert für warm-weissen Kanal (uint8) (0-x)
  // wval=[0-9] -> Set value for warm white channel (uint8) (0-x)

  else if (parsedKVInput->keysFoundBF == (uint64_t) 1 << ESP32_Control_Set_IK_Station_Auto_Connect) {

	}

// --------------------------------------------------------------------------------------------------

  // yell=[ ] -> Setze RGB Kanal auf Leuchtfarbe gelb (0-x)
  // yell=[ ] -> Set RGB Channel SCDEing yellow (0-x)

  else if (parsedKVInput->keysFoundBF == (uint64_t) 1 << ESP32_Control_Set_IK_Station_WiFi_Bandwidth) {

	}

// --------------------------------------------------------------------------------------------------




















  // bmax=[ ] -> Setze vom RGB Kanal den blauen Anteil maximal (0-x)
  // bmax=[ ] -> Set blue component of the RGB channel to maximum (0-x)

  else if (parsedKVInput->keysFoundBF == (uint64_t) 1 << ESP32_Control_Set_IK_WSAP_DHCPS_Lease_Start_IP) {

	}

//--------------------------------------------------------------------------------------------------

  // bmin=[ ] -> Setze vom RGB Kanal den blauen Anteil minimal (0-x)
  // bmin=[ ] -> Set blue component of the RGB channel to minimum (0-x)

  else if (parsedKVInput->keysFoundBF == (uint64_t) 1 << ESP32_Control_Set_IK_WSAP_DHCPS_Lease_End_IP) {

	}

//--------------------------------------------------------------------------------------------------

  // bdim=[0-9] -> Setze dimmer für den blauen Anteil vom RGB Kanal (uint8) (0-x)
  // bdim=[0-9] -> Set dimming for the blue component of the RGB channel (uint8) (0-x)
/*
  else if (parsedKVInput->keysFoundBF == (uint64_t) 1 << ESP32_Control_Set_IK_WSAP_DHCPS) {

	uint8_t NewDHCPSStatus;

	// valid input happened ?
	if (SCDEH_GetQueryKeyID(setArgs+parsedKVInput->ESP32_Control_keyData_t[ESP32_Control_Set_IK_WSAP_DHCPS].off
		,parsedKVInput->ESP32_Control_keyData_t[ESP32_Control_Set_IK_WSAP_DHCPS].len, &NewDHCPSStatus, DisEn)) {

		// save DHCPS Status
		WifiSoftAPDHCPSStatus = NewDHCPSStatus;

		// mark affected readings for TX
		affectedReadings |= parsedKVInput->ESP32_Control_keyData_t[ESP32_Control_Set_IK_WSAP_DHCPS].affectedReadings;

		// New Timestamp
//		SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[ADID].IB03_X_TiSt =
//			GetUniqueTIST();

		// Push processed CMD to to Response ...
//		RespArgsWPos += os_sprintf( RespArgsWPos,"cmd=WSAP_DHCPS");

		}
	}
*/
//--------------------------------------------------------------------------------------------------

  // gree=[ ] -> Setze RGB Kanal auf Leuchtfarbe grün (0-x)
  // gree=[ ] -> Set RGB Channel SCDEing green (0-x)
/*
  else if (parsedKVInput->keysFoundBF == (uint64_t) 1 << ESP32_Control_Set_IK_WSAP) {

	uint8_t NewApOpMode;

	// valid input happened ?
	if (SCDEH_GetQueryKeyID(setArgs+parsedKVInput->ESP32_Control_keyData_t[ESP32_Control_Set_IK_WSAP].off
		,parsedKVInput->ESP32_Control_keyData_t[ESP32_Control_Set_IK_WSAP].len, &NewApOpMode, DisEn)) {

		// save ap op mode
		WifiApOpMode = NewApOpMode;

		// mark affected readings for TX
		affectedReadings |= parsedKVInput->ESP32_Control_keyData_t[ESP32_Control_Set_IK_WSAP].affectedReadings;

		// New Timestamp
//		SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[ADID].IB05_X_TiSt =
//			GetUniqueTIST();

		// Push processed CMD to to Response ...
//		RespArgsWPos += os_sprintf( RespArgsWPos,"cmd=WSAP");

//		RespArgsWPos = SCDEH_AddCmdToWritePos(RespArgsWPos
//			,SCDE_QueryKeys
//			,ESP32_Control_Set_IK_WSAP);

		}
	}
*/
//--------------------------------------------------------------------------------------------------

  // blck=[ ] -> Setze RGB Kanal auf Leuchtfarbe schwarz (0-x)
  // blck=[ ] -> Set RGB Channel SCDEing black (0-x)

  else if (parsedKVInput->keysFoundBF == (uint64_t) 1 << ESP32_Control_Set_IK_Station) {

	}

//--------------------------------------------------------------------------------------------------

  // whte=[ ] -> Setze RGB Kanal auf Leuchtfarbe weiss (0-x)
  // whte=[ ] -> Set RGB Channel SCDEing white (0-x)

  else if (parsedKVInput->keysFoundBF == (uint64_t) 1 << ESP32_Control_Set_IK_Station_DHCP) {

	}

//--------------------------------------------------------------------------------------------------

  // wdim=[0-9] -> Setze dimmer für warm-weissen Kanal (uint8) (0-x)
  // wdim=[0-9] -> Set dimming for warm white channel (uint8) (0-x)
/*
  else if (parsedKVInput->keysFoundBF == (uint64_t) 1 << ESP32_Control_Set_IK_Station_Physical_Mode) {

	uint8_t NewPhyMode;

	// valid input happened ?
	if (SCDEH_GetQueryKeyID(setArgs+parsedKVInput->ESP32_Control_keyData_t[ESP32_Control_Set_IK_Station_Physical_Mode].off
		,parsedKVInput->ESP32_Control_keyData_t[ESP32_Control_Set_IK_Station_Physical_Mode].len, &NewPhyMode, PhMode)) {

		// save Phy Mode
		WifiStationPhyMode = NewPhyMode;

		// mark affected readings for TX
		affectedReadings |= parsedKVInput->ESP32_Control_keyData_t[ESP32_Control_Set_IK_Station_Physical_Mode].affectedReadings;

		// New Timestamp
//		SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[ADID].IB10_X_TiSt =
//			GetUniqueTIST();

		// Push processed CMD to to Response ...
//		RespArgsWPos += os_sprintf( RespArgsWPos,"cmd=Station_Physical_Mode");

		}
	}
*/
//--------------------------------------------------------------------------------------------------

  // wmax=[ ] -> Setze warm-weissen Kanal maximal (0-x)
  // wmax=[ ] -> Set warm white channel maximum (0-x)

  else if (parsedKVInput->keysFoundBF == (uint64_t) 1 << ESP32_Control_Set_IK_Station_Get_RSSI) {

	}

//--------------------------------------------------------------------------------------------------

  // TiSt_NTP_Server_Main=[azAZ09._] -> Setze NTP-Server Adresse (0-x)
  // TiSt_NTP_Server_Main=[azAZ09._] -> Set NTP-Server adress (0-x)
/*
  else if (parsedKVInput->keysFoundBF == (uint64_t) 1 << ESP32_Control_Set_IK_TiSt_NTP_Server_Main) {

	// valid input happened ?
	if (SCDEH_GetSpecialStrVal(setArgs+parsedKVInput->ESP32_Control_keyData_t[ESP32_Control_Set_IK_TiSt_NTP_Server_Main].off
		,parsedKVInput->ESP32_Control_keyData_t[ESP32_Control_Set_IK_TiSt_NTP_Server_Main].len
		,(char*) &SysCfgRamFlashMirrored->NTDomain1
		,sizeof (SysCfgRamFlashMirrored->NTDomain1)
		,2) ) {	

		// save mirror
		SCDE_SaveSysCfgRamFlashMirroredToMirror();

		// stop sntp for changes
		sntp_stop();

		// set new  sntp server name
		sntp_setservername(0, (char*) &SysCfgRamFlashMirrored->NTDomain1);

		// reinit sntp
		sntp_init();

		// mark affected readings for TX
		affectedReadings |= parsedKVInput->ESP32_Control_keyData_t[ESP32_Control_Set_IK_TiSt_NTP_Server_Main].affectedReadings;

		// New Timestamp
		SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[ADID].IB12_X_TiSt =
			GetUniqueTIST();

		// Push processed CMD to to Response ...
		RespArgsWPos += os_sprintf( RespArgsWPos,"cmd=TiSt_NTP_Server_Main");

		}
	}
*/
//--------------------------------------------------------------------------------------------------

  // TiSt_NTP_Server_Backup_1=[azAZ09._] -> Setze NTP-Server Adresse, Backup 1 (0-x)
  // TiSt_NTP_Server_Backup_1=[azAZ09._] -> Set NTP-Server adress, Backup 1 (0-x)
/*
  else if (parsedKVInput->keysFoundBF == (uint64_t) 1 << ESP32_Control_Set_IK_TiSt_NTP_Server_Backup_1) {

	// valid input happened ?
	if (SCDEH_GetSpecialStrVal(setArgs+parsedKVInput->ESP32_Control_keyData_t[ESP32_Control_Set_IK_TiSt_NTP_Server_Backup_1].off
		,parsedKVInput->ESP32_Control_keyData_t[ESP32_Control_Set_IK_TiSt_NTP_Server_Backup_1].len
		,(char*) &SysCfgRamFlashMirrored->NTDomain2
		,sizeof (SysCfgRamFlashMirrored->NTDomain2)
		,2) ) {	

		// save mirror
		SCDE_SaveSysCfgRamFlashMirroredToMirror();

		// stop sntp for changes
		sntp_stop();

		// set new  sntp server name
		sntp_setservername(1, (char*) &SysCfgRamFlashMirrored->NTDomain2);

		// reinit sntp
		sntp_init();

		// mark affected readings for TX
		affectedReadings |= parsedKVInput->ESP32_Control_keyData_t[ESP32_Control_Set_IK_TiSt_NTP_Server_Backup_1].affectedReadings;

		// New Timestamp
		SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[ADID].IB12_X_TiSt =
			GetUniqueTIST();

		// Push processed CMD to to Response ...
		RespArgsWPos += os_sprintf( RespArgsWPos,"cmd=TiSt_NTP_Server_Backup_1");

		}
	}
*/
//--------------------------------------------------------------------------------------------------

  // TiSt_NTP_Server_Backup_2=[azAZ09._] char[?] -> Setze NTP-Server Adresse, Backup 2 (0-x)
  // TiSt_NTP_Server_Backup_2=[azAZ09._] char[?] -> Set NTP-Server adress, Backup 2 (0-x)
/*
  else if (parsedKVInput->keysFoundBF == (uint64_t) 1 << ESP32_Control_Set_IK_TiSt_NTP_Server_Backup_2) {

	// valid input happened ?
	if (SCDEH_GetSpecialStrVal(setArgs+parsedKVInput->ESP32_Control_keyData_t[ESP32_Control_Set_IK_TiSt_NTP_Server_Backup_2].off
		,parsedKVInput->ESP32_Control_keyData_t[ESP32_Control_Set_IK_TiSt_NTP_Server_Backup_2].len
		,(char*) &SysCfgRamFlashMirrored->NTDomain3
		,sizeof (SysCfgRamFlashMirrored->NTDomain3)
		,2) ) {	

		// save mirror
		SCDE_SaveSysCfgRamFlashMirroredToMirror();

		// stop sntp for changes
		sntp_stop();

		// set new  sntp server name
		sntp_setservername(2, (char*) &SysCfgRamFlashMirrored->NTDomain3);

		// reinit sntp
		sntp_init();

		// mark affected readings for TX
		affectedReadings |= parsedKVInput->ESP32_Control_keyData_t[ESP32_Control_Set_IK_TiSt_NTP_Server_Backup_2].affectedReadings;

		// New Timestamp
		SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[ADID].IB12_X_TiSt =
			GetUniqueTIST();

		// Push processed CMD to to Response ...
		RespArgsWPos += os_sprintf( RespArgsWPos,"cmd=TiSt_NTP_Server_Backup_2");

		}
	}
*/
//--------------------------------------------------------------------------------------------------

  // wmax=[ ] -> Setze warm-weissen Kanal maximal (0-x) 
  // wmax=[ ] -> Set warm white channel maximum (0-x)
/*
  else if (parsedKVInput->keysFoundBF == (uint64_t) 1 << ESP32_Control_Set_IK_TiSt_Time_Zone) {

	int8 NewTZ;

	// valid input happened ?
	if (SCDEH_GetDecInt8Val((uint8*)Args+parsedKVInput->ESP32_Control_keyData_t[ESP32_Control_Set_IK_TiSt_Time_Zone].off
		,parsedKVInput->ESP32_Control_keyData_t[ESP32_Control_Set_IK_TiSt_Time_Zone].len, &NewTZ)) {

		if ( (NewTZ >= -11) && (NewTZ <= 13) ) {

			// save new time-zone
			SysCfgRamFlashMirrored->NTTimezone = NewTZ;

			// save mirror
			SCDE_SaveSysCfgRamFlashMirroredToMirror();

			// stop sntp for changes
			sntp_stop();

			// set new sntp TZ
			sntp_set_timezone(NewTZ);

			// reinit sntp
			sntp_init();

			// mark affected readings for TX
			affectedReadings |= parsedKVInput->ESP32_Control_keyData_t[ESP32_Control_Set_IK_TiSt_Time_Zone].affectedReadings;

			// New Timestamp
			SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[ADID].IB12_X_TiSt =
				GetUniqueTIST();

			// Push processed CMD to to Response ...
			RespArgsWPos += os_sprintf( RespArgsWPos,"cmd=TiSt_Time_Zone");

			}
		}

	}
*/
/*
//--------------------------------------------------------------------------------------------------

  // wmax=[ ] -> Setze warm-weissen Kanal maximal (0-x)
  // wmax=[ ] -> Set warm white channel maximum (0-x)

  else if (parsedKVInput->keysFoundBF == (uint64_t) 1 << ESP32_Control_Set_IK_TiSt_Get_Time) {

	}


//--------------------------------------------------------------------------------------------------

  // wmax=[ ] -> Setzt das Geräte warm-weissen Kanal maximal (0-x)
  // wmax=[ ] -> Set the Device into state 'restart'

  else if (parsedKVInput->keysFoundBF == (uint64_t) 1 << ESP32_Control_Set_IK_SoCHW_Restart) {

	system_restart();

	}
*/
//--------------------------------------------------------------------------------------------------

  // SoCHW_Factory_Reset_SCDE=[ ] -> Setzt die Smart-Connected-Device-Engine auf Werkseinstellung zurück 'factory reset'
  // SoCHW_Factory_Reset_SCDE=[ ] -> Set Smart-Connected-Device-Engine to state 'factory reset'
/*
  else if (parsedKVInput->keysFoundBF == (uint64_t) 1 << ESP32_Control_Set_IK_SoCHW_Factory_Reset_SCDE) {

//	SCDE_ReconstructSysCfgRomNoMirror();		// todo
//	SCDE_ReconstructSysCfgRamNoMirror();		// reconstruct Sys-Cfg stored in ram (done by reboot)
	SCDE_ReconstructSysCfgRamFlashMirrored();	// reconstruct Sys-Cfg stored in ram (flash-mirrored) 
	SCDE_ReconstructSysCfgRamRtcMirrored();		// reconstruct Sys-Cfg stored in rtc

	// initial setup / correction of the SDK parameters
	SCDE_SDKInitialParameterSetup();

	// fresh start
	system_restart();

	}
*/
//--------------------------------------------------------------------------------------------------

  // SoCHW_Factory_Reset_CAPI=[ ] -> Setzt die SoC-Hardware-Chip-API auf Werkseinstellung zurück 'factory reset'
  // SoCHW_Factory_Reset_CAPI=[ ] -> Set SoC-Hardware-Chip-API to state 'factory reset'

  else if (parsedKVInput->keysFoundBF == (uint64_t) 1 << ESP32_Control_Set_IK_SoCHW_Factory_Reset_CAPI) {

	// restore SDK parameters (SDK-factory reset)
	system_restore();

//	SCDE_ReconstructSysCfgRomNoMirror();		// todo
//	SCDE_ReconstructSysCfgRamNoMirror();		// reconstruct Sys-Cfg stored in ram (done by reboot)
//	SCDE_ReconstructSysCfgRamFlashMirrored();	// reconstruct Sys-Cfg stored in ram (flash-mirrored) 
//	SCDE_ReconstructSysCfgRamRtcMirrored();		// reconstruct Sys-Cfg stored in rtc

	// initial setup / correction of the SDK parameters
//	SCDE_SDKInitialParameterSetup();

	// fresh start
	system_restart();

	}

//--------------------------------------------------------------------------------------------------

  // SoCHW_Factory_Reset_CAPI=[ ] -> Setzt die SoC-Hardware-Chip-API auf Werkseinstellung zurück 'factory reset'
  // SoCHW_Factory_Reset_CAPI=[ ] -> Set SoC-Hardware-Chip-API to state 'factory reset'

  else if (parsedKVInput->keysFoundBF == (uint64_t) 1 << ESP32_Control_Set_IK_SoCHW_Factory_Reset) {

	// restore SDK parameters (SDK-factory reset)
	system_restore();

	// initial setup / correction of the SDK parameters
//	SCDE_SDKInitialParameterSetup();

	// fresh start
	system_restart();

	}

//>Caps Bit n.a.------------------------------------------------------------------------------------

  // name=[a-zA-Z0-9_.] -> Setze einen neuen Feature Namen (char[31]) (0-x)
  // name=[a-zA-Z0-9_.] -> Set a new Feature Name (char[31]) (0-x)
/*
  else if (parsedKVInput->keysFoundBF == (uint64_t) 1 << ESP32_Control_Set_IK_name) {

	}
*/
//>Caps Bit n.a.------------------------------------------------------------------------------------

  // caps=[ ] -> Fragt nach den Fähigkeiten-Bits (0-x)
  // caps=[ ] -> Request for Capabilities-Bits (0-x)
/*
  else if (parsedKVInput->keysFoundBF == (uint64_t) 1 << ESP32_Control_Set_IK_caps) {

	// mark affected readings for TX
	affectedReadings |= parsedKVInput->ESP32_Control_keyData_t[ESP32_Control_Set_IK_caps].affectedReadings;

	// New Timestamp
//	WriteInt32toSysCfgRamRtcMirrored(&SysCfgRamRtcMirrored->MySCDE_FeatCfgRamRtcMirrored_M1[ADID].IB01_X_TiSt,
//		GetUniqueTIST());
//bug in anderem code

	// Push processed CMD to to Response ...
//	RespArgsWPos += os_sprintf( RespArgsWPos,"cmd=caps");

	}
*/
//--------------------------------------------------------------------------------------------------



// -------------------------------------------------------------------------------------------------
// Save changes to SDK
// -------------------------------------------------------------------------------------------------

  // Processed block #01 readings, that affects ap_wifi_config (WSAP-Wifi-Configuration) ? then save & set
  if (affectedReadings & ( SCDE_R_NAME
			| SCDE_R_WSAP_Password
			| SCDE_R_WSAP_RF_Channel
			| SCDE_R_WSAP_Maximal_Connections
			| SCDE_R_WSAP_Authentication_Method
			| SCDE_R_WSAP_SSID
			| SCDE_R_WSAP_Beacon_Interval) ) {

	# if SCDE_WIFI_DBG >= 5
	HexDumpOut ("WSAP ap_wifi_config ch!",
		&ap_wifi_config,
		sizeof(ap_wifi_config));
	# endif

	// store length of ssid (may be changed)
	ap_wifi_config.ap.ssid_len = 
		strlen((char*)&ap_wifi_config.ap.ssid);

	// Set new WSAP Cfg
	esp_wifi_set_config (WIFI_IF_AP, &ap_wifi_config);

	// Set new Hostname
	tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_AP, (const char *) &ap_wifi_config.ap.ssid);

  }

// -------------------------------------------------------------------------------------------------

   // Processed block #02 readings, that affects AP_ip_info (WSAP-Ip-Info) ? then save & set
  if (affectedReadings & ( SCDE_R_WSAP_IP_Adress
			| SCDE_R_WSAP_Netmask
			| SCDE_R_WSAP_Gateway_Adress) ) {

	# if SCDE_WIFI_DBG >= 5
	HexDumpOut ("WSAP ip_info ch!",
		&ap_ip_info,
		sizeof(ap_ip_info));
	# endif

	// Set new AP IP INFO
	tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_AP, &ap_ip_info);

  }

// -------------------------------------------------------------------------------------------------

  // Processed block #03 readings, that affects ap_mac_addr (WSAP-MAC-Adress) ? then save & set
  if (affectedReadings & ( SCDE_R_WSAP_MAC_Adress ) ) {

	# if SCDE_WIFI_DBG >= 5
	HexDumpOut ("WSAP mac_addr ch!",
		&ap_mac_addr,
		sizeof(ap_mac_addr));
	# endif

	// Set new AP macaddr
	esp_wifi_set_mac(WIFI_IF_AP,(uint8_t *) &ap_mac_addr);

  }

// -------------------------------------------------------------------------------------------------

  // Processed block #04 readings, that affects ap_wifi_bandwidth (WSAP-WiFi-Bandwidth) ? then save & set
  if (affectedReadings & ( SCDE_R_WSAP_WiFi_Bandwidth ) ) {

	// Set new AP wifi_bandwidth
	esp_wifi_set_bandwidth(WIFI_IF_AP, ap_wifi_bandwidth);

  }

// -------------------------------------------------------------------------------------------------

  // Processed block #05 readings, that affects country WiFi-Country) ? then save & set
  if (affectedReadings & ( SCDE_R_WiFi_Country ) ) {

	// Set new WiFi country
	esp_wifi_set_country(&country);

  }

// -------------------------------------------------------------------------------------------------

  // Processed block #06 readings, that affects sta_ip_info (Station-Ip-Info) ? then save & set
  if (affectedReadings & ( SCDE_R_Station_IP_Adress
			|SCDE_R_Station_Netmask
			|SCDE_R_Station_Gateway_Adress ) ) {

	# if SCDE_WIFI_DBG >= 5
	HexDumpOut ("STA ip_info ch!",
		&sta_ip_info,
		sizeof(sta_ip_info));
	# endif

	// Set new STA ip_info
	tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_STA, &sta_ip_info);

  }

// -------------------------------------------------------------------------------------------------

  // Processed block #07 readings, that affects sta_mac_addr (Station-MAC-Adress) ? then save & set
  if (affectedReadings & ( SCDE_R_Station_MAC_Adress ) ) {

	# if SCDE_WIFI_DBG >= 5
	HexDumpOut ("STA mac_addr ch!",
		&sta_mac_addr,
		sizeof(sta_mac_addr));
	# endif

	// Set new AP macaddr
	esp_wifi_set_mac(WIFI_IF_STA,(uint8_t *) &sta_mac_addr);

  }

// -------------------------------------------------------------------------------------------------

  // Processed block #08 readings, that affects sta_autoconnect_status (Station_Auto_Connect) ? then save & set
  if (affectedReadings & ( SCDE_R_Station_Auto_Connect ) ) {

	// Set new auto-connect-status
	esp_wifi_set_auto_connect(sta_autoconnect_status);

  }

// -------------------------------------------------------------------------------------------------

  // Processed block #09 readings, that affects sta_wifi_bandwidth (Station-WiFi-Bandwidth) ? then save & set
  if (affectedReadings & ( SCDE_R_Station_WiFi_Bandwidth ) ) {

	// Set new station bandwidth
	esp_wifi_set_bandwidth(WIFI_IF_STA, sta_wifi_bandwidth);

  }

// -------------------------------------------------------------------------------------------------













/*
  // Processed block #03 readings, that affects AP_dhcps_lease (Service-Access-Point-Configuration) ? then save & set
  if (affectedReadings & ( SCDE_R_WSAP_DHCPS_Lease_Start_IP
			| SCDE_R_WSAP_DHCPS_Lease_End_IP
			| SCDE_R_WSAP_DHCPS) ) {

	# if SCDE_WIFI_DBG >= 5
	HexDumpOut ("WSAP dhcps_lease ch!",
		&dhcps_lease,
		sizeof(dhcps_lease));
	# endif

	// Set new AP dhcps_lease
	wifi_softap_set_dhcps_lease(&dhcps_lease);

	// set new DHCPS Status
	if (WifiSoftAPDHCPSStatus) tcpip_adapter_dhcpc_start(WIFI_IF_AP);
	else tcpip_adapter_dhcpc_stop(WIFI_IF_AP);

	}
*/



/*  // block #5 get the bandwith of the Service Access Point
  wifi_bandwidth_t ap_wifi_bandwidth;
  esp_wifi_get_bandwidth(WIFI_IF_AP, &ap_wifi_bandwidth);

  // block #6 get current Station Settings
  wifi_config_t sta_wifi_config;
  esp_wifi_get_config (WIFI_IF_STA, &sta_wifi_config);*/



/*
  // Processed block 05 readings, that affects AP dhcps status (Service-Access-Point-Configuration) ? then save & set
  if (affectedReadings & ( SCDE_R_WSAP ) )

	{

	// Set new AP opmode
	esp_wifi_set_mode( (esp_wifi_get_mode() & 0b01) | (WifiApOpMode << 1) );

	}
*/






/*
 // Processed block 07 readings, that affects STA opmode (Station-Configuration) ? then save & set
  if (affectedReadings & ( SCDE_R_Station ) )

	{

	// Set new AP opmode
 	//  int WifiStaOpMode = (wifi_get_opmode() & 0b01);

	}


  // Processed block 08 readings, that affects STA dhcp (Station-Configuration) ? then save & set
  if (affectedReadings & ( SCDE_R_Station_DHCP ) )

	{

	// Set new ?
 	//  int WifiStationDHCPStatus = wifi_station_dhcpc_status();

	tcpip_adapter_dhcps_start(tcpip_adapter_if_t tcpip_if);
	tcpip_adapter_dhcps_stop(tcpip_adapter_if_t tcpip_if);
	}


  // Processed block 9 readings, that affects STA auto connect status (Station-Configuration) ? then save & set
  if (affectedReadings & ( SCDE_R_Station_Auto_Connect ) )

	{

	// Set new STA auto connect status
 	//wifi_station_set_auto_connect(WifiStationAutoConnectStatus);

	}


 // Processed block 10 readings, that affects STA phy mode (Station-Configuration) ? then save & set
  if (affectedReadings & ( SCDE_R_Station_Physical_Mode ) )

	{

	// Set new STA phy mode
 	wifi_set_phy_mode(WifiStationPhyMode);

	}
*/






  // debugview all readings
  affectedReadings = 0xffffffff;

// =================================================================================================

  // prepare the readings update according to 'affectedReadings'

  // do we have updates?
  if (affectedReadings) {

  SCDEFn->readingsBeginUpdateFn(Common_Definition);

  }

  // temp for reading creation
  uint8_t *readingNameText;
  size_t readingNameTextLen;
  uint8_t *readingValueText;
  size_t readingValueTextLen;

// ------------------------------------------------------------------------------------------------- 

  // add Reading 'Name'
  if (affectedReadings & SCDE_R_NAME) {

	#if SCDEH_DBG >= 5
	printf("&NAME=%s"
		,(char*) &ap_wifi_config.ap.ssid);
	#endif


	readingNameTextLen = asprintf((char **) &readingNameText
		,"Name");

	readingValueTextLen = asprintf((char **) &readingValueText
		,"%.*s"
		,strlen((char *) &ap_wifi_config.ap.ssid)
		,(char *) &ap_wifi_config.ap.ssid);		// max length check??

	SCDEFn->readingsBulkUpdateFn(Common_Definition
		,readingNameText
		,readingNameTextLen
		,readingValueText
		,readingValueTextLen);

  }

// -------------------------------------------------------------------------------------------------

  // add Reading 'WSAP_Password'
  if (affectedReadings & SCDE_R_WSAP_Password) {

	#if SCDEH_DBG >= 5
	printf("&WSAP_Password=%s"
		,(char*) &ap_wifi_config.ap.password);
	#endif


	readingNameTextLen = asprintf((char **) &readingNameText
		,"WSAP_Password");

	readingValueTextLen = asprintf((char **) &readingValueText
		,"%.*s"
		,strlen((char *) &ap_wifi_config.ap.password)
		,(char *) &ap_wifi_config.ap.password);		// max length check??

	SCDEFn->readingsBulkUpdateFn(Common_Definition
		,readingNameText
		,readingNameTextLen
		,readingValueText
		,readingValueTextLen);

  }

// ------------------------------------------------------------------------------------------------- 

  // add Reading 'WSAP_RF_Channel'
  if (affectedReadings & SCDE_R_WSAP_RF_Channel) {

	#if SCDEH_DBG >= 5
	printf("&WSAP_RF_Channel=%u"
		,ap_wifi_config.ap.channel);
	#endif


	readingNameTextLen = asprintf((char **) &readingNameText
		,"WSAP_RF_Channel");

	readingValueTextLen = asprintf((char **) &readingValueText
		,"%u"
		,ap_wifi_config.ap.channel);

	SCDEFn->readingsBulkUpdateFn(Common_Definition
		,readingNameText
		,readingNameTextLen
		,readingValueText
		,readingValueTextLen);

  }

// ------------------------------------------------------------------------------------------------- 

  // add Reading 'WSAP_Maximal_Connections'
  if (affectedReadings & SCDE_R_WSAP_Maximal_Connections) {

	#if SCDEH_DBG >= 5
	printf("&WSAP_Maximal_Connections=%u"
		,ap_wifi_config.ap.max_connection);
	#endif


	readingNameTextLen = asprintf((char **) &readingNameText
		,"WSAP_Maximal_Connections");

	readingValueTextLen = asprintf((char **) &readingValueText
		,"%u"
		,ap_wifi_config.ap.max_connection);

	SCDEFn->readingsBulkUpdateFn(Common_Definition
		,readingNameText
		,readingNameTextLen
		,readingValueText
		,readingValueTextLen);

  }

// ------------------------------------------------------------------------------------------------- 

  // add Reading 'WSAP_Authentication_Method'
  if (affectedReadings & SCDE_R_WSAP_Authentication_Method) {

	#if SCDEH_DBG >= 5
	printf("&WSAP_Authentication_Method=%s"
		,SCDE_GetDesc(Auth_M,ap_wifi_config.ap.authmode));
	#endif


	readingNameTextLen = asprintf((char **) &readingNameText
		,"WSAP_Authentication_Method");

	readingValueTextLen = asprintf((char **) &readingValueText
		,"%s"
		,SCDE_GetDesc(Auth_M,ap_wifi_config.ap.authmode));

	SCDEFn->readingsBulkUpdateFn(Common_Definition
		,readingNameText
		,readingNameTextLen
		,readingValueText
		,readingValueTextLen);

  }

// ------------------------------------------------------------------------------------------------- 

  // add Reading 'WSAP_SSID'
  if (affectedReadings & SCDE_R_WSAP_SSID) {

	#if SCDEH_DBG >= 5
	printf("&WSAP_SSID=%s"
		,SCDE_GetDesc(SSID_H,ap_wifi_config.ap.ssid_hidden));
	#endif


	readingNameTextLen = asprintf((char **) &readingNameText
		,"WSAP_SSID");

	readingValueTextLen = asprintf((char **) &readingValueText
		,"%s"
		,SCDE_GetDesc(SSID_H,ap_wifi_config.ap.ssid_hidden));

	SCDEFn->readingsBulkUpdateFn(Common_Definition
		,readingNameText
		,readingNameTextLen
		,readingValueText
		,readingValueTextLen);

  }

// ------------------------------------------------------------------------------------------------- 

  // add Reading 'WSAP_Beacon_Interval'
  if (affectedReadings & SCDE_R_WSAP_Beacon_Interval) {

	#if SCDEH_DBG >= 5
	printf("&WSAP_Beacon_Interval=%u"
		,ap_wifi_config.ap.beacon_interval);
	#endif


	readingNameTextLen = asprintf((char **) &readingNameText
		,"WSAP_RF_Channel");

	readingValueTextLen = asprintf((char **) &readingValueText
		,"%u"
		,ap_wifi_config.ap.beacon_interval);

	SCDEFn->readingsBulkUpdateFn(Common_Definition
		,readingNameText
		,readingNameTextLen
		,readingValueText
		,readingValueTextLen);

  }

// -------------------------------------------------------------------------------------------------

  // add Reading 'WSAP_IP_Adress'
  if (affectedReadings & SCDE_R_WSAP_IP_Adress) {

	#if SCDEH_DBG >= 5
	printf("&WSAP_IP_Adress=%03d.%03d.%03d.%03d"
		,ap_ip_info.ip.addr & 0xff
		,(ap_ip_info.ip.addr >> 8) & 0xff
		,(ap_ip_info.ip.addr >> 16) & 0xff
		,ap_ip_info.ip.addr >> 24);
	#endif


	readingNameTextLen = asprintf((char **) &readingNameText
		,"WSAP_IP_Adress");

	readingValueTextLen = asprintf((char **) &readingValueText
		,"%03d.%03d.%03d.%03d"
		,ap_ip_info.ip.addr & 0xff
		,(ap_ip_info.ip.addr >> 8) & 0xff
		,(ap_ip_info.ip.addr >> 16) & 0xff
		,ap_ip_info.ip.addr >> 24);

	SCDEFn->readingsBulkUpdateFn(Common_Definition
		,readingNameText
		,readingNameTextLen
		,readingValueText
		,readingValueTextLen);

  }

// -------------------------------------------------------------------------------------------------

  // add Reading 'WSAP_Netmask'
  if (affectedReadings & SCDE_R_WSAP_Netmask) {

	#if SCDEH_DBG >= 5
	printf("&WSAP_Netmask=%03d.%03d.%03d.%03d"
		,ap_ip_info.netmask.addr & 0xff
		,(ap_ip_info.netmask.addr >> 8) & 0xff
		,(ap_ip_info.netmask.addr >> 16) & 0xff
		,ap_ip_info.netmask.addr >> 24);
	#endif


	readingNameTextLen = asprintf((char **) &readingNameText
		,"WSAP_Netmask");

	readingValueTextLen = asprintf((char **) &readingValueText
		,"%03d.%03d.%03d.%03d"
		,ap_ip_info.netmask.addr & 0xff
		,(ap_ip_info.netmask.addr >> 8) & 0xff
		,(ap_ip_info.netmask.addr >> 16) & 0xff
		,ap_ip_info.netmask.addr >> 24);

	SCDEFn->readingsBulkUpdateFn(Common_Definition
		,readingNameText
		,readingNameTextLen
		,readingValueText
		,readingValueTextLen);

  }

// -------------------------------------------------------------------------------------------------

  // add Reading 'WSAP_Gateway_Adress'
  if (affectedReadings & SCDE_R_WSAP_Gateway_Adress) {

	#if SCDEH_DBG >= 5
	printf("&WSAP_Gateway_Adress=%03d.%03d.%03d.%03d"
		,ap_ip_info.gw.addr & 0xff
		,(ap_ip_info.gw.addr >> 8) & 0xff
		,(ap_ip_info.gw.addr >> 16) & 0xff
		,ap_ip_info.gw.addr >> 24);
	#endif


	readingNameTextLen = asprintf((char **) &readingNameText
		,"WSAP_Gateway_Adress");

	readingValueTextLen = asprintf((char **) &readingValueText
		,"%03d.%03d.%03d.%03d"
		,ap_ip_info.gw.addr & 0xff
		,(ap_ip_info.gw.addr >> 8) & 0xff
		,(ap_ip_info.gw.addr >> 16) & 0xff
		,ap_ip_info.gw.addr >> 24);

	SCDEFn->readingsBulkUpdateFn(Common_Definition
		,readingNameText
		,readingNameTextLen
		,readingValueText
		,readingValueTextLen);
  }

// -------------------------------------------------------------------------------------------------

  // add Reading 'WSAP_MAC_Adress'
  if (affectedReadings & SCDE_R_WSAP_MAC_Adress) {

	#if SCDEH_DBG >= 5
	printf("&WSAP_MAC_Adress=%02X.%02X.%02X.%02X.%02X.%02X.%02X.%02X"
		,ap_mac_addr[0]
		,ap_mac_addr[1]
		,ap_mac_addr[2]
		,ap_mac_addr[3]
		,ap_mac_addr[4]
		,ap_mac_addr[5]
		,ap_mac_addr[6]
		,ap_mac_addr[7]);
	#endif


	readingNameTextLen = asprintf((char **) &readingNameText
		,"WSAP_MAC_Adress");

	readingValueTextLen = asprintf((char **) &readingValueText
		,"%02X.%02X.%02X.%02X.%02X.%02X.%02X.%02X"
		,ap_mac_addr[0]
		,ap_mac_addr[1]
		,ap_mac_addr[2]
		,ap_mac_addr[3]
		,ap_mac_addr[4]
		,ap_mac_addr[5]
		,ap_mac_addr[6]
		,ap_mac_addr[7]);

	SCDEFn->readingsBulkUpdateFn(Common_Definition
		,readingNameText
		,readingNameTextLen
		,readingValueText
		,readingValueTextLen);

  }

// -------------------------------------------------------------------------------------------------

  // add Reading 'WSAP_WiFi_Bandwidth'
  if (affectedReadings & SCDE_R_WSAP_WiFi_Bandwidth) {

	#if SCDEH_DBG >= 5
	printf("&WSAP_WiFi_Bandwidth=%s"
		,SCDE_GetDesc(wifiBandwidth, ap_wifi_bandwidth));	
	#endif


	readingNameTextLen = asprintf((char **) &readingNameText
		,"WSAP_WiFi_Bandwidth");

	readingValueTextLen = asprintf((char **) &readingValueText
		,"%s"
		,SCDE_GetDesc(wifiBandwidth, ap_wifi_bandwidth));

	SCDEFn->readingsBulkUpdateFn(Common_Definition
		,readingNameText
		,readingNameTextLen
		,readingValueText
		,readingValueTextLen);

  }

// -------------------------------------------------------------------------------------------------

  // add Reading 'WiFi_Country'
  if (affectedReadings & SCDE_R_WiFi_Country) {

	#if SCDEH_DBG >= 5
	printf("&WiFi_Country=%s"
		,SCDE_GetDesc(wifiCountry, &country));
	#endif


	readingNameTextLen = asprintf((char **) &readingNameText
		,"WiFi_Country");

	readingValueTextLen = asprintf((char **) &readingValueText
		,"%s"
		,SCDE_GetDesc(wifiCountry, &country));

	SCDEFn->readingsBulkUpdateFn(Common_Definition
		,readingNameText
		,readingNameTextLen
		,readingValueText
		,readingValueTextLen);

  }

// -------------------------------------------------------------------------------------------------

  // add Reading 'Station_IP_Adress'
  if (affectedReadings & SCDE_R_Station_IP_Adress) {

	#if SCDEH_DBG >= 5
	printf("&Station_IP_Adress=%03d.%03d.%03d.%03d"
		,sta_ip_info.ip.addr & 0xff
		,(sta_ip_info.ip.addr >> 8) & 0xff
		,(sta_ip_info.ip.addr >> 16) & 0xff
		,sta_ip_info.ip.addr >> 24);
	#endif


	readingNameTextLen = asprintf((char **) &readingNameText
		,"Station_IP_Adress");

	readingValueTextLen = asprintf((char **) &readingValueText
		,"%03d.%03d.%03d.%03d"
		,sta_ip_info.ip.addr & 0xff
		,(sta_ip_info.ip.addr >> 8) & 0xff
		,(sta_ip_info.ip.addr >> 16) & 0xff
		,sta_ip_info.ip.addr >> 24);

	SCDEFn->readingsBulkUpdateFn(Common_Definition
		,readingNameText
		,readingNameTextLen
		,readingValueText
		,readingValueTextLen);

  }

// -------------------------------------------------------------------------------------------------

  // add Reading 'Station_Netmask'
  if (affectedReadings & SCDE_R_Station_Netmask) {

	#if SCDEH_DBG >= 5
	printf("&Station_Netmask=%03d.%03d.%03d.%03d"
		,sta_ip_info.netmask.addr & 0xff
		,(sta_ip_info.netmask.addr >> 8) & 0xff
		,(sta_ip_info.netmask.addr >> 16) & 0xff
		,sta_ip_info.netmask.addr >> 24);
	#endif


	readingNameTextLen = asprintf((char **) &readingNameText
		,"Station_Netmask");

	readingValueTextLen = asprintf((char **) &readingValueText
		,"%03d.%03d.%03d.%03d"
		,sta_ip_info.netmask.addr & 0xff
		,(sta_ip_info.netmask.addr >> 8) & 0xff
		,(sta_ip_info.netmask.addr >> 16) & 0xff
		,sta_ip_info.netmask.addr >> 24);

	SCDEFn->readingsBulkUpdateFn(Common_Definition
		,readingNameText
		,readingNameTextLen
		,readingValueText
		,readingValueTextLen);

  }

// -------------------------------------------------------------------------------------------------

  // add Reading 'Station_Gateway_Adress'
  if (affectedReadings & SCDE_R_Station_Gateway_Adress) {

	#if SCDEH_DBG >= 5
	printf("&Station_Gateway_Adress=%03d.%03d.%03d.%03d"
		,sta_ip_info.gw.addr & 0xff
		,(sta_ip_info.gw.addr >> 8) & 0xff
		,(sta_ip_info.gw.addr >> 16) & 0xff
		,sta_ip_info.gw.addr >> 24);
	#endif


	readingNameTextLen = asprintf((char **) &readingNameText
		,"Station_Gateway_Adress");

	readingValueTextLen = asprintf((char **) &readingValueText
		,"%03d.%03d.%03d.%03d"
		,sta_ip_info.gw.addr & 0xff
		,(sta_ip_info.gw.addr >> 8) & 0xff
		,(sta_ip_info.gw.addr >> 16) & 0xff
		,sta_ip_info.gw.addr >> 24);

	SCDEFn->readingsBulkUpdateFn(Common_Definition
		,readingNameText
		,readingNameTextLen
		,readingValueText
		,readingValueTextLen);

  }

// -------------------------------------------------------------------------------------------------

  // add Reading 'Station_MAC_Adress'
  if (affectedReadings & SCDE_R_Station_MAC_Adress) {

	#if SCDEH_DBG >= 5
	printf("&Station_MAC_Adress=%02X.%02X.%02X.%02X.%02X.%02X.%02X.%02X"
		,sta_mac_addr[0]
		,sta_mac_addr[1]
		,sta_mac_addr[2]
		,sta_mac_addr[3]
		,sta_mac_addr[4]
		,sta_mac_addr[5]
		,sta_mac_addr[6]
		,sta_mac_addr[7]);
	#endif


	readingNameTextLen = asprintf((char **) &readingNameText
		,"Station_MAC_Adress");

	readingValueTextLen = asprintf((char **) &readingValueText
		,"%02X.%02X.%02X.%02X.%02X.%02X.%02X.%02X"
		,sta_mac_addr[0]
		,sta_mac_addr[1]
		,sta_mac_addr[2]
		,sta_mac_addr[3]
		,sta_mac_addr[4]
		,sta_mac_addr[5]
		,sta_mac_addr[6]
		,sta_mac_addr[7]);

	SCDEFn->readingsBulkUpdateFn(Common_Definition
		,readingNameText
		,readingNameTextLen
		,readingValueText
		,readingValueTextLen);

  }

// -------------------------------------------------------------------------------------------------

  // add Reading 'Station_Auto_Connect'
  if (affectedReadings & SCDE_R_Station_Auto_Connect) {

	#if SCDEH_DBG >= 5
	printf("&Station_Auto_Connect=%s"
		,SCDE_GetDesc(DisEn, sta_autoconnect_status));
	#endif


	readingNameTextLen = asprintf((char **) &readingNameText
		,"Station_Auto_Connect");

	readingValueTextLen = asprintf((char **) &readingValueText
		,"%s"
		,SCDE_GetDesc(DisEn, sta_autoconnect_status));

	SCDEFn->readingsBulkUpdateFn(Common_Definition
		,readingNameText
		,readingNameTextLen
		,readingValueText
		,readingValueTextLen);

  }

// -------------------------------------------------------------------------------------------------

  // add Reading 'Station_WiFi_Bandwidth'
  if (affectedReadings & SCDE_R_Station_WiFi_Bandwidth) {

	#if SCDEH_DBG >= 5
	printf("&Station_WiFi_Bandwidth=%s"
		,SCDE_GetDesc(wifiBandwidth, sta_wifi_bandwidth));
	#endif


	readingNameTextLen = asprintf((char **) &readingNameText
		,"Station_WiFi_Bandwidth");

	readingValueTextLen = asprintf((char **) &readingValueText
		,"%s"
		,SCDE_GetDesc(wifiBandwidth, sta_wifi_bandwidth));

	SCDEFn->readingsBulkUpdateFn(Common_Definition
		,readingNameText
		,readingNameTextLen
		,readingValueText
		,readingValueTextLen);

  }






















/*
//> Add Readings: Internals - TiSt-Sharing - Block 3 ------------------------------------------------

  if (affectedReadings & (SCDE_R_WSAP_DHCPS_Lease_Start_IP
		| SCDE_R_WSAP_DHCPS_Lease_End_IP
		| SCDE_R_WSAP_DHCPS ) ) {

	RespArgsWPos += os_sprintf( RespArgsWPos
		,"&WSAP_DHCPS_Lease_Start_IP=%03d.%03d.%03d.%03d"
		 "&WSAP_DHCPS_Lease_End_IP=%03d.%03d.%03d.%03d"
		 "&WSAP_DHCPS=%s"
		, dhcps_lease.start_ip.addr & 0xff
		,(dhcps_lease.start_ip.addr >> 8) & 0xff
		,(dhcps_lease.start_ip.addr >> 16) & 0xff
		, dhcps_lease.start_ip.addr >> 24
		, dhcps_lease.end_ip.addr & 0xff
		,(dhcps_lease.end_ip.addr >> 8) & 0xff
		,(dhcps_lease.end_ip.addr >> 16) & 0xff
		, dhcps_lease.end_ip.addr >> 24
		,SCDE_GetDesc(DisEn, WifiSoftAPDHCPSStatus));
  }
*/

/*
//> Add Readings: Internals - TiSt-Sharing - Block 5 ------------------------------------------------

  if (affectedReadings & SCDE_R_WSAP) {

	RespArgsWPos += os_sprintf( RespArgsWPos
		,"&WSAP=%s"
		,SCDE_GetDesc(DisEn, WifiApOpMode));

  }
*/

/*
//> Add Readings: Internals - TiSt-Sharing - Block 7 ------------------------------------------------

  if (affectedReadings & SCDE_R_Station) {

	RespArgsWPos += os_sprintf( RespArgsWPos
		,"&Station=%s"
		,SCDE_GetDesc(DisEn, WifiStaOpMode) );

  }

//> Add Readings: Internals - TiSt-Sharing - Block 8 ------------------------------------------------

  if (affectedReadings & SCDE_R_Station_DHCP) {

	RespArgsWPos += os_sprintf( RespArgsWPos
		,"&Station_DHCP=%s"
		,SCDE_GetDesc(DisEn, WifiStationDHCPStatus));

  }
*/

/*
//> Add Readings: Internals - TiSt-Sharing - Block 10 ------------------------------------------------

  if (affectedReadings & SCDE_R_Station_Physical_Mode) {

	RespArgsWPos += os_sprintf( RespArgsWPos
		,"&Station_Physical_Mode=%s"
		,SCDE_GetDesc(PhMode, WifiStationPhyMode));

  }
*/

/*
//> Add Readings: Internals - TiSt-Sharing - Block 12 ------------------------------------------------

  if (affectedReadings & (SCDE_R_TiSt_NTP_Server_Main
		| SCDE_R_TiSt_NTP_Server_Backup_1
		| SCDE_R_TiSt_NTP_Server_Backup_2
		| SCDE_R_TiSt_Time_Zone) ) {

	RespArgsWPos += os_sprintf( RespArgsWPos
		,"&TiSt_NTP_Server_Main=%s"
		 "&TiSt_NTP_Server_Backup_1=%s"
		 "&TiSt_NTP_Server_Backup_2=%s"
		 "&TiSt_Time_Zone=%d"
		,sntp_getservername(0)
		,sntp_getservername(1)
		,sntp_getservername(2)
		,sntp_get_timezone());

  }

//> Add Readings: Readings - TiSt-Sharing - Block 1 ------------------------------------------------

  if (affectedReadings & (SCDE_R_Station_RSSI
		| SCDE_R_TiSt_Time) ) {

	RespArgsWPos += os_sprintf( RespArgsWPos
		,"&Station_RSSI=%d"
		 "&TiSt_Time=%s"
		,wifi_station_get_rssi()
		,sntp_get_real_time(sntp_get_current_timestamp()));

  }

//> Add Readings: Readings - TiSt-Sharing - Block 2 ------------------------------------------------

  if (affectedReadings & SCDE_R_Station_RSSI) {

	RespArgsWPos += os_sprintf( RespArgsWPos
		,"&SoC_Info=SDKV:%s ChID:%X BootV:%d BootP:%X FlashID:%X"
		 "&Firmware=V %u.%u, Build %u"
		,system_get_sdk_version()
		,system_get_chip_id()
		,system_get_boot_version()
		,system_upgrade_userbin_check()
		,spi_flash_get_id()
		,(unsigned int) SysCfgRamFlashMirrored->Version_major
		,(unsigned int) SysCfgRamFlashMirrored->Version_minor
		,(unsigned int) SysCfgRamFlashMirrored->build);

  }


//>always add Reading - state ----------------------------------------------------------------------

	// are we adding ?
//problem	if (RespArgsWPos != RespArgsBuf) RespArgsWPos += os_sprintf(RespArgsWPos,"&");

	// always add state ! on / off
	if ( (SysCfgRamRtcMirrored->MySCDE_FeatCfgRamRtcMirrored_M1[ADID].RB01_RGBPixel[0] == 0) &
		(SysCfgRamRtcMirrored->MySCDE_FeatCfgRamRtcMirrored_M1[ADID].RB01_RGBPixel[1] == 0) &
		(SysCfgRamRtcMirrored->MySCDE_FeatCfgRamRtcMirrored_M1[ADID].RB01_RGBPixel[2] == 0) &
		(SysCfgRamRtcMirrored->MySCDE_FeatCfgRamRtcMirrored_M1[ADID].RB02_WWPixel[0] == 0) &
		(SysCfgRamRtcMirrored->MySCDE_FeatCfgRamRtcMirrored_M1[ADID].RB03_CWPixel[0] == 0) )
		RespArgsWPos += os_sprintf(RespArgsWPos, "state=off");
	else  RespArgsWPos += os_sprintf(RespArgsWPos, "state=on");
	RespArgsWPos += os_sprintf(RespArgsWPos, "state=todo");

//--------------------------------------------------------------------------------------------------
*/

  // free allocated memory for query result key-field
  free(parsedKVInput);




  // were there any updates?
  if (affectedReadings) {

  SCDEFn->readingsEndUpdateFn(Common_Definition);

  }


























  // make common ptr to modul specific ptr
  ESP32_Control_Definition_t* ESP32_Control_Definition = (ESP32_Control_Definition_t*) Common_Definition;

  #if SCDEH_DBG >= 5
  printf("\n|ESP32_Control_Set, Name:%.*s, got args:%.*s>"
	,ESP32_Control_Definition->common.nameLen
	,ESP32_Control_Definition->common.name
	,setArgsLen
	,setArgs);
  #endif


  // response with error text
  // response with error text
	// alloc mem for retMsg
  retMsg = malloc(sizeof(strTextMultiple_t));

  // response with error text
  retMsg->strTextLen = asprintf(&retMsg->strText
	,"ESP32_Control_Set, Name:%.*s, got args:%.*s"
	,ESP32_Control_Definition->common.nameLen
	,ESP32_Control_Definition->common.name
	,setArgsLen
	,setArgs);

  return retMsg;

  }



/**
 * -------------------------------------------------------------------------------------------------
 *  FName: ESP32_Control_State
 *  Desc: FN is called when a Definition of this Module gets, and should set an state update. 
 *        Normally called from setstate cmd when executing state (=readings?) recovery from save .cfg
 *  Info: An FN for State in an Module is optional!
 *  Para: const Common_Definition_t *ESP32_Control_Definition -> Definition that should execute an status update
 *        const time_t stateTiSt -> time stamp for status update / reading value
 *        const xString_t stateNameString -> state Name for status update / reading value
 *        const xString_t stateValueString -> state Value for the status update / reading value
 *        const xString_t stateMimeString -> state Mime for the status update / reading value
 *  Rets: xMultipleStringSLTQE_t -> singly linked tail queue element which stores one xString_t string
 *                                  as response text in allocated memory. NULL=no response text
 * -------------------------------------------------------------------------------------------------
 */
xMultipleStringSLTQE_t*
ESP32_Control_State(Common_Definition_t *Common_Definition
	,const time_t stateTiSt
	,const xString_t stateNameString
	,const xString_t stateValueString
	,const xString_t stateMimeString)
{
	
  // Fn return message. NULL = no return message
  xMultipleStringSLTQE_t *retMsgMultipleStringSLTQE = NULL;
	
  // make common ptr to modul specific ptr
  ESP32_Control_Definition_t* ESP32_Control_Definition =
		(ESP32_Control_Definition_t*) Common_Definition;

// -------------------------------------------------------------------------------------------------

  #if ESP32_Control_Module_DBG >= 5
  // prepare TiSt-string for LogFn
  strText_t timeString =
  	SCDEFn->FmtDateTimeFn(stateTiSt);

  SCDEFn->Log3Fn(Common_Definition->name
	,Common_Definition->nameLen
	,5
	,"Executing StateFn of Module '%.*s' for Definition '%.*s'. "
         "Should set State for '%.*s' with Value '%.*s' and Mime '%.*s'. TimeStamp '%.*s'."
	,ESP32_Control_Definition->common.module->ProvidedByModule->typeNameLen
	,ESP32_Control_Definition->common.module->ProvidedByModule->typeName
	,stateNameString.length
	,stateNameString.characters
	,stateValueString.length
	,stateValueString.characters
	,stateMimeString.length
	,stateMimeString.characters
	,timeString.strTextLen
	,timeString.strText);

  // free TiSt-string from LogFn
  free(timeString.strText);
  #endif

// -------------------------------------------------------------------------------------------------

  return retMsgMultipleStringSLTQE;
}




/**
 * --------------------------------------------------------------------------------------------------
 *  FName: ESP32_Control_Undefine
 *  Desc: Removes the define of an "device" of 'WebIF' type. Contains devicespecific init code.
 *  Info: Invoked by cmd-line 'Undefine ESP32_Control_Definition.common.Name'
 *  Para: ESP32_Control_Definition_t *ESP32_Control_Definition -> WebIF Definition that should be removed
 *  Rets: strTextMultiple_t* -> response text NULL=no text
 * --------------------------------------------------------------------------------------------------
 */
strTextMultiple_t* ICACHE_FLASH_ATTR
ESP32_Control_Undefine(Common_Definition_t* Common_Definition)
  {

  // for Fn response msg
  strTextMultiple_t *retMsg = NULL;

  // make common ptr to modul specific ptr
  ESP32_Control_Definition_t* ESP32_Control_Definition =
	(ESP32_Control_Definition_t*) Common_Definition;

  #if SCDEH_DBG >= 5
  printf("\n|ESP32_Control_Undefine, Name:%.*s>"
	,ESP32_Control_Definition->common.nameLen
	,ESP32_Control_Definition->common.name);
  #endif


  // response with error text
	// alloc mem for retMsg
  retMsg = malloc(sizeof(strTextMultiple_t));

  // response with error text
  retMsg->strTextLen = asprintf(&retMsg->strText
	,"ESP32_Control_Set, Name:%.*s"
	,ESP32_Control_Definition->common.nameLen
	,ESP32_Control_Definition->common.name);

  return retMsg;

  }













/* -------------------------------------------------------------------------------------------------
 *  FName: ESP32_Control_Attribute
 *  Desc: Informs an Definition of this Type for attribute activities (set/del)
 *  Info: 'attrCmd' is the command text for the activity: set, del
 *        'attrName' is the attribute name text
 *        'attrVal' is the attribute value text
 *        THE attrVal CAN BE MANIPULATED / CORRECTED
 *  Para: Common_Definition_t* Common_Definition -> the belonging definition for the attribute-activitie
 *	  const uint8_t *attrCmdText -> ptr to attribute-command text "attrCmd"
 *	  const size_t attrCmdTextLen -> length of attribute-command text "attrCmd"
 *	  const uint8_t *attrNameText -> ptr to attribute-name text "attrName"
 *	  const size_t attrNameTextLen -> length of attribute-name text "attrName"
 *	  uint8_t **attrValTextPtr -> ptr to ptr holding attribute-value text "attrValue" - manipulation possible
 *	  size_t *attrValLenTextPtr -> ptr to length of attribute-value text "attrValue" - manipulation possible
 *  Rets: strTextMultiple_t* -> veto = error-text-string in allocated mem, or NULL = OK - no veto
 * -------------------------------------------------------------------------------------------------
 */
strTextMultiple_t* ICACHE_FLASH_ATTR
ESP32_Control_Attribute(Common_Definition_t* Common_Definition
		,const uint8_t *attrCmdText
		,const size_t attrCmdTextLen
		,const uint8_t *attrNameText
		,const size_t attrNameTextLen
		,uint8_t **attrValTextPtr
		,size_t *attrValTextLenPtr)

  {

  // for Fn response msg
  strTextMultiple_t *retMsg = NULL;

  uint8_t *attrValText = *attrValTextPtr;
  size_t attrValTextLen = *attrValTextLenPtr;

  printf("ESP32_Control_AttributeFN for defName:%.*s -> attrCmd:%.*s attrName:%.*s attrVal:%.*s\n"
	,Common_Definition->nameLen
	,Common_Definition->name
	,attrCmdTextLen
	,attrCmdText
  	,attrNameTextLen
	,attrNameText
	,(int) attrValTextLen
	,attrValText);



/*
  // set start of possible def-Name
  const uint8_t *defName = args;

  // set start of possible attr-Name
  const uint8_t *attrName = args;

  // a seek-counter
  int i = 0;

  // seek to next space !'\32'
  while( (i < argsLen) && (*attrName != ' ') ) {i++;attrName++;}

  // length of def-Name
  size_t defNameLen = i;

  // seek to start position of attr-Name '\32'
  while( (i < argsLen) && (*attrName == ' ') ) {i++;attrName++;}

  // set start of possible attr-Val
  const uint8_t *attrVal = attrName;

  // a 2nd seek-counter
  int j = 0;

  // seek to next space !'\32'
  while( (i < argsLen) && (*attrVal != ' ') ) {i++,j++;attrVal++;}

  // length of attr-Name
  size_t attrNameLen = j;

  // start position of attr-Val
  while( (i < argsLen) && (*attrVal == ' ') ) {i++;attrVal++;}

  // length of attr-Val
  size_t attrValLen = argsLen - i;

  // veryfy lengths > 0, definition 0 allowed
  if ( (defNameLen == 0) || (attrNameLen == 0) )

		{

		// response with error text
		asprintf(&retMsg
				,"Could not interpret command ''! Usage: Attr <defname> <attrname> [<attrval>]");

		return retMsg;

	//	}

// -------------------------------------------------------------------------------------------------
*/

  return retMsg;

}



/**
 * --------------------------------------------------------------------------------------------------
 *  FName: ESP32_Control_Shutdown
 *  Desc: called after 
 *  Info: Invoked by cmd-line 'Undefine ESP32_Control_Definition.common.Name'
 *  Para: ESP32_Control_Definition_t *ESP32_Control_Definition -> WebIF Definition that should be removed
 *  Rets: strTextMultiple_t* -> response text NULL=no text
 * --------------------------------------------------------------------------------------------------
 */
strTextMultiple_t* ICACHE_FLASH_ATTR
ESP32_Control_Shutdown(Common_Definition_t* Common_Definition)
  {

  // for Fn response msg
  strTextMultiple_t *retMsg = NULL;

  // make common ptr to modul specific ptr
  ESP32_Control_Definition_t* ESP32_Control_Definition =
	(ESP32_Control_Definition_t*) Common_Definition;

// -------------------------------------------------------------------------------------------------

  #if SCDEH_DBG >= 5
  printf("\n|ESP32_Control_Shutdown, Name:%.*s>"
	,ESP32_Control_Definition->common.nameLen
	,ESP32_Control_Definition->common.name);
  #endif

// -------------------------------------------------------------------------------------------------

  return retMsg;

  }











