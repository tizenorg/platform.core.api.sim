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
	char* full_name = NULL;
	char* short_name = NULL;
	int ret_value = sim_get_cphs_operator_name(&full_name, &short_name);

	switch(ret_value)
	{
		case SIM_ERROR_NONE:
			ret = 0;
			if( full_name != NULL )
			{
				LOGI("CPHS full ONS is %s", full_name);
				free(full_name);
			}
			else
			{
				LOGI("CPHS full ONS is NULL");
			}

			if( short_name != NULL )
			{
				LOGI("CPHS short ONS is %s", short_name);
				free(short_name);
			}
			else
			{
				LOGI("CPHS short ONS is NULL");
			}
			break;
		case SIM_ERROR_INVALID_PARAMETER:
			LOGE("Invalid parameter");
			ret = -1;
			break;
		case SIM_ERROR_OPERATION_FAILED:
			LOGE("Cannot find SPN value");
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
