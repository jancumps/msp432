/*
 * eload_api.h
 *
 *  Created on: 24 dec. 2016
 *      Author: jancu
 */

#ifndef ELOAD_API_ELOAD_API_H_
#define ELOAD_API_ELOAD_API_H_

/*!
 *  @brief  Retrieve voltage sampled from the instrument terminals
 *
 *  This function returns the most recent sampled voltage on the instruments main terminals.
 *  Calling this function doesn't initiate a sample. There's a scheduled task that collects
 *  the info in the background.
 *
e.
 */

double eloadGetVoltageDC();


#endif /* ELOAD_API_ELOAD_API_H_ */
