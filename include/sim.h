/*
 * Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 

#ifndef __TIZEN_TELEPHONY_SIM_H__
#define __TIZEN_TELEPHONY_SIM_H__


#include <tizen.h>


#ifdef __cplusplus
 extern "C" {
#endif

	 
/**
 * @file sim.h
 * @brief This file contains the SIM APIs and related enumeration.
 */

/**
 * @addtogroup CAPI_TELEPHONY_SIM_MODULE
 * @{
 */

 
/**
 * @brief Enumeration of error code for sim card.
 */
typedef enum
{
	SIM_ERROR_NONE			= TIZEN_ERROR_NONE,	/**< Successful */
	SIM_ERROR_OUT_OF_MEMORY		= TIZEN_ERROR_OUT_OF_MEMORY,		/**< Out of memory */		                
	SIM_ERROR_INVALID_PARAMETER	= TIZEN_ERROR_INVALID_PARAMETER,		/**< Invalid parameter */
	SIM_ERROR_OPERATION_FAILED	= TIZEN_ERROR_TELEPHONY_CLASS | 0x3000,	/**< Operation failed */
	SIM_ERROR_NOT_AVAILABLE		= TIZEN_ERROR_TELEPHONY_CLASS | 0x3001,	/**< SIM is not available */ 
} sim_error_e;

/**
 * @brief Enumeration of the state of SIM card.
 * 
 */
typedef enum
{
	SIM_STATE_UNAVAILABLE, /**< SIM is not available on this device */
	SIM_STATE_LOCKED,      /**< SIM is locked */
	SIM_STATE_AVAILABLE,	/**< SIM is available on this device */
	SIM_STATE_UNKNOWN,	/**< SIM is in transition between states */
} sim_state_e;	


/**
 * @brief Gets the Integrated Circuit Card IDentification (ICC-ID).
 * @details The Integrated Circuit Card Identification number internationally identifies SIM cards. 
 * @remarks @c icc_id must be released with free() by you.
 *
 * @param[out] icc_id The Integrated Circuit Card IDentification
 * @return 0 on success, otherwise a negative error value.
 * @retval #SIM_ERROR_NONE Successful
 * @retval #SIM_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #SIM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #SIM_ERROR_OPERATION_FAILED Operation failed
 * @retval #SIM_ERROR_NOT_AVAILABLE SIM is not available
 * @pre The SIM state must be #SIM_STATE_AVAILABLE.
 * @see	sim_get_state()
 *
 */
int sim_get_icc_id(char **icc_id);

/**
 * @brief Gets the Mobile Country Code (MCC) of SIM provider.
 * @details The Mobile Country Code is embedded in the SIM card.
 *
 * @remarks @c mcc must be released with free() by you.
 *
 * @param[out] mcc The Mobile Country Code
 * @return 0 on success, otherwise a negative error value.
 * @retval #SIM_ERROR_NONE Successful
 * @retval #SIM_ERROR_OUT_OF_MEMORY Out of memory 
 * @retval #SIM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #SIM_ERROR_OPERATION_FAILED Operation failed
 * @retval #SIM_ERROR_NOT_AVAILABLE SIM is not available
 * @pre The SIM state must be #SIM_STATE_AVAILABLE.
 * @see	sim_get_state()
 *
 */
int sim_get_mcc(char **mcc);

/**
 * @brief Gets the Mobile Network Code (MNC) of SIM provider.
 * @details This function gets Mobile Network Code embedded in SIM card.
 *
 * @remarks @c mnc must be released with free() by you.
 * @param[out] mnc The Mobile Network Code
 * @return 0 on success, otherwise a negative error value.
 * @retval #SIM_ERROR_NONE Successful
 * @retval #SIM_ERROR_OUT_OF_MEMORY Out of memory 
 * @retval #SIM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #SIM_ERROR_OPERATION_FAILED Operation failed
 * @retval #SIM_ERROR_NOT_AVAILABLE SIM is not available
 * @pre The SIM state must be #SIM_STATE_AVAILABLE.
 * @see	sim_get_state()
 *
 */
int sim_get_mnc(char **mnc);

/**
 * @brief Gets the Mobile Subscription Identification Number (MSIN) of SIM provider.
 * @details This function gets Mobile Subscription Identification Number embedded in SIM card.
 *
 * @remarks @c msin must be released with free() by you.
 * @param[out] msin The  Mobile Subscription Identification Number
 * @return 0 on success, otherwise a negative error value.
 * @retval #SIM_ERROR_NONE Successful
 * @retval #SIM_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #SIM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #SIM_ERROR_OPERATION_FAILED Operation failed
 * @retval #SIM_ERROR_NOT_AVAILABLE SIM is not available
 * @pre The SIM state must be #SIM_STATE_AVAILABLE.
 * @see	sim_get_state()
 *
 */
int sim_get_msin(char **msin);

/**
 * @brief Gets the Service Provider Name (SPN) of SIM card.
 * @details This function gets Service Provider Name embedded in SIM card.
 * If this value is not stored in SIM card, NULL will be returned.
 * @remarks @c spn must be released with free() by you.
 * @param[out] spn The Service Provider Name
 * @return 0 on success, otherwise a negative error value.
 * @retval #SIM_ERROR_NONE Successful
 * @retval #SIM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #SIM_ERROR_OPERATION_FAILED Operation failed
 * @retval #SIM_ERROR_NOT_AVAILABLE SIM is not available
 * @pre The SIM state must be #SIM_STATE_AVAILABLE.
 * @see	sim_get_state()
 *
 */
int sim_get_spn(char **spn);

/**
 * @brief Gets the Operator Name String (ONS) of Common PCN Handset Specification (CPHS) in SIM card.
 * @details This function gets the full name and the short name of CPHS operator embedded in SIM card.
 * If this value is not stored in SIM card, NULL will be returned.
 * @remarks @c full_name and @c short_name must be released with free() by you.
 * @param[out] full_name The full name of CPHS operator
 * @param[out] short_name The short name of CPHS operator
 * @return 0 on success, otherwise a negative error value.
 * @retval #SIM_ERROR_NONE Successful
 * @retval #SIM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #SIM_ERROR_OPERATION_FAILED Operation failed
 * @retval #SIM_ERROR_NOT_AVAILABLE SIM is not available
 * @pre The SIM state must be #SIM_STATE_AVAILABLE.
 * @see	sim_get_state()
 *
 */
int sim_get_cphs_operator_name(char** full_name, char** short_name);

/**
 * @brief Gets the state of SIM.
 *
 * @param[out] sim_state The current state of SIM
 * @return 0 on success, otherwise a negative error value.
 * @retval #SIM_ERROR_NONE Successful
 * @retval #SIM_ERROR_OPERATION_FAILED Operation failed
 * @retval #SIM_ERROR_INVALID_PARAMETER Invalid parameter
 *
 */
int sim_get_state(sim_state_e *sim_state);

/**
 * @brief Gets the SIM card subscriber number.
 * @details This function gets subscriber number embedded in SIM card. This value contains MSISDN related to the subscriber. \n
 * If this value is not stored in SIM card, NULL will be returned.
 *
 * @remarks @c subscriber_number must be released with free() by you.
 *
 * @param[out] subscriber_number The subscriber number in SIM
 * @return 0 on success, otherwise a negative error value.
 * @retval #SIM_ERROR_NONE Successful
 * @retval #SIM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #SIM_ERROR_OPERATION_FAILED Operation failed
 * @retval #SIM_ERROR_NOT_AVAILABLE SIM is not available
 * @pre The SIM state must be #SIM_STATE_AVAILABLE.
 * @see	sim_get_state()
 *
 */
int sim_get_subscriber_number(char **subscriber_number);

/**
 * @brief Called when sim card state changes.
 * @param [in] state The status of sim
 * @param [in] user_data The user data passed from the callback registration function
 * @pre This callback function is invoked if you register this function using sim_set_state_changed_cb(). 
 * 
 * @see sim_set_state_changed_cb()
 * @see sim_unset_state_changed_cb()
 */
typedef void(* sim_state_changed_cb)(sim_state_e state, void *user_data);

/**
 * @brief Registers a callback function to be invoked when sim card state changes. 
 *
 * @remarks You can register several callback functions.   
 *
 * @param [in] callback	The callback function to register
 * @param [in] user_data The user data to be passed to the callback function
 * @param [out] id A callback ID
 * @return 0 on success, otherwise a negative error value.
 * @retval #SIM_ERROR_NONE Successful
 * @retval #SIM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #SIM_ERROR_OPERATION_FAILED Operation failed
 * @post sim_state_changed_cb() will be invoked. 
 * @see sim_state_changed_cb()
 * @see	sim_unset_state_changed_cb()
 */
int sim_set_state_changed_cb(sim_state_changed_cb callback, void *user_data);

/**
 * @brief Unregisters the callback function.
 * 
 * @param [in] id The callback ID
 * @return 0 on success, otherwise a negative error value.
 * @retval #SIM_ERROR_NONE Successful
 * @retval #SIM_ERROR_OPERATION_FAILED Operation failed.  
 * @see	sim_set_state_changed_cb()
 *
 */
int sim_unset_state_changed_cb();

/**
 * @}
 */


#ifdef __cplusplus
 }
#endif


#endif // __TIZEN_TELEPHONY_SIM_H__

