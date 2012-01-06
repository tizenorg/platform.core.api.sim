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


#include <stdio.h>
#include <stdlib.h>
#include <sim.h>
#include <dlog.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "TIZEN_N_SIM_TEST"

int main()
{
	int ret = 0;
	char* mcc = NULL;
	int ret_value = sim_get_mcc(&mcc);

	switch(ret_value)
	{
		case SIM_ERROR_NONE:
			LOGI("MCC is %s", mcc);
			ret = 0;
			free(mcc);
			break;
		case SIM_ERROR_OUT_OF_MEMORY:
			LOGE("Out of memory");
			ret = -1;
			break;                                                
		case SIM_ERROR_INVALID_PARAMETER:
			LOGE("Invalid parameter");
			ret = -1;
			break;                                                                        
		case SIM_ERROR_OPERATION_FAILED:
			LOGE("Can not get MCC value");
			ret = -1;
			break;
		case SIM_ERROR_NOT_AVAILABLE:
			LOGE("SIM is not available");
			ret = -1;
			break;						
		default:
			LOGE("Unexpected return value");
			ret = -1;
			break;
	}

	return ret;
}
