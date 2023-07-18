#ifndef ServAPCfg_tpl_H
#define ServAPCfg_tpl_H



extern SelectAData DisEn[];
extern SelectAData MaxConn[];
extern SelectAData Channel[];
extern SelectAData Auth_M[];
extern SelectAData SSID_H[];
extern SelectAData NrAP[];
extern SelectAData PhMode[];


void ServAPCfg_tpl(WebIf_HTTPDConnSlotData_t *connData, char *token, void **arg);


#endif /* ServAPCfg_tpl_H */
