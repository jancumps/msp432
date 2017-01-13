/*
 * pid.c
 *

    **********************************************************************************************
     * Arduino PID Library - Version 1.1.1
     * by Brett Beauregard <br3ttb@gmail.com> brettbeauregard.com
     *
     * This Library is licensed under a GPLv3 License
     **********************************************************************************************
 *  Created on: 13 jan. 2017
 *      Author: jancu
 */


#include "pid.h"


#include <xdc/runtime/Timestamp.h>

    void pidInitialize();
    long millis();

    double dispKp;              // * we'll hold on to the tuning parameters in user-entered
    double dispKi;              //   format for display purposes
    double dispKd;              //

    double kp;                  // * (P)roportional Tuning Parameter
    double ki;                  // * (I)ntegral Tuning Parameter
    double kd;                  // * (D)erivative Tuning Parameter

    int controllerDirection;

    double *myInput;              // * Pointers to the Input, Output, and Setpoint variables
    double *myOutput;             //   This creates a hard link between the variables and the
    double *mySetpoint;           //   PID, freeing the user from having to constantly tell us
                                  //   what these values are.  with pointers we'll just know.

    unsigned long lastTime;
    double ITerm, lastInput;

    unsigned long SampleTime;
    double outMin, outMax;
    bool inAuto;





    /*Constructor (...)*********************************************************
     *    The parameters specified here are those for for which we can't set up
     *    reliable defaults, so we need to have the user set them.
     ***************************************************************************/
    void pidInit(double* Input, double* Output, double* Setpoint,
            double Kp, double Ki, double Kd, int ControllerDirection)
    {

        myOutput = Output;
        myInput = Input;
        mySetpoint = Setpoint;
        inAuto = false;

        pidSetOutputLimits(0, 255);               //default output limit corresponds to
                                                    //the arduino pwm limits

        SampleTime = 100;                           //default Controller Sample Time is 0.1 seconds

        pidSetControllerDirection(ControllerDirection);
        pidSetTunings(Kp, Ki, Kd);

        lastTime = millis()-SampleTime;
    }


    /* Compute() **********************************************************************
     *     This, as they say, is where the magic happens.  this function should be called
     *   every time "void loop()" executes.  the function will decide for itself whether a new
     *   pid Output needs to be computed.  returns true when the output is computed,
     *   false when nothing has been done.
     **********************************************************************************/
    bool pidCompute()
    {
       if(!inAuto) return false;
       unsigned long now = millis();
       unsigned long timeChange = (now - lastTime);
       if(timeChange>=SampleTime)
       {
          /*Compute all the working error variables*/
          double input = *myInput;
          double error = *mySetpoint - input;
          ITerm+= (ki * error);
          if(ITerm > outMax) ITerm= outMax;
          else if(ITerm < outMin) ITerm= outMin;
          double dInput = (input - lastInput);

          /*Compute PID Output*/
          double output = kp * error + ITerm- kd * dInput;

          if(output > outMax) output = outMax;
          else if(output < outMin) output = outMin;
          *myOutput = output;

          /*Remember some variables for next time*/
          lastInput = input;
          lastTime = now;
          return true;
       }
       else return false;
    }


    /* SetTunings(...)*************************************************************
     * This function allows the controller's dynamic performance to be adjusted.
     * it's called automatically from the constructor, but tunings can also
     * be adjusted on the fly during normal operation
     ******************************************************************************/
    void pidSetTunings(double Kp, double Ki, double Kd)
    {
       if (Kp<0 || Ki<0 || Kd<0) return;

       dispKp = Kp; dispKi = Ki; dispKd = Kd;

       double SampleTimeInSec = ((double)SampleTime)/1000;
       kp = Kp;
       ki = Ki * SampleTimeInSec;
       kd = Kd / SampleTimeInSec;

      if(controllerDirection ==PID_REVERSE)
       {
          kp = (0 - kp);
          ki = (0 - ki);
          kd = (0 - kd);
       }
    }

    /* SetSampleTime(...) *********************************************************
     * sets the period, in Milliseconds, at which the calculation is performed
     ******************************************************************************/
    void pidSetSampleTime(int NewSampleTime)
    {
       if (NewSampleTime > 0)
       {
          double ratio  = (double)NewSampleTime
                          / (double)SampleTime;
          ki *= ratio;
          kd /= ratio;
          SampleTime = (unsigned long)NewSampleTime;
       }
    }

    /* SetOutputLimits(...)****************************************************
     *     This function will be used far more often than SetInputLimits.  while
     *  the input to the controller will generally be in the 0-1023 range (which is
     *  the default already,)  the output will be a little different.  maybe they'll
     *  be doing a time window and will need 0-8000 or something.  or maybe they'll
     *  want to clamp it from 0-125.  who knows.  at any rate, that can all be done
     *  here.
     **************************************************************************/
    void pidSetOutputLimits(double Min, double Max)
    {
       if(Min >= Max) return;
       outMin = Min;
       outMax = Max;

       if(inAuto)
       {
           if(*myOutput > outMax) *myOutput = outMax;
           else if(*myOutput < outMin) *myOutput = outMin;

           if(ITerm > outMax) ITerm= outMax;
           else if(ITerm < outMin) ITerm= outMin;
       }
    }

    /* SetMode(...)****************************************************************
     * Allows the controller Mode to be set to manual (0) or Automatic (non-zero)
     * when the transition from manual to auto occurs, the controller is
     * automatically initialized
     ******************************************************************************/
    void pidSetMode(int Mode)
    {
        bool newAuto = (Mode == PID_AUTOMATIC);
        if(newAuto == !inAuto)
        {  /*we just went from manual to auto*/
            pidInitialize();
        }
        inAuto = newAuto;
    }

    /* Initialize()****************************************************************
     *  does all the things that need to happen to ensure a bumpless transfer
     *  from manual to automatic mode.
     ******************************************************************************/
    void pidInitialize()
    {
       ITerm = *myOutput;
       lastInput = *myInput;
       if(ITerm > outMax) ITerm = outMax;
       else if(ITerm < outMin) ITerm = outMin;
    }

    /* SetControllerDirection(...)*************************************************
     * The PID will either be connected to a DIRECT acting process (+Output leads
     * to +Input) or a REVERSE acting process(+Output leads to -Input.)  we need to
     * know which one, because otherwise we may increase the output when we should
     * be decreasing.  This is called from the constructor.
     ******************************************************************************/
    void pidSetControllerDirection(int Direction)
    {
       if(inAuto && Direction !=controllerDirection)
       {
          kp = (0 - kp);
          ki = (0 - ki);
          kd = (0 - kd);
       }
       controllerDirection = Direction;
    }

    /* Status Funcions*************************************************************
     * Just because you set the Kp=-1 doesn't mean it actually happened.  these
     * functions query the internal state of the PID.  they're here for display
     * purposes.  this are the functions the PID Front-end uses for example
     ******************************************************************************/
    double pidetKp(){ return  dispKp; }
    double pidGetKi(){ return  dispKi;}
    double pidGetKd(){ return  dispKd;}
    int pidGetMode(){ return  inAuto ? PID_AUTOMATIC : PID_MANUAL;}
    int pidGetDirection(){ return controllerDirection;}

    long millis() {
        long t = Timestamp_get32();
//        long seconds = t >> 15;
        long msecs = (t & 0x7fff) * 1000 /32768;
        return msecs;
    }

