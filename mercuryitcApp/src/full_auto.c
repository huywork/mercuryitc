#include <registryFunction.h>
#include <epicsExport.h>
#include <aSubRecord.h>
#include <stdlib.h>
#include <stdio.h>
#include <menuFtype.h>
#include <errlog.h>
#include <math.h>
#include <alarm.h>

/**
 * Settings for full auto mode using SPC
 */
struct Settings {
    double deadband;  // temperature deadband 
    double minimumPressure;  // minimum allowed pressure
    double maximumPressure;  // maximum allowed pressure
    double offset;  // offset in pressure to ramp up to once the temperature is stable
    double offsetDuration;  // time over which to ramp to the pressure in minutes
    double gainPressure;  // gain of pressure per delta in temperature
};

/**
  * Return a value, if it is outside the range return the nearest limit;
  * Args:
  *   value: value to coerce
  *   lim1: limit 1
  *   lim2: limit2
  * Return:
  *    value between lim1 and lim2
  */
double coerce(double value, double lim1, double lim2) {
    double min;
    double max;
    if (lim1 < lim2) {
        min = lim1;
        max = lim2;
    } else {
        min = lim2;
        max = lim1;
    }
    
    if (value < min) {
        value = min;
    } else if (value > max) {
        value = max;
    }
    
    return value;
}

/**
 *  Calculate the ramp pressure based so it ramps from offset down to 0 in duration minutes
 *  Args:
 *     settings: auto flow settings
 *     startRampTime: of the ramp in seconds
 *     currentTime: in seconds
 *  Returns:
 *      ramp pressure
 */
double pressureRamp(struct Settings settings, double startRampTime, double currentTime) {
    double rampPressure;
    double durationInSeconds = settings.offsetDuration * 60.0;
    
    // pro-rata amount of pressure to add based on the time since the ramp started
    rampPressure = (1.0 - (currentTime - startRampTime) / durationInSeconds) * settings.offset;
    
    return coerce(rampPressure, 0, settings.offset);

}

/**
  * Get pressure for temperatures above the deadband, excluding offset ramp
  *
  * Args:
  *  settings: settings for full auto
  *  temp: current temp
  *  tempSP: set point
  * Returns:
  *  pressure
  */
double pressureAboveDeadband(struct Settings settings, double temp, double tempSP, double pressureFromTable) {
    double gainPressure;
    double pressureBeforeRamp;
    
    gainPressure = fabs(temp-tempSP - settings.deadband) * settings.gainPressure;
    gainPressure = pow(gainPressure, 2);
    pressureBeforeRamp = pressureFromTable + gainPressure;

    return coerce(pressureBeforeRamp, pressureFromTable, settings.maximumPressure);
}

/** 
 * Get the pressure for the given temperature and set point.
 *
 * Args:
 *  settings: the settings
 *  temp: curent temperature
 *  tempSP: temperature setpoint
 *  startRampTime: time last ramp started, i.e. last time temperature was outside the deadband
 *  currentTime: current ime
 *  pressure: return the pressure that the system should be atan
 * Retruns:
 *   0 for sucess; otherwise error coded
 */
long pressureForTemp(struct Settings settings, double temp, double tempSP, double startRampTime, double currentTime, double pressureFromTable, double* pressure){
    double finalPressure;
    double deadband = settings.deadband;
    
    double deltaTemp = temp - tempSP;
    if ( deltaTemp <= -2*deadband) {  
        // a long way under the deadband, set at minimum pressure so temp will rise
        finalPressure = settings.minimumPressure;
    } else if (deltaTemp <= -deadband){
        // below the deadband set at base pressure so temp will rise
        finalPressure = pressureFromTable;
    } else if (deltaTemp <= deadband){
        // below the deadband set at base pressure so temp will rise
        finalPressure = pressureFromTable + pressureRamp(settings, startRampTime, currentTime);
    } else {
        finalPressure = pressureAboveDeadband(settings, temp, tempSP, pressureFromTable) + pressureRamp(settings, startRampTime, currentTime);
    }
    
    *pressure = coerce(finalPressure, settings.minimumPressure, settings.maximumPressure);
    return 0;
}

/**
  * Get the value of a field from the record ensuring it is of the correct type
  *
  * Args:
  *     prec: pointer to the record
  *     inpLetter: letter for the input link
  *     value: value of the input link
  *     type: type of input link
  *     output_value: point to where value should be set
  *  Returns: 0 if ok; 1 otherwise
  */
long getFieldFromRecord(aSubRecord *prec, char inpLetter, void* value, epicsEnum16 type, double * output_value){
    if (type != menuFtypeDOUBLE)
    {
        errlogSevPrintf(errlogMajor, "%s incorrect input argument %c type should be DOUBLE", prec->name, inpLetter);
        return 1;
    }
    *output_value = *(double *)value;
    return 0;
}

/**
  * Check that a severity field is not invalid and of the right type
  *
  * Args:
  *     prec: pointer to the record
  *     inpLetter: letter for the input link
  *     value: value of the input link
  *     type: type of input link
  *  Returns: 0 if ok and not in alarm; 1 otherwise
  */
long notInvalidAlarm(aSubRecord *prec, char inpLetter, void* value, epicsEnum16 type){
    if (type != menuFtypeENUM)
    {
        errlogSevPrintf(errlogMajor, "%s incorrect input argument %c type should be ENUM", prec->name, inpLetter);
        return 1;
    }
    if (*(epicsEnum16  *)value == INVALID_ALARM) {
         errlogSevPrintf(errlogMajor, "%s link %c is in alarm, not processing record", prec->name, inpLetter);
         return 1;
    }
    return 0;
}

/**
  Routine for asub record to call
  Args:
    pres: pointer to record
  Returns:
    0 sucess; error otherwise
*/
static long calcPressure(aSubRecord *prec) {

    double temp;
    double tempSP;
    struct Settings settings;
    long success;
    double pressure;
    double currentTime;
    double startRampTime;
    double pressureFromTable;

    
    if(getFieldFromRecord(prec, 'A', prec->a, prec->fta, &temp) != 0 ||
        getFieldFromRecord(prec, 'B', prec->b, prec->ftb, &tempSP) != 0 ||
        getFieldFromRecord(prec, 'C', prec->c, prec->ftc, &settings.deadband) != 0 ||
        getFieldFromRecord(prec, 'D', prec->d, prec->ftd, &settings.minimumPressure) != 0 ||
        getFieldFromRecord(prec, 'E', prec->e, prec->fte, &settings.maximumPressure) != 0 ||
        getFieldFromRecord(prec, 'F', prec->f, prec->ftf, &settings.offset) != 0 ||
        getFieldFromRecord(prec, 'G', prec->g, prec->ftg, &settings.offsetDuration) != 0 ||
        getFieldFromRecord(prec, 'H', prec->h, prec->fth, &currentTime) != 0 ||
        getFieldFromRecord(prec, 'I', prec->i, prec->fti, &startRampTime) != 0 ||
        getFieldFromRecord(prec, 'J', prec->j, prec->ftj, &settings.gainPressure) != 0 ||
        getFieldFromRecord(prec, 'K', prec->k, prec->ftk, &pressureFromTable) != 0 ||
        notInvalidAlarm(prec, 'L', prec->l, prec->ftl) != 0 ||
        notInvalidAlarm(prec, 'M', prec->m, prec->ftm) != 0 ||
        notInvalidAlarm(prec, 'N', prec->n, prec->ftn) != 0) {
        return 1;
    }        
    
    // reset ramp if temperature is outside the deadband
    if (fabs(temp - tempSP) > settings.deadband) {
        startRampTime = currentTime;
    }
    
    success = pressureForTemp(settings, temp, tempSP, startRampTime, currentTime, pressureFromTable, &pressure);
    
    if (success == 0) {
        *(double*)prec->vala = pressure;
        *(double*)prec->valb = startRampTime;
    }
    
    return success;
}

epicsRegisterFunction(calcPressure);
