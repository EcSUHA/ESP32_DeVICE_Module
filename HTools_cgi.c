//#################################################################################################
//###  Function: CGI for Hidden Tools Func
//###  ESP 8266EX SOC Activities ...
//###  (C) EcSUHA
//###  Maik Schulze, Sandfuhren 4, 38448 Wolfsburg, Germany 
//###  MSchulze780@GMAIL.COM
//###  EcSUHA - ECONOMIC SURVEILLANCE AND HOME AUTOMATION - WWW.EcSUHA.DE
//##################################################################################################
#include "ProjectConfig.h"
#include <esp8266.h>
#include "WebIf_Module.h"
#include "Platform.h"


//ToDo:
// update debug msgs


#define SCDE_WIFI_DBG 5

/*
#include "c_types.h"
#include <string.h>
#include <osapi.h>
#include "user_interface.h"
#include "mem.h"
#include "SCDED.h"
*/
//#include "CGI_StdUi.h"
//#include "espmissingincludes.h"
//#include "SCDE.h"
//#include "CloudUpdate.h"

#include "HTools_cgi.h"
#include "CGI_NoAuthErr.h"
#include "CGI_NotFoundErr.h"



/* --------------------------------------------------------------------------------------------------
 *  FName: ReadFullFlash_cgi
 *  Desc: Function to finally transmitts the data in the Send-Buffer (conn->sendBuff - if any)
 *        This should be a hidden function - File download of mirrored 1st Megabyte of SPI Flash 
 *        Methods: POST || GET ; Scheme: http ; Mime: cgi ; Url: SOCHWCfg
 *  Para: WebIf_HTTPDConnSlotData_t *conn -> ptr to connection slot
 *  Rets: int -> cgi result cmd
 * --------------------------------------------------------------------------------------------------
 */
int ICACHE_FLASH_ATTR
ReadFullFlash_cgi(WebIf_HTTPDConnSlotData_t *conn)
  {

  // Connection aborted? Nothing to clean up.
  if (conn->conn == NULL)
	return HTTPD_CGI_DISCONNECT_CONN;

// ---------------------------------------------------------------------------------------------------

  // Check if Parser is ready
  if (conn->SlotParserState != s_HTTPD_Msg_Prsd_Complete)
	return HTTPD_CGI_WAITRX_CONN;

// ---------------------------------------------------------------------------------------------------
/*spz
  // Check for valid authorization (AUTH_GENERIC_RESSOURCE)
  if (SCDED_AuthCheck(conn, AUTH_GENERIC_RESSOURCE))
	{
	// set callback for "Not Authorized Error"
	conn->cgi = NoAuthErr_cgi;
	return HTTPD_CGI_REEXECUTE;
	}
*/
//---------------------------------------------------------------------------------------------------
  # if SCDE_WIFI_DBG >= 3
  char *Args;
  if (conn->parser_method == HTTP_GET)		// GET Method ?
	{
	Args = conn->getArgs;
	}
  else //if (conn->parser_method == HTTP_POST)	// POST Method ?
	{
	// all post data buffered? If not continue receiving all post data to buffer ...
	if (conn->parser_content_length) return HTTPD_CGI_PROCESS_CONN;

	Args = conn->postBuff;
	}

 // # if SCDE_WIFI_DBG >= 3
  if (Args == NULL)
	{
	os_printf(".cgi->NO Args!");
	}
  else
	{
	os_printf(".cgi->Args:%s\n",Args);
	}
  # endif

//--------------------------------------------------------------------------------------------------

  // load TXed position
  int *pos = (int *) &conn->PCData;

  // Init / Start Downloading indicated by 0
  if (*pos == 0)

	{

	# if SCDE_WIFI_DBG >= 4
	os_printf("DL SPI-Flash.\n");
	# endif
	
	SCDED_StartRespHeader(conn, 200);
	SCDED_AddHdrFld(conn, "Content-Type", "application/bin", -1);
	SCDED_EndHeader(conn);

//  dram0_0_seg :                         org = 0x3FFE8000, len = 0x14000
//  iram1_0_seg :                         org = 0x40100000, len = 0x8000


//	*pos=0x40200000;
	*pos=0x3FFE8000;

//	return HTTPD_CGI_PROCESS_CONN;
	}

  // transfer next 1024 Byte Block
  SCDED_Send(conn, (const char *)(*pos), 1024);

  // increase TXed position
  *pos += 1024;
	
  // reached upper ROM-Mirror-End end = ready?
//  if (*pos>=0x40200000+(1024*1024))
  if (*pos>=0x3FFE8000 + 0x14000)
	return HTTPD_CGI_DISCONNECT_CONN;

  // else continue till 1-MByte done
  else return HTTPD_CGI_PROCESS_CONN;

  }




