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


#include <sim.h>
#include <TapiCommon.h>
#include <ITapiSim.h>
#include <vconf.h>
#include <vconf-keys.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dlog.h>
#include <glib.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "TIZEN_N_SIM"

typedef struct sim_cb_data
{
	sim_state_e previous_state;
	const void* cb;
	void* user_data;
} sim_cb_data;

// Whether vconf_notifiy_key_chnaged is registered or not
static bool init_is_registered = false;
static bool chv_is_registered = false;
static bool slot_is_registered = false;

// Callback function data
static sim_cb_data sim_state_cb = {SIM_STATE_UNKNOWN, NULL, NULL};

// Callback function adapter
static void __sim_state_changed_cb_adapter(keynode_t *node, void* user_data);

// Internal Macros
#define SIM_CHECK_INPUT_PARAMETER(arg) \
	if( arg == NULL ) \
	{ \
		LOGE("[%s] INVALID_PARAMETER(0x%08x)", __FUNCTION__, SIM_ERROR_INVALID_PARAMETER); \
		return SIM_ERROR_INVALID_PARAMETER; \
	}

#define SIM_INIT() \
	if( tel_init() != TAPI_API_SUCCESS ) \
	{ \
		LOGE("[%s] OPERATION_FAILED(0x%08x)", __FUNCTION__, SIM_ERROR_OPERATION_FAILED); \
		return SIM_ERROR_OPERATION_FAILED; \
	}


int sim_get_icc_id(char** icc_id)
{
	TelSimIccIdInfo_t icc_data;
	int error_code = SIM_ERROR_NONE;
	int card_changed = 0;
	TelSimCardStatus_t sim_card_state = 0x00;

	SIM_CHECK_INPUT_PARAMETER(icc_id);
	SIM_INIT();
	
	if( tel_get_sim_init_info(&sim_card_state, &card_changed) != 0 
		|| sim_card_state != TAPI_SIM_STATUS_SIM_INIT_COMPLETED )
	{
		LOGE("[%s] NOT_AVAILABLE(0x%08x)", __FUNCTION__, SIM_ERROR_NOT_AVAILABLE);		
		error_code = SIM_ERROR_NOT_AVAILABLE;	
	}
	
	else
	{
		if( tel_get_sim_iccid(&icc_data) != 0 )
		{
			LOGE("[%s] OPERATION_FAILED(0x%08x)", __FUNCTION__, SIM_ERROR_OPERATION_FAILED);		
			error_code = SIM_ERROR_OPERATION_FAILED;	
		}
		else
		{
			*icc_id = strndup(icc_data.icc_num, icc_data.icc_length);
			if( *icc_id == NULL )
			{
				LOGE("[%s] OUT_OF_MEMORY(0x%08x)", __FUNCTION__, SIM_ERROR_OUT_OF_MEMORY);
				error_code = SIM_ERROR_OUT_OF_MEMORY; 
			}			
		}
	}

	tel_deinit();
	return error_code;
}


int sim_get_mcc(char** mcc)
{
	TelSimImsiInfo_t sim_imsi_info;
	int error_code = SIM_ERROR_NONE;
	int card_changed = 0;
	TelSimCardStatus_t sim_card_state = 0x00;

	SIM_CHECK_INPUT_PARAMETER(mcc);
	SIM_INIT();
	
	if( tel_get_sim_init_info(&sim_card_state, &card_changed) != 0 
		|| sim_card_state != TAPI_SIM_STATUS_SIM_INIT_COMPLETED )
	{
		LOGE("[%s] NOT_AVAILABLE(0x%08x)", __FUNCTION__, SIM_ERROR_NOT_AVAILABLE);		
		error_code = SIM_ERROR_NOT_AVAILABLE;	
	}
	else
	{
		if( tel_get_sim_imsi(&sim_imsi_info) != 0 )
		{
			LOGE("[%s] OPERATION_FAILED(0x%08x)", __FUNCTION__, SIM_ERROR_OPERATION_FAILED);		
			error_code = SIM_ERROR_OPERATION_FAILED;	
		}
		else
		{
			*mcc = (char*)malloc(sizeof(char) * (TAPI_SIM_MCC_CODE_LEN+1));
			if( *mcc == NULL )
			{
				LOGE("[%s] OUT_OF_MEMORY(0x%08x)", __FUNCTION__, SIM_ERROR_OUT_OF_MEMORY);
				error_code = SIM_ERROR_OUT_OF_MEMORY;                                                        
			}
			else
			{
				strncpy(*mcc, sim_imsi_info.szMcc, TAPI_SIM_MCC_CODE_LEN+1);				
			}
		}
	}

	tel_deinit();
	return error_code;
}


int sim_get_mnc(char** mnc)
{
	TelSimImsiInfo_t sim_imsi_info;
	int error_code = SIM_ERROR_NONE;
	int card_changed = 0;
	TelSimCardStatus_t sim_card_state = 0x00;	

	SIM_CHECK_INPUT_PARAMETER(mnc);
	SIM_INIT();

	if( tel_get_sim_init_info(&sim_card_state, &card_changed) != 0 
		|| sim_card_state != TAPI_SIM_STATUS_SIM_INIT_COMPLETED )
	{
		LOGE("[%s] NOT_AVAILABLE(0x%08x)", __FUNCTION__, SIM_ERROR_NOT_AVAILABLE);		
		error_code = SIM_ERROR_NOT_AVAILABLE;	
	}
	else
	{
		if( tel_get_sim_imsi(&sim_imsi_info) != 0 )
		{
			LOGE("[%s] OPERATION_FAILED(0x%08x)", __FUNCTION__, SIM_ERROR_OPERATION_FAILED);		
			error_code = SIM_ERROR_OPERATION_FAILED;	
		}
		else
		{
			*mnc = (char*)malloc(sizeof(char) * (TAPI_SIM_MNC_CODE_LEN+1));
			if( *mnc == NULL )
			{
				LOGE("[%s] OUT_OF_MEMORY(0x%08x)", __FUNCTION__, SIM_ERROR_OUT_OF_MEMORY);
				error_code = SIM_ERROR_OUT_OF_MEMORY;
			}
			else
			{
				strncpy(*mnc, sim_imsi_info.szMnc, TAPI_SIM_MNC_CODE_LEN+1);
			}
		}
	}

	tel_deinit();
	return SIM_ERROR_NONE;
}


int sim_get_msin(char** msin)
{
	TelSimImsiInfo_t sim_imsi_info;
	int error_code = SIM_ERROR_NONE;
	int card_changed = 0;
	TelSimCardStatus_t sim_card_state = 0x00;

	SIM_CHECK_INPUT_PARAMETER(msin);
	SIM_INIT();

	if( tel_get_sim_init_info(&sim_card_state, &card_changed) != 0
		|| sim_card_state != TAPI_SIM_STATUS_SIM_INIT_COMPLETED )
	{
		LOGE("[%s] NOT_AVAILABLE(0x%08x)", __FUNCTION__, SIM_ERROR_NOT_AVAILABLE);
		error_code = SIM_ERROR_NOT_AVAILABLE;
	}
	else
	{
		if( tel_get_sim_imsi(&sim_imsi_info) != 0 )
		{
			LOGE("[%s] OPERATION_FAILED(0x%08x)", __FUNCTION__, SIM_ERROR_OPERATION_FAILED);
			error_code = SIM_ERROR_OPERATION_FAILED;
		}
		else
		{
			*msin = (char*)malloc(sizeof(char) * (TAPI_SIM_MSIN_CODE_LEN+1));
			if( *msin == NULL )
			{
				LOGE("[%s] OUT_OF_MEMORY(0x%08x)", __FUNCTION__, SIM_ERROR_OUT_OF_MEMORY);
				error_code = SIM_ERROR_OUT_OF_MEMORY;
			}
			else
			{
				strncpy(*msin, sim_imsi_info.szMsin, TAPI_SIM_MSIN_CODE_LEN+1);
			}
		}
	}

	tel_deinit();
	return error_code;
}


int sim_get_spn(char** spn)
{
	int error_code = SIM_ERROR_NONE;
	int card_changed = 0;
	TelSimCardStatus_t sim_card_state = 0x00;	
	char* service_provider_name = NULL;

	SIM_CHECK_INPUT_PARAMETER(spn);
	SIM_INIT();

	error_code = tel_get_sim_init_info(&sim_card_state, &card_changed);
	if( error_code != 0 || sim_card_state != TAPI_SIM_STATUS_SIM_INIT_COMPLETED )
	{
		LOGE("[%s] NOT_AVAILABLE(0x%08x)", __FUNCTION__, SIM_ERROR_NOT_AVAILABLE);		
		error_code = SIM_ERROR_NOT_AVAILABLE;	
	}
	else
	{
		service_provider_name = vconf_get_str(VCONFKEY_TELEPHONY_SPN_NAME);
		if( service_provider_name == NULL )
		{
			LOGE("[%s] OPERATION_FAILED(0x%08x)", __FUNCTION__, SIM_ERROR_OPERATION_FAILED);
			error_code = SIM_ERROR_OPERATION_FAILED;
		}
		else if( strlen(service_provider_name) == 0 )
		{
			LOGI("[%s] spn has no value", __FUNCTION__);
			free(service_provider_name);
			*spn = NULL;
		}
		else
		{
			*spn = service_provider_name;
		}
	}

	tel_deinit();
	return error_code;
}


int sim_get_cphs_operator_name(char** full_name, char** short_name)
{
	TelSimCphsLocalInfo_t cphs_info;
	int error_code = SIM_ERROR_NONE;
	int card_changed = 0;
	TelSimCardStatus_t sim_card_state = 0x00;

	SIM_CHECK_INPUT_PARAMETER(full_name);
	SIM_CHECK_INPUT_PARAMETER(short_name);
	SIM_INIT();

	if( tel_get_sim_init_info(&sim_card_state, &card_changed) != 0
		|| sim_card_state != TAPI_SIM_STATUS_SIM_INIT_COMPLETED )
	{
		LOGE("[%s] NOT_AVAILABLE(0x%08x)", __FUNCTION__, SIM_ERROR_NOT_AVAILABLE);
		error_code = SIM_ERROR_NOT_AVAILABLE;
	}
	else
	{
		if( tel_get_sim_cphs_info(&cphs_info) != 0 )
		{
			LOGE("[%s] OPERATION_FAILED(0x%08x)", __FUNCTION__, SIM_ERROR_OPERATION_FAILED);
			error_code = SIM_ERROR_OPERATION_FAILED;
		}
		else
		{
			if(cphs_info.opname.NameLength)
			{
				*full_name = strndup((const char*)cphs_info.opname.OperatorName, cphs_info.opname.NameLength);
				if(*full_name == NULL)
				{
					LOGE("[%s] OUT_OF_MEMORY(0x%08x)", __FUNCTION__, SIM_ERROR_OUT_OF_MEMORY);
					error_code = SIM_ERROR_OUT_OF_MEMORY;
				}
			}
			else
			{
				*full_name = NULL;
			}

			if(cphs_info.opshortform.ShortNameLength)
			{
				*short_name = strndup((const char*)cphs_info.opshortform.OperatorShortName, cphs_info.opshortform.ShortNameLength);
				if(*short_name == NULL)
				{
					LOGE("[%s] OUT_OF_MEMORY(0x%08x)", __FUNCTION__, SIM_ERROR_OUT_OF_MEMORY);
					error_code = SIM_ERROR_OUT_OF_MEMORY;
				}
			}
			else
			{
				*short_name = NULL;
			}
		}
	}

	tel_deinit();
	return error_code;
}

int sim_get_state(sim_state_e* sim_state)
{
	int card_changed = 0;
	TelSimCardStatus_t sim_card_state = 0x00;
	int error_code = SIM_ERROR_NONE;

	SIM_CHECK_INPUT_PARAMETER(sim_state);
	SIM_INIT();

	if( tel_get_sim_init_info(&sim_card_state, &card_changed) != 0 )
	{
		LOGE("[%s] OPERATION_FAILED(0x%08x)", __FUNCTION__, SIM_ERROR_OPERATION_FAILED);		
		error_code = SIM_ERROR_OPERATION_FAILED;
	}
	else
	{
		switch(sim_card_state)
		{
			case TAPI_SIM_STATUS_CARD_ERROR:
				*sim_state = SIM_STATE_UNAVAILABLE;
				break;
			case TAPI_SIM_STATUS_CARD_NOT_PRESENT:
				*sim_state = SIM_STATE_UNAVAILABLE;
				break;
			case TAPI_SIM_STATUS_SIM_INITIALIZING:
				*sim_state = SIM_STATE_UNKNOWN;
				break;
			case TAPI_SIM_STATUS_SIM_INIT_COMPLETED:
				*sim_state = SIM_STATE_AVAILABLE;
				break;
			case TAPI_SIM_STATUS_SIM_PIN_REQUIRED:
				*sim_state = SIM_STATE_LOCKED;
				break;
			case TAPI_SIM_STATUS_SIM_PUK_REQUIRED:
				*sim_state = SIM_STATE_LOCKED;
				break;				
			case TAPI_SIM_STATUS_CARD_BLOCKED:
				*sim_state = SIM_STATE_UNAVAILABLE;
				break;				
			case TAPI_SIM_STATUS_SIM_NCK_REQUIRED:
				*sim_state = SIM_STATE_LOCKED;
				break;				
			case TAPI_SIM_STATUS_SIM_NSCK_REQUIRED:
				*sim_state = SIM_STATE_LOCKED;
				break;				
			case TAPI_SIM_STATUS_SIM_SPCK_REQUIRED:
				*sim_state = SIM_STATE_LOCKED;
				break;				
			case TAPI_SIM_STATUS_SIM_CCK_REQUIRED:
				*sim_state = SIM_STATE_LOCKED;
				break;				
			case TAPI_SIM_STATUS_CARD_REMOVED:
				*sim_state = SIM_STATE_UNAVAILABLE;
				break;			
			case TAPI_SIM_STATUS_SIM_LOCK_REQUIRED:
				*sim_state = SIM_STATE_LOCKED;
				break;
			default:
				*sim_state = SIM_STATE_UNAVAILABLE;
				break;			
		}
	}

	tel_deinit();
	return error_code;
}


int sim_get_subscriber_number(char** subscriber_number)
{
	int error_code = SIM_ERROR_NONE;
	int card_changed = 0;
	TelSimCardStatus_t sim_card_state = 0x00;	
	char* subscriber_number_p = NULL;

	SIM_CHECK_INPUT_PARAMETER(subscriber_number);
	SIM_INIT();

	error_code = tel_get_sim_init_info(&sim_card_state, &card_changed);
	if( error_code != 0 || sim_card_state != TAPI_SIM_STATUS_SIM_INIT_COMPLETED )
	{
		LOGE("[%s] NOT_AVAILABLE(0x%08x)", __FUNCTION__, SIM_ERROR_NOT_AVAILABLE);		
		error_code = SIM_ERROR_NOT_AVAILABLE;	
	}
	else
	{
		subscriber_number_p = vconf_get_str(VCONFKEY_TELEPHONY_SUBSCRIBER_NUMBER);
		if( subscriber_number_p == NULL )
		{
			LOGE("[%s] OPERATION_FAILED(0x%08x)", __FUNCTION__, SIM_ERROR_OPERATION_FAILED);
			error_code = SIM_ERROR_OPERATION_FAILED;
		}
		else if( strlen(subscriber_number_p) == 0 )	
		{
			LOGI("[%s] subscriber number has no value", __FUNCTION__);
			free(subscriber_number_p);
			*subscriber_number = NULL;
		}
		else
		{
			*subscriber_number = subscriber_number_p;
		}
	}

	tel_deinit();
	return error_code;
}

int sim_set_state_changed_cb(sim_state_changed_cb sim_cb, void* user_data)
{
	sim_state_e state = SIM_STATE_UNKNOWN;

	SIM_CHECK_INPUT_PARAMETER(sim_cb);

	if( sim_get_state(&state) != SIM_ERROR_NONE )
	{
		LOGE("[%s] OPERATION_FAILED(0x%08x) : fail to get current state of SIM", __FUNCTION__, SIM_ERROR_OPERATION_FAILED);
		return SIM_ERROR_OPERATION_FAILED;
	}

	if( init_is_registered == false) 
	{		
		if( vconf_notify_key_changed(VCONFKEY_TELEPHONY_SIM_INIT, (vconf_callback_fn)__sim_state_changed_cb_adapter, NULL) != 0 )
		{
			LOGE("[%s] OPERATION_FAILED(0x%08x) : fail to register callback of sim initialization", 
					__FUNCTION__, SIM_ERROR_OPERATION_FAILED);
			return SIM_ERROR_OPERATION_FAILED;
		}
		init_is_registered = true;
	}

	if( chv_is_registered == false )
	{
		if( vconf_notify_key_changed(VCONFKEY_TELEPHONY_SIM_CHV, (vconf_callback_fn)__sim_state_changed_cb_adapter, NULL) != 0)
		{
			LOGE("[%s] OPERATION_FAILED(0x%08x) : fail to register callback of several lock verification", 
					__FUNCTION__, SIM_ERROR_OPERATION_FAILED);
			return SIM_ERROR_OPERATION_FAILED;
		}
		chv_is_registered = true;		
	}

	if( slot_is_registered == false )
	{
		if(vconf_notify_key_changed(VCONFKEY_TELEPHONY_SIM_SLOT, (vconf_callback_fn)__sim_state_changed_cb_adapter, NULL) != 0)
		{
			LOGE("[%s] OPERATION_FAILED(0x%08x) : fail to register callback of sim slot", 
					__FUNCTION__, SIM_ERROR_OPERATION_FAILED);
			return SIM_ERROR_OPERATION_FAILED;
		}
		slot_is_registered = true;
	}

	sim_state_cb.previous_state = state;
	sim_state_cb.cb = sim_cb;
	sim_state_cb.user_data = user_data;

	return SIM_ERROR_NONE;
}

int sim_unset_state_changed_cb()
{
	if( init_is_registered == true) 
	{		
		if( vconf_ignore_key_changed(VCONFKEY_TELEPHONY_SIM_INIT, (vconf_callback_fn)__sim_state_changed_cb_adapter) != 0 )
		{
			LOGE("[%s] OPERATION_FAILED(0x%08x) : fail to unregister callback of sim initialization", 
					__FUNCTION__, SIM_ERROR_OPERATION_FAILED);
			return SIM_ERROR_OPERATION_FAILED;
		}
		init_is_registered = false;
	}

	if( chv_is_registered == true )
	{
		if( vconf_ignore_key_changed(VCONFKEY_TELEPHONY_SIM_CHV, (vconf_callback_fn)__sim_state_changed_cb_adapter) != 0)
		{
			LOGE("[%s] OPERATION_FAILED(0x%08x) : fail to unregister callback of several lock verification", 
					__FUNCTION__, SIM_ERROR_OPERATION_FAILED);
			return SIM_ERROR_OPERATION_FAILED;
		}
		chv_is_registered = false;		
	}

	if( slot_is_registered == true )
	{
		if( vconf_ignore_key_changed(VCONFKEY_TELEPHONY_SIM_SLOT, (vconf_callback_fn)__sim_state_changed_cb_adapter) != 0)
		{
			LOGE("[%s] OPERATION_FAILED(0x%08x) : fail to unregister callback of sim slot", 
					__FUNCTION__, SIM_ERROR_OPERATION_FAILED);
			return SIM_ERROR_OPERATION_FAILED;
		}
		slot_is_registered = false;
	}

	sim_state_cb.previous_state = SIM_STATE_UNKNOWN;
	sim_state_cb.cb = NULL;
	sim_state_cb.user_data = NULL;

	return SIM_ERROR_NONE;
}

static void __sim_state_changed_cb_adapter(keynode_t *node, void* user_data) 
{
	sim_state_e sim_state = SIM_STATE_UNKNOWN;

	if( sim_state_cb.cb == NULL )
	{
		return;
	}

	if( sim_get_state(&sim_state) == SIM_ERROR_NONE )
	{
		if( sim_state != sim_state_cb.previous_state )
		{
			((sim_state_changed_cb)(sim_state_cb.cb))(sim_state, sim_state_cb.user_data);
			sim_state_cb.previous_state = sim_state;
		}
	}
}





