/*
 * adc_impl.h
 *
 *  Created on: 2 mrt. 2017
 *      Author: jancu
 */

#ifndef ADC_IMPL_ADC_IMPL_H_
#define ADC_IMPL_ADC_IMPL_H_

uint16_t adcImplGetAdc(uint32_t uModule);

uint16_t adcImplToValue(uint16_t uRaw);
float adcImplToFloat(uint16_t uRaw);

#endif /* ADC_IMPL_ADC_IMPL_H_ */
