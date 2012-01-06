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
#include <string.h>
#include <sim.h>
#include <dlog.h>
#include <glib.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "TIZEN_N_SIM_TEST"

static GMainLoop *event_loop;

char* convert_state_to_string(sim_state_e sim_state)
{
	switch(sim_state)
	{
		case SIM_STATE_AVAILABLE:
			return "available";
		case SIM_STATE_LOCKED:
			return "locked";
		case SIM_STATE_UNKNOWN:
			return "unknown";
		case SIM_STATE_UNAVAILABLE:
			return "unavailable";
		default:
			return "unexpected";
	}	
}

void sim_state_changed(sim_state_e state, void* user_data)
{
	LOGI("[%s] Start sim_state_changed_cb", __FUNCTION__);

	LOGI("[%s] Status of sim: %s", __FUNCTION__, convert_state_to_string(state));
	LOGI("[%s] user data: %s", __FUNCTION__, user_data);

	LOGI("[%s] End sim_state_changed_cb", __FUNCTION__);	
	g_main_loop_quit(event_loop);
}

int main()
{
	if( sim_set_state_changed_cb(sim_state_changed, "sim_state_changed_test") == SIM_ERROR_NONE )
	{
		LOGI("[%s] Succeeded to add callback function", __FUNCTION__);
	}
	else
	{
		LOGE("[%s] Failed to add callback function", __FUNCTION__);
		return -1;
	}

	LOGI("[%s] If you change the state of SIM card, then callback function will be called", __FUNCTION__);
	event_loop = g_main_loop_new(NULL, FALSE);
	g_main_loop_run(event_loop);

	if( sim_unset_state_changed_cb() == SIM_ERROR_NONE )
	{
		LOGI("[%s] Succeeded to remove callback function", __FUNCTION__);
	}
	else
	{
		LOGE("[%s] Failed to remove callback function", __FUNCTION__);
		return -1;
	}

	return 0;
}
