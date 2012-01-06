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
#include <sim.h>
#include <stdlib.h>
#include <dlog.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "TIZEN_N_SIM_TEST"


int main()
{
	int ret = 0;
	char* icc_id = NULL;
	int ret_value = sim_get_icc_id(&icc_id);

	switch(ret_value)
	{
		case SIM_ERROR_NONE:
			LOGI("ICC-ID is %s", icc_id);
			ret = 0;
			if(icc_id != NULL)
				free(icc_id);
			break;
		case SIM_ERROR_OUT_OF_MEMORY:
			LOGI("Out of memory\n");
			ret = -1;
			break;                                    
		case SIM_ERROR_INVALID_PARAMETER:
			LOGI("Invalid parameter\n");
			ret = -1;
			break;                                                
		case SIM_ERROR_OPERATION_FAILED:
			LOGI("Can not get ICC-ID value\n");
			ret = -1;
			break;
		case SIM_ERROR_NOT_AVAILABLE:
			LOGI("SIM is not available\n");
			ret = -1;
			break;						
		default:
			LOGI("Unexpected return value\n");
			ret = -1;
			break;
	}

	return ret;
}
