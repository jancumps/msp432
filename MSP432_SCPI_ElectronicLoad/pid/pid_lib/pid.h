/*
 * pid.h
 *
 *
 *
 *
 ****************************************************************
* Arduino PID Library - Version 1.1.1
* by Brett Beauregard <br3ttb@gmail.com> brettbeauregard.com
*
* This Library is licensed under a GPLv3 License
***************************************************************

 - For an ultra-detailed explanation of why the code is the way it is, please visit:
   http://brettbeauregard.com/blog/2011/04/improving-the-beginners-pid-introduction/

 - For function documentation see:  http://playground.arduino.cc/Code/PIDLibrary
 *
 *  Created on: 13 jan. 2017
 *      Author: jancu
 */




#ifndef PID_PID_LIB_PID_H_
#define PID_PID_LIB_PID_H_

#include <stdbool.h>

#define PID_LIBRARY_VERSION 1.1.1

  //Constants used in some of the functions below
  #define PID_AUTOMATIC 1
  #define PID_MANUAL    0
  #define PID_DIRECT  0
  #define PID_REVERSE  1


  //commonly used functions **************************************************************************
    void pidInit(double*, double*, double*,        // * constructor.  links the PID to the Input, Output, and
        double, double, double, int);     //   Setpoint.  Initial tuning parameters are also set here

    void pidSetMode(int Mode);               // * sets PID to either Manual (0) or Auto (non-0)

    bool pidCompute();                       // * performs the PID calculation.  it should be
                                          //   called every time loop() cycles. ON/OFF and
                                          //   calculation frequency can be set using SetMode
                                          //   SetSampleTime respectively

    void pidSetOutputLimits(double, double); //clamps the output to a specific range. 0-255 by default, but
                                          //it's likely the user will want to change this depending on
                                          //the application



  //available but not commonly used functions ********************************************************
    void pidSetTunings(double, double,       // * While most users will set the tunings once in the
                    double);              //   constructor, this function gives the user the option
                                          //   of changing tunings during runtime for Adaptive control
    void pidSetControllerDirection(int);     // * Sets the Direction, or "Action" of the controller. DIRECT
                                          //   means the output will increase when error is positive. REVERSE
                                          //   means the opposite.  it's very unlikely that this will be needed
                                          //   once it is set in the constructor.
    void pidSetSampleTime(int);              // * sets the frequency, in Milliseconds, with which
                                          //   the PID calculation is performed.  default is 100

    //Display functions ****************************************************************
      double pidGetKp();                       // These functions query the pid for interal values.
      double pidGetKi();                       //  they were created mainly for the pid front-end,
      double pidGetKd();                       // where it's important to know what is actually
      int pidGetMode();                        //  inside the PID.
      int pidGetDirection();                   //



#endif /* PID_PID_LIB_PID_H_ */
