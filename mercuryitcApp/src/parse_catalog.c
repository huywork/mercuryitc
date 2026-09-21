#include <registryFunction.h>
#include <epicsExport.h>
#include <aSubRecord.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <menuFtype.h>
#include <errlog.h>
#include <epicsString.h>

#define MAX_DEVICES 64u

static long parse_catalog(aSubRecord *prec) {

	char* input = (char*)(prec->a);
    
    epicsOldString temp_devices[MAX_DEVICES];
    epicsUInt32 assigned_temp_devices = 0;
    epicsOldString pres_devices[MAX_DEVICES];
    epicsUInt32 assigned_pres_devices = 0;
    epicsOldString aux_devices[MAX_DEVICES];
    epicsUInt32 assigned_aux_devices = 0;
    epicsOldString htr_devices[MAX_DEVICES];
    epicsUInt32 assigned_htr_devices = 0;
    epicsOldString lvl_devices[MAX_DEVICES];
    epicsUInt32 assigned_lvl_devices = 0;
    
    char* strtok_saveptr = NULL;
    char* location;
    char* type;
    char* dev_string;
    
    unsigned i;
    
    dev_string = epicsStrtok_r(input, ":", &strtok_saveptr);
    
    while(dev_string) {
            
        if(strcmp(dev_string, "DEV") != 0) {
            errlogSevPrintf(errlogMajor, "parse_catalog: corrupted message, invalid DEV header");
            return 1;
        }
        
        location = epicsStrtok_r(NULL, ":", &strtok_saveptr);
        
        if (location == NULL) {
            errlogSevPrintf(errlogMajor, "parse_catalog: corrupted message, location is NULL");
            return 1;
        }
        
        type = epicsStrtok_r(NULL, ":", &strtok_saveptr);
        
        if (type == NULL) {
            errlogSevPrintf(errlogMajor, "parse_catalog: corrupted message, type is NULL");
            return 1;
        }
        
        if (strcmp(type, "PRES") == 0) {
            strncpy(pres_devices[assigned_pres_devices++], location, MAX_STRING_SIZE);
        } 
        else if (strcmp(type, "TEMP") == 0) {
            strncpy(temp_devices[assigned_temp_devices++], location, MAX_STRING_SIZE);
        }
        else if (strcmp(type, "AUX") == 0) {
            strncpy(aux_devices[assigned_aux_devices++], location, MAX_STRING_SIZE);
        }
        else if (strcmp(type, "HTR") == 0) {
            strncpy(htr_devices[assigned_htr_devices++], location, MAX_STRING_SIZE);
        }
        else if (strcmp(type, "LVL") == 0) {
            strncpy(lvl_devices[assigned_lvl_devices++], location, MAX_STRING_SIZE);
        } 
        else {
            errlogSevPrintf(errlogMajor, "parse_catalog: unknown device type");
            return 1;
        }
        
        if (assigned_temp_devices >= MAX_DEVICES 
            || assigned_pres_devices >= MAX_DEVICES 
            || assigned_aux_devices >= MAX_DEVICES 
            || assigned_htr_devices >= MAX_DEVICES 
            || assigned_lvl_devices >= MAX_DEVICES) 
        {
            errlogSevPrintf(errlogMajor, "parse_catalog: too many devices");
            return 1;
        }
        
        dev_string = epicsStrtok_r(NULL, ":", &strtok_saveptr);
    }
    
    for (i = 0; i < assigned_temp_devices; i++) {
        strncpy(((epicsOldString*)(prec->vala))[i], temp_devices[i], MAX_STRING_SIZE);
    }
    prec->neva = assigned_temp_devices;
    
    for (i = 0; i < assigned_pres_devices; i++) {
        strncpy(((epicsOldString*)(prec->valb))[i], pres_devices[i], MAX_STRING_SIZE);
    }
    prec->nevb = assigned_pres_devices;
    
    for (i = 0; i < assigned_lvl_devices; i++) {
        strncpy(((epicsOldString*)(prec->valc))[i], lvl_devices[i], MAX_STRING_SIZE);
    }
    prec->nevc = assigned_lvl_devices;
    
    for (i = 0; i < assigned_htr_devices; i++) {
        strncpy(((epicsOldString*)(prec->vald))[i], htr_devices[i], MAX_STRING_SIZE);
    }
    prec->nevd = assigned_htr_devices;
    
    for (i = 0; i < assigned_aux_devices; i++) {
        strncpy(((epicsOldString*)(prec->vale))[i], aux_devices[i], MAX_STRING_SIZE);
    }
    prec->neve = assigned_aux_devices;
	
    return 0;
}

epicsRegisterFunction(parse_catalog);
