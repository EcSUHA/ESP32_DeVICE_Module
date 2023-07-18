//#################################################################################################
//###  Function: CGI Code for SwITCH Feature - Generic for SCD-Engine
//###  ESP 8266EX SOC Activities ...
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
#include "WebIf_Module_s.h"
#include "WebIf_Module.h"

#include "ESP32_DeVICE_Module.h"
#include "ESP32_DeVICE_Webif_Jso_Cb.h"

#include "CGI_NoAuthErr.h"
#include "CGI_NotFoundErr.h"



//##################################################################################################
//### FName: SwITCH_jso
//###  Desc: Creates DATA for client side Java Script and sends it (JSON)
//###        Req.Args: valid "conn->ActiveDirID" or args "fSwITCH + lSwITCH"
//###        no tist = full output!!! else:
//###        1st "ALL TIST CHECK" to detect anychange -> if not = Longpoll
//###        -> if yes output, but changed (NEWER TIST) only
//###  Para: PTR to HTTPDConnSlotData 
//###  Rets: -/-
//##################################################################################################
// Buffer-Write-Strategy defined by BIT-SET stored in conn->cgiData (initially zero):
// 00	-> StartResponseHeader
// 01   -> AddHdrFld -> content type
// 02   -> EndHeader
// 03   -> "jso Lead In"
// 04   -> "first row - no Komma" flag
// 05   -> "jso Main Data Rows"
// 06   -> "jso Lead Out"
// 30   -> "multi row - modules definitions" flag
// 31   -> "BWS assigned" flag
/*# Data Sample for one Switch.X.jso
{
 "STATUS": { 
"test": "0", 
"Switch": [
{"FUNC": "3", "NAME": "EG.Kinderzimmer.FBHHeizkreis", "VAL": "0", "MIN": "0", "MAX": "255", "DIM": "0"}
]
}
}
*/

// NOTES:
// conn->cgiData stores buffer write strategy
// conn->PCArg stores 'remote_tist'





int ICACHE_FLASH_ATTR 
ESP32_DeVICE_Webif_Jso_Cb(WebIf_HTTPDConnSlotData_t* conn)
{
  // Connection aborted? Nothing to clean up.
  if ( conn->conn == NULL )
	  return HTTPD_CGI_DISCONNECT_CONN;

//---------------------------------------------------------------------------------------------------

  // check if parser is ready
  if ( conn->SlotParserState != s_HTTPD_Msg_Prsd_Complete )
	  return HTTPD_CGI_WAITRX_CONN;

//---------------------------------------------------------------------------------------------------
/*later fix
  // check for valid authorization (AUTH_GENERIC_RESSOURCE)
  if (SCDED_AuthCheck(conn, AUTH_GENERIC_RESSOURCE)) {

	  // set callback for "Not Authorized Error"
	  conn->cgi = NoAuthErr_cgi;

	  return HTTPD_CGI_REEXECUTE;
  }	
*/
//---------------------------------------------------------------------------------------------------	

  char *Args;

  // GET Method ?
  if ( conn->parser_method == HTTP_GET ) {

	  Args = conn->getArgs;
  }
  // POST Method ?
  else { //if (conn->parser_method == HTTP_POST)

	  // all post data buffered? If not continue receiving all post data to buffer ...
	  if (conn->parser_content_length) return HTTPD_CGI_PROCESS_CONN;

	  Args = conn->p_body_data;
  }

//--------------------------------------------------------------------------------------------------

  # if ESP32_DeVICE_Module_DBG >= 3
  if ( Args == NULL ) {

	  printf("|.jso->NO Args!>");
  }

  else {

	  printf("|.jso->Args:%s>",Args);
  }
  # endif

//--------------------------------------------------------------------------------------------------

  // load Buffer-Write-Strategy (BWS)
  uint32_t BWS = (uint32_t) conn->PCData;

//--------------------------------------------------------------------------------------------------

  // fresh connection? no Buffer-Write-Strategy flag set ? Init BWS !
  if ( !BWS ) {

	  # if ESP32_DeVICE_Module_DBG >= 3
	  printf("|BWS,init>");
	  # endif

	  // An 'definition' is assigned? -> Output .jso for one specific 'definition'
	  // (http://192.168.0.29/1/SwITCH.jso) not assigned
	  // (http://192.168.0.29/1/myswitch.jso) assigned, if name matches definition name
	  if ( conn->p_entry_active_dir_matching_definition ) {

		  // - one specific definition assigned, 
		  // - "multi row + modules definitions" flag stays cleared,
		  // - single row output
	  }

	  // an 'definition' is NOT assigned! -> output .jso for all 'definition's of this 'module'
	  else {
	  
		  // get first 'entry definition' for this 'module'
  		  entry_common_definition_t* p_entry_common_definition = 
			  STAILQ_FIRST (&SCDERoot_at_ESP32_DeVICE_M->head_definition);

 		  // loop through the 'definition's till the 'module' matches 
		  while ( p_entry_common_definition != NULL ) {

			  // this 'module' ? break, keep !
			  if ( p_entry_common_definition->module->provided == &ESP32_DeVICE_ProvidedByModule ) break;

			  // get next 'definition' entry, to process in loop
			  p_entry_common_definition = STAILQ_NEXT(p_entry_common_definition, entries);
		  }

		  // no 'definition' from this 'module' found ? -> show not found error!
		  if (!p_entry_common_definition) {

			  // set callback for "not found error"
			  conn->cgi = NotFoundErr_cgi;

			  return HTTPD_CGI_REEXECUTE;
		  }

		  // store as start for Buffer-Write-Strategy
		  conn->p_entry_active_dir_matching_definition = p_entry_common_definition;

		  // set "multi row - modules definitions" flag	
		  BWS |= ( 1 << 30 );
	  }

//--------------------------------------------------------------------------------------------------

	  // may be we can do longpoll in case of no new values ?
	  int32_t remote_tist = -1;

	  // check if remote sent an "tist" (timestamp), 
	  if ( HTTPDFindValueDecInt32FromKey(Args, "TIST", &remote_tist)) {

	      // find the oldest module-feature 'tist' from 'definition's (needed for longpoll check)
		  int32_t newest_feat_tist = 0;
        
		  // get first 'entry_definition' to deal with
		  entry_common_definition_t *p_entry_common_definition = 
			  conn->p_entry_active_dir_matching_definition;

		  // for multi row output...  loop all definitions ...
		  do {	
			  // check if current entry definition belongs to this 'module'
			  if ( p_entry_common_definition->module->provided == &ESP32_DeVICE_ProvidedByModule ) {

			      // compare timestamps 'tist' from all readings
				  int32_t cmp_tist;

				  // get general / names 'tist' from this 'definition'
				  cmp_tist = p_entry_common_definition->timestamp;

				  // update 'newest_feat_tist'
				  if ( newest_feat_tist < cmp_tist ) newest_feat_tist = cmp_tist;

			      // loop readings stored in definition
			      entry_reading2_t *p_current_entry_reading;
			      STAILQ_FOREACH(p_current_entry_reading,
			          &p_entry_common_definition->head_readings2, entries) {

				      // get the 'timestamp' from this 'reading'
				      cmp_tist = p_current_entry_reading->reading.timestamp;

				      // update 'newest_feat_tist'
				      if ( newest_feat_tist < cmp_tist ) newest_feat_tist = cmp_tist;
			      }			  
			  }

			  // get next definition entry to process it
			  p_entry_common_definition = STAILQ_NEXT (p_entry_common_definition, entries);

  		  // loop if pending 'definition's an "multi row - modules definitions" flag is set
		  } while ( p_entry_common_definition && ( BWS & 1 << 30 ) );

		  // now check if remote data is complete up to date, if yes do longpoll ...
		  // ... else renew specific or all rows
		  if ( newest_feat_tist <= remote_tist ) {

			  #if ESP32_DeVICE_Module_DBG >= 4
			  printf("|rmt-TIST:%d,ofeat-TIST:%d,real-TIST:%d,LP!->%dsec>",
				  remote_tist,
				  newest_feat_tist,
				  (int)SCDEFn_at_ESP32_DeVICE_M->GetTiStFn(),
				  conn->LP10HzCounter);
			  #endif

			  // do longpoll, limit time (20 sec)
			  if ( conn->LP10HzCounter < 20 * 10 ) {

				  conn->LP10HzCounter++;

				  return HTTPD_CGI_PROCESS_CONN;
			  }
		  }
	  }

	  // use conn->PCArg for 'remote_tist' storage
	  conn->PCArg = (void *) remote_tist;

	  // make Buffer-Write-Strategy (BWS) complete, to build correct answer
	  BWS |= 0b10000000000000000000000001111111;
  }

//---------------------------------------------------------------------------------------------------

  // create response step by step, according to defined buffer-write-strategy
  int len;
  char buff[256];

//---------------------------------------------------------------------------------------------------

  // send start-response-header ?
  if ( BWS & 1 << 0 ) {

	  BWS &= ~( 1 << 0 );

	  //if (!HTTPDStartRespHeader(conn, 200)) return HTTPD_CGI_PROCESS_CONN;
	  if ( !SCDED_StartRespHeader2 (conn) ) return HTTPD_CGI_PROCESS_CONN;
  }

//---------------------------------------------------------------------------------------------------

  // add header fields?
  if (BWS & 1 << 1 ) {

	  BWS &= ~( 1 << 1 );

	  if ( !SCDED_AddHdrFld (conn, "Content-Type", "text/json", -1) ) return HTTPD_CGI_PROCESS_CONN;
  }

//---------------------------------------------------------------------------------------------------

  // send end header ?
  if ( BWS & 1 << 2 ) {

	  BWS &= ~( 1 << 2 );

	  if ( !SCDED_EndHeader (conn) ) return HTTPD_CGI_PROCESS_CONN;
  }

//---------------------------------------------------------------------------------------------------

  // send "jso Lead In" ?
  if ( BWS & 1 << 3 ) {

	  BWS &= ~( 1 << 3 );

	  len = sprintf (buff, "{\"STATUS\":{\"MEMORY\":\"%d\",\"TIME\":\"%u\",\n\"SCDE\":[\n",
		  0,//system_get_free_heap_size ()
		  (int)SCDEFn_at_ESP32_DeVICE_M->GetTiStFn());

	  if ( !HTTPD_Send_To_Send_Buffer (conn, buff, len) ) return HTTPD_CGI_PROCESS_CONN;
  }

//---------------------------------------------------------------------------------------------------

  // do "jso Main Data Rows" ?
  if ( BWS & 1 << 5 ) {

      // conn->PCArg stores 'remote_tist'
	  int32_t remote_tist =  (uint32_t) conn->PCArg;

      entry_common_definition_t* p_entry_common_definition;
      
	  // multi row output?  loop all definitions ...
      do {
 	  
          // get current 'entry_definition' to deal with
          p_entry_common_definition = 
		      conn->p_entry_active_dir_matching_definition;
 
 	 	  // set definition entry that should be processed in next loop
		  conn->p_entry_active_dir_matching_definition =
		      STAILQ_NEXT(p_entry_common_definition, entries);
 	 
		  // check if current entry definition belongs to this 'module'
		  if ( p_entry_common_definition->module->provided == &ESP32_DeVICE_ProvidedByModule ) {

			  // loop readings stored in definition and get oldest tist
			  int32_t newest_feat_tist = 0;
			  int32_t cmp_tist;
			  entry_reading2_t *p_current_entry_reading;
			  STAILQ_FOREACH(p_current_entry_reading,
			      &p_entry_common_definition->head_readings2, entries) {

			      // get the 'timestamp' from this 'reading'
				  cmp_tist = p_current_entry_reading->reading.timestamp;

				  // update 'newest_feat_tist'
				  if ( newest_feat_tist < cmp_tist ) newest_feat_tist = cmp_tist;
			  }			  


			  // 'tist'-check, output row only if older than remote_tist, or no 'tist'= -1
			  if ( remote_tist == -1 ||
				  remote_tist < p_entry_common_definition->timestamp ||
                  remote_tist < newest_feat_tist) {



				  // 'tist'-check result: Output this ROW
				  len = sprintf(buff, 
					  "%s{\"Name\":\"%.*s\",\"TIST\":\"%d\"",
					  ( BWS & 1 << 4 ) ? "" : ",\n",  // "first row - no Komma" flag
					  p_entry_common_definition->nname.len,
					  p_entry_common_definition->nname.p_char,
					  (int) p_entry_common_definition->timestamp);


				  // delete "first row - no Komma" flag
				  BWS &= ~( 1 << 4 );

				  // send start of row
				  HTTPD_Send_To_Send_Buffer(conn, buff, len);


                  // continue row - loop readings stored in definition
			      entry_reading2_t *p_current_entry_reading;
			      STAILQ_FOREACH(p_current_entry_reading,
			          &p_entry_common_definition->head_readings2, entries) {

				      // get reading value as text	
				      string_t value_as_text = 
                          p_current_entry_reading->reading.p_reading_type->p_get_raw_reading_as_text_fn(&p_current_entry_reading->reading);				
					
					  // 'tist'-check result: Output this ROW
				      len = sprintf(buff, 
					      ",\"%.*s\":\"%.*s\"",
						  p_current_entry_reading->reading.name.len,
						  p_current_entry_reading->reading.name.p_char,
						  value_as_text.len,
						  value_as_text.p_char);

				      // send reading
				      HTTPD_Send_To_Send_Buffer(conn, buff, len);
				      
                      // free reading value as text
				      free(value_as_text.p_char);
			      }			  

				  // 'now finalize row
				  len = sprintf(buff, 
                       "}");

				  // send end of row - may return from cb if buffer is full!
				  if ( !HTTPD_Send_To_Send_Buffer (conn, buff, len) ) return HTTPD_CGI_PROCESS_CONN;
			  }
          }

		  // get prepared next definition for processing
		  p_entry_common_definition = conn->p_entry_active_dir_matching_definition;





/*
			// 'tist'-check, output row if older than remote_tist, or no 'tist' ...
			if ( remote_tist == -1 ||
				remote_tist < p_entry_ESP32_DeVICE_definition->RB01_X_TiSt ||
				remote_tist < p_entry_ESP32_DeVICE_definition->IB01_X_TiSt) {

				// 'tist'-check result: Output this ROW
				len = sprintf(buff, 
					"%s{\"Name\":\"%.*s\",\"TIST\":\"%d\",\"VAL\":\"%u\",\"MIN\":\"%u\","
					"\"MAX\":\"%u\",\"DIM\":\"%u\"}",
					( BWS & 1 << 4 ) ? "" : ",\n",  // "first row - no Komma" flag
					p_entry_ESP32_DeVICE_definition->common.nameLen,
					p_entry_ESP32_DeVICE_definition->common.name,
					(int) p_entry_ESP32_DeVICE_definition->RB01_X_TiSt,
					(uint) p_entry_ESP32_DeVICE_definition->RB01_Val,
					(uint) p_entry_ESP32_DeVICE_definition->IB01_SwValMin,
					(uint) p_entry_ESP32_DeVICE_definition->IB01_SwValMax,
					(100 * (p_entry_ESP32_DeVICE_definition->RB01_Val -
						p_entry_ESP32_DeVICE_definition->IB01_SwValMin) / 
					(p_entry_ESP32_DeVICE_definition->IB01_SwValMax - 
						p_entry_ESP32_DeVICE_definition->IB01_SwValMin+10) ) );

				// delete "first row - no Komma" flag
				BWS &= ~( 1 << 4 );

				// send row
				if ( !HTTPD_Send_To_Send_Buffer (conn, buff, len) ) return HTTPD_CGI_PROCESS_CONN;
			}
*/



	  // loop if pending entry_definition an of course an "multi row - modules definitions" flag
      } while (p_entry_common_definition && ( BWS & 1 << 30 ) );

	  BWS &= ~( 1 << 5 );
  }

//--------------------------------------------------------------------------------------------------

  // send "jso lead out" ?
  if ( BWS & 1 << 6 ) {

	  BWS &= ~( 1 << 6 );

	  len = sprintf (buff, "]\n}\n}");

	  if ( !HTTPD_Send_To_Send_Buffer (conn, buff, len) ) return HTTPD_CGI_PROCESS_CONN;
  }

//--------------------------------------------------------------------------------------------------

  return HTTPD_CGI_DISCONNECT_CONN;
}

 

