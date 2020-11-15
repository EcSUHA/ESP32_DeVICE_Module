//#################################################################################################
//###  Function: CGI Code for SCDE Feature - Generic for SCD-Engine 2
//###  ESP 8266 / 32 / IoT SOC Activities ...
//###  (C) EcSUHA
//###  Maik Schulze, Sandfuhren 4, 38448 Wolfsburg, Germany 
//###  MSchulze780@GMAIL.COM
//###  EcSUHA - ECONOMIC SURVEILLANCE AND HOME AUTOMATION - WWW.EcSUHA.DE
//##################################################################################################

#include <ProjectConfig.h>
#include <esp8266.h>
#include <Platform.h>

// the Smart-Connected-Device-Engine structures & types ..
#include <SCDE_s.h>

// provides WebIf, need the structures & types ...
#include "WebIf_Module.h"

#include "ESP32_DeVICE_Module.h"
#include "ESP32_SCDE_WEBIF_get_cb.h"



/*
 *--------------------------------------------------------------------------------------------------
 *FName: ESP32_SCDE_WEBIF_get
 * Desc: For ESP32_SCDE - the WEBIF_get resource Remote Procedure (RP).
 *       Should be assinged to URI resources. Is used to process incoming get, ... requests
 *       to HTTP & HATP Server. POST ; Scheme HTTP ; Path opt. with UID ; Mime .get 
 *       Used Header-Fields:
 *       An Finite-State-Machine processes requests and responses with code and message.
 * Info: conn->getArgs -> Query String
 *       conn->body_data -> reserved for internals and attribute exchange
 * Para: HTTPDConnSlotData *conn -> ptr to connection slot
 * Rets: int -> get result cmd
 *--------------------------------------------------------------------------------------------------
 */

// for: SCDE_get RPC, Finite State Machine processing State - Stage 2 (after parsing finished)
enum SCDE_get_state
  {					// #XX for debugging
    S2S_get_Check_Stage1_Parser_State = 0// #00 check if parser of stage 1 is in a state that stage 2 can continue work
  , S2S_get_Action_S1_Parse_Body_Data	// #01 detected that stage 1 has not parsed the complete message. Processing body data
  , S2S_get_Action_S1_Msg_Prsd_Complete	// #02 detected that stage 1 has parsed the complete message including body data
  , S2S_get_Check_RPC_Authorization	// #03 Check if request contains the required authorization
  , S2S_get_Check_and_Get_ADID		// #04 Check and get the Active-Directory-ID
  , S2S_get_Parse_Query_String		// #05 Parse the Query String

  // done - start writing response to TX buff.
  // No Error messages after this State !		 	   
  , S2S_get_TX_Response_with_Code	// #06 add response to the TX (response) buffer. TX + Disconn if error code
  , S2S_get_TX_Add_HDR_Flds		// #07 add header fields (if any) to the TX (response) buffer
  , S2S_get_TX_Add_Body_Data		// #08 add body data (if any) to the TX (response) buffer
  , S2S_get_DONE_TX_Response_Buff	// #09 done, incl. response, now TX response buffer and then disconnect
  };

int
ESP32_SCDE_WEBIF_get(WebIf_HTTPDConnSlotData_t* conn)
{
  // connection aborted? nothing to clean up.
  if ( conn->conn == NULL ) 
	return HTTPD_CGI_DISCONNECT_CONN;

//---------------------------------------------------------------------------------------------------

  # if ESP32_SCDE_Module_DBG >= 4
  printf("|SCDE_WEBIF_get>");
  # endif

/*
  #if ESP32_SCDE_Module_DBG >= 5
  SCDEFn_at_ESP32_SCDE_M->Log3Fn(conn->p_entry_active_dir_matching_definition->name,
	conn->p_entry_active_dir_matching_definition->nameLen,
	5,
	"SCDE_get Fn of Module '%.*s' is called.",
	conn->p_entry_active_dir_matching_definition->module->provided->typeNameLen,
	conn->p_entry_active_dir_matching_definition->module->provided->typeName);
  #endif
*/

//---------------------------------------------------------------------------------------------------

  // init variables (that are only living this call)

  // pointer used to point to identified arguments (Query)
  char* Args = NULL; // null requred to prevent errror msg

  // the response message -> goes to body data
  Entry_String_t* respMsg = NULL;

  // the response header fields
//  strTextMultiple_t* respHdrFlds = NULL;



  // for extracted Active Directory ID
//  int16_t ADID = 0; // 0 blodsinn?

  // remember the readings affected by the parsing process
//  uint32 AffectedReadings = 0;



  // Response argument buffer & write position
//  char RespArgsBuf[512];
//  char* RespArgsWPos = RespArgsBuf;

  // Header Fields buffer & write position
//  char HdrFldsBuf[512];
//  char* HdrFldsWPos = NULL;

  // Body-Data buffer & write position
//  char BdyDataBuf[512];
//  char* BdyDataWPos = NULL;

  // store errors ...
//  int stage2_scde_errno = (enum http_errno) 0; //enum?

//--------------------------------------------------------------------------------------------------

  // stage 2 processing state
//  int S2_Statex;

  // enum state S2_State = (enum state) conn->S2_State;
  enum SCDE_get_state S2_State = (enum SCDE_get_state) S2S_get_Check_Stage1_Parser_State;

  // set initial stage 2 processing state (= 0)
 // S2_Statex = S2S_get_Check_Stage1_Parser_State;

  # if ESP32_SCDE_Module_DBG >= 4
  printf("|FST-Start>");
  # endif

  reexecute:

  // Debug output, Stage 2, FSM State,
  # if ESP32_SCDE_Module_DBG >= 4
  printf("|S2.%d>",
	S2_State);
  # endif

  switch (S2_State) {

//--------------------------------------------------------------------------------------------------

	// #00 check if parser of stage 1 is in a state that stage 2 can continue work

	case S2S_get_Check_Stage1_Parser_State:

	if (conn->SlotParserState != s_HTTPD_Msg_Prsd_Complete) {

		return HTTPD_CGI_WAITRX_CONN;
	}

//	else
//		{
//		// Header parsed, now parsing BodyData - update state
//		S2_State = Step_Msg_Prs_Body_Data;
//		}

	else {

		// Message parsed complete - update state
		S2_State = S2S_get_Action_S1_Msg_Prsd_Complete;
	}

	break;

//--------------------------------------------------------------------------------------------------

	case S2S_get_Action_S1_Parse_Body_Data: // #01

	break;

//--------------------------------------------------------------------------------------------------

	// #02 set Arguments according to method, check if arguments

	case S2S_get_Action_S1_Msg_Prsd_Complete:

	// GET Method ?
	if (conn->parser_method == HTTP_GET) {

		Args = conn->getArgs;

	}

	// POST Method ?
	else if (conn->parser_method == HTTP_POST) {

		Args = conn->p_body_data;

	}

	// do we have arguments ? -> Error & break if not
	if (!Args) {

		// no arguments error- finnish
		conn->parser_http_errno = HPE_INVALID_URL;

		// goto error;
		S2_State = S2S_get_TX_Response_with_Code;
	}

	// else go to next step
	else {

		#if ESP32_SCDE_Module_DBG >= 4
		printf("Args:%s>",
			Args);
  		#endif

		// we have args - go to next step
		S2_State = S2S_get_Check_RPC_Authorization;
	}

	break;

//--------------------------------------------------------------------------------------------------

	// #03 Check for valid authorization (AUTH_GENERIC_RESSOURCE)

	case S2S_get_Check_RPC_Authorization:

/* skip auth temp..
	if (SCDED_AuthCheck(conn, AUTH_GENERIC_RESSOURCE)) {

		// not authorized - finnish with error
		conn->parser_http_errno = HPE_NO_AUTH;

		// goto error;
		S2_State = S2S_get_TX_Response_with_Code;

	}

	else {

		// authorized - go to next step
		S2_State = S2S_get_Parse_Query_String; // SCHRITT übersprungen !!!S2S_get_Check_and_Get_ADID;
	}
*/S2_State = S2S_get_Parse_Query_String;
	break;

//--------------------------------------------------------------------------------------------------
/*
	// #04 Check & Get Active Directory ID (ADID)

	case S2S_get_Check_and_Get_ADID:
//ist immer valid!!!
	// Get index value from conn->ActiveDirID
	ADID  = conn->ActiveDirID -1;

	// resource available check ...
	if ( ADID >= SysCfgRamFlashMirrored->MySCDE_FeatHWCfg.NumFeat)

		{

		// Active Directory ID (ADID) out of range error- finnish with error
		conn->parser_http_errno = HPE_ADID_AOR;
		//goto error;
		S2_State = S2S_get_TX_Response_with_Code;

		}

	else {

		#if ESP32_SCDE_Module_DBG >= 4
		printf("ADID:%d>",
			ADID);
  		#endif

		// Got valid ADID - go to next step
		S2_State = S2S_get_Parse_Query_String;
	}

	break;
*/
//--------------------------------------------------------------------------------------------------

	// #05 Parse the Query String

	case S2S_get_Parse_Query_String:
	{

        char **argv;
        int argc;

        // split the query to args
        argv = SCDEFn_at_ESP32_DeVICE_M->ArgParse_SplitURLEncodedArgsToAllocatedMemFn(&argc, Args);

        respMsg = ESP32_DeVICE_GetV2(conn->p_entry_active_dir_matching_definition
              ,argv, argc);

        // ArgParse Split_ToArgsFn has allocated mem. Free it.
        SCDEFn_at_ESP32_DeVICE_M->ArgParse_FreeSplittedArgsInAllocatedMemFn(argv);

	// assuming processed OK
	conn->parser_http_errno = 0;

	// parsed query string  - go to next step
	S2_State = S2S_get_TX_Response_with_Code;

	}

	break;

//--------------------------------------------------------------------------------------------------

	// #06 add response to the TX (response) buffer. TX + Disconn if error code

	case S2S_get_TX_Response_with_Code:

	// start resonse header with code and desc.
	SCDED_StartRespHeader2(conn);

	// >0 -> error, something went wrong, disconn
	if (conn->parser_http_errno) {

		// response added to TX buf, code ERROR - NO header fields, NO body data !!!
		return HTTPD_CGI_DISCONNECT_CONN;
	}

	// response added to TX buf, code OK - go to next step
	S2_State = S2S_get_TX_Add_HDR_Flds;

	break;

//--------------------------------------------------------------------------------------------------

	// #07 add header fields (if any) to the TX (response) buffer

	case S2S_get_TX_Add_HDR_Flds:

	// add 'CONTENT-LENGHT' Hdr-Fld, if we will send data
	if (respMsg && respMsg->string.len) {

	  char *contentLengthStr;

          int contentLengthLen = asprintf(&contentLengthStr
			,"%d"
			,respMsg->string.len);

          if (contentLengthStr) {

		SCDED_AddHdrFld(conn, "CONTENT-LENGHT", contentLengthStr, contentLengthLen);

		free(contentLengthStr);
	  }
	}

	// end header by adding '\r\n'
	SCDED_EndHeader(conn);

	// header fields added - go to next step
	S2_State = S2S_get_TX_Add_Body_Data;

	break;

//--------------------------------------------------------------------------------------------------

	// #08 add body data (if any) to the TX (response) buffer

	case S2S_get_TX_Add_Body_Data:

	if (respMsg) {

		HTTPD_Send_To_Send_Buffer(conn
			,(char *) respMsg->string.p_char
			,respMsg->string.len);

		if (respMsg->string.p_char) free(respMsg->string.p_char);
		free(respMsg);
	}

	// body data added - go to next step
	S2_State = S2S_get_DONE_TX_Response_Buff;

	break;

//--------------------------------------------------------------------------------------------------

	// #09 done, incl. response, now TX response buffer and then disconnect
	case S2S_get_DONE_TX_Response_Buff:

	# if ESP32_SCDE_Module_DBG >= 4
	printf("|Done!>");
	# endif

	return HTTPD_CGI_DISCONNECT_CONN;

	break;

//--------------------------------------------------------------------------------------------------

	default:
	
	// set error number
	conn->parser_http_errno = HPE_UNHANDLED_STATE;

	// error number set - go to next step (TX response with code and disconnect)
	S2_State = S2S_get_TX_Response_with_Code;

	break;

//--------------------------------------------------------------------------------------------------

  }

  goto reexecute;

  return HTTPD_CGI_DISCONNECT_CONN;
}



