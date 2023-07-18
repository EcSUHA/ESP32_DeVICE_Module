idf_component_register(

    SRCS            "ESP32_DeVICE_Module.c"
    
		            "ESP32_DeVICE_Attribute.c"
		            "ESP32_DeVICE_Define.c"
		        #   "ESP32_DeVICE_Delete.c"
		        #   "ESP32_DeVICE_DirectRead.c"
		        #   "ESP32_DeVICE_DirectWrite.c"
		        #   "ESP32_DeVICE_Except.c"
		            "ESP32_DeVICE_Get.c"
			    #   "ESP32_DeVICE_Idle.c"	            
		            "ESP32_DeVICE_Initialize.c"
		            "ESP32_DeVICE_Notify.c"
			    #   "ESP32_DeVICE_Parse.c"		
			    #   "ESP32_DeVICE_Read.c"	
			    #   "ESP32_DeVICE_Ready.c"	            
		            "ESP32_DeVICE_Rename.c"
		            "ESP32_DeVICE_Set.c"
		            "ESP32_DeVICE_Shutdown.c"
		            "ESP32_DeVICE_State.c"
		            "ESP32_DeVICE_Undefine.c"
#     			    "ESP32_DeVICE_Write.c"
		                      
	                "ESP32_DeVICE_Webif_Get_Cb.c"
	                "ESP32_DeVICE_Webif_Set_Cb.c"
	                "ESP32_DeVICE_Webif_Jso_Cb.c"
	                
	                "ESP32_DeVICE_readings.c"
				    "HTools_cgi.c"
					"ServAPCfg_tpl.c"
   
    INCLUDE_DIRS    "include" 
                    "../../main/scde_core/include"
                    "../../main/include"
                    "../../main/scde_optional/modules/WebIf_Module/include"
)

