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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dlog.h>

#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>

#include <tapi.h>
#include <tel_sim.h>
#include <tapi_sim.h>
#include <tapi_events.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "CAPI_TELEPHONY_SIM"

typedef struct {
	sim_state_e previous_state;
	TelHandle *th;
	void *cb;
	void *user_data;
} sim_cb_data;

static TelHandle *handle = NULL;
static sim_cb_data *ccb = NULL;

// Internal Macros
#define SIM_CHECK_INPUT_PARAMETER(arg) \
	if (arg == NULL) { \
		LOGE("[%s] INVALID_PARAMETER(0x%08x)", __FUNCTION__, SIM_ERROR_INVALID_PARAMETER); \
		return SIM_ERROR_INVALID_PARAMETER; \
	}

#define SIM_INIT(th) \
	th = tapi_init(NULL); \
	if (!th) { \
		LOGE("[%s] OPERATION_FAILED(0x%08x)", __FUNCTION__, SIM_ERROR_OPERATION_FAILED); \
		return SIM_ERROR_OPERATION_FAILED; \
	}

#define SIM_DEINIT(th) tapi_deinit(th)

static sim_error_e _convert_access_rt_to_sim_error(TelSimResult access_rt)
{
	sim_error_e error = SIM_ERROR_NONE;

	switch (access_rt) {
	case TEL_SIM_RESULT_SUCCESS:
		error = SIM_ERROR_NONE;
		break;
	case TEL_SIM_RESULT_ACCESS_CONDITION_NOT_SATISFIED:
	case TEL_SIM_RESULT_CARD_ERROR:
		error = SIM_ERROR_NOT_AVAILABLE;
		break;
	case TEL_SIM_RESULT_FAILURE:
	default:
		error = SIM_ERROR_OPERATION_FAILED;
		break;
	}

	return error;
}

static int __sim_check_init_status(TelHandle *handle)
{
	TelSimCardStatusInfo sim_status_info = {0,};
	int error_code = SIM_ERROR_NONE;

	if (tapi_sim_get_init_info(handle, &sim_status_info) != TEL_RETURN_SUCCESS
			|| sim_status_info.status != TEL_SIM_STATUS_SIM_INIT_COMPLETED) {
		error_code = SIM_ERROR_NOT_AVAILABLE;
		LOGE("[%s] NOT_AVAILABLE(0x%08x)", __FUNCTION__, error_code);
	}

	return error_code;
}

static GVariant *__sim_get_info(const char *method)
{
	GVariant *sync_gv = NULL;
	char **cp_list = NULL;
	GDBusConnection *dbus_connection;
	GError *error = NULL;

	tapi_get_cp_name_list(&cp_list);
	if (cp_list == NULL) {
		LOGE("CP List is NULL");
		return NULL;
	}

	dbus_connection = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error);
	if (error) {
		LOGE("Error Creating DBus Connection: [%s]", error->message);
		g_error_free(error);
	} else {
		sync_gv = g_dbus_connection_call_sync(dbus_connection,
				TELEPHONY_SERVICE, cp_list[0],
				TELEPHONY_SIM_INTERFACE, method,
				NULL, NULL, G_DBUS_CALL_FLAGS_NONE, -1, NULL, &error);

		if (error) {
			LOGE("g_dbus_conn failed. error (%s)", error->message);
			g_error_free(error);
		}
	}

	/* Free cp_list */
	g_object_unref(dbus_connection);
	g_strfreev(cp_list);
	return sync_gv;
}

int sim_get_icc_id(char **icc_id)
{
	TelHandle *handle = NULL;
	int error_code;
	GVariant *sync_gv = NULL;

	SIM_CHECK_INPUT_PARAMETER(icc_id);
	*icc_id = NULL;
	SIM_INIT(handle);

	error_code = __sim_check_init_status(handle);
	if (error_code != SIM_ERROR_NONE) {
		LOGE("[%s] SIM NOT Initialized (0x%08x)", __FUNCTION__, error_code);
		return error_code;
	}

	sync_gv = __sim_get_info("GetIccid");
	if (sync_gv) {
		gint result;
		gchar *iccid = NULL;

		g_variant_get(sync_gv, "(is)", &result, &iccid);
		if (result == TEL_SIM_RESULT_SUCCESS) {
			if (iccid != NULL) {
				*icc_id = g_strdup(iccid);
				g_free(iccid);
				LOGI("ICCID: [%s]", *icc_id);
			} else {
				LOGE("ICCID is NULL");
			}
		} else {
			error_code = _convert_access_rt_to_sim_error(result);
			LOGE("[%s] Operation Failed!!! (0x%08x)", __FUNCTION__, error_code);
		}
		g_variant_unref(sync_gv);
	} else {
		error_code = SIM_ERROR_OPERATION_FAILED;
		LOGE("[%s] Operation Failed!!! (0x%08x)", __FUNCTION__, error_code);
	}

	SIM_DEINIT(handle);
	return error_code;
}

static int __sim_get_imsi(TelHandle *handle, TelSimImsiInfo *sim_imsi_info)
{
	int error_code = SIM_ERROR_NONE;

	error_code = __sim_check_init_status(handle);
	if (error_code != SIM_ERROR_NONE) {
		LOGE("[%s] SIM NOT Initialized (0x%08x)", __FUNCTION__, error_code);
		return error_code;
	}

	error_code = tapi_sim_get_imsi(handle, sim_imsi_info);
	return error_code;
}

int sim_get_mcc(char **mcc)
{
	TelHandle *handle = NULL;
	TelSimImsiInfo sim_imsi_info = {"",};
	int error_code;

	SIM_CHECK_INPUT_PARAMETER(mcc);
	SIM_INIT(handle);

	error_code = __sim_get_imsi(handle, &sim_imsi_info);
	if (error_code != TEL_RETURN_SUCCESS) {
		LOGE("[%s] OPERATION_FAILED(0x%08x)", __FUNCTION__, error_code);
		*mcc = NULL;
	} else {
		*mcc = g_strdup(sim_imsi_info.mcc);
		LOGI("MCC: [%s]", *mcc);
	}

	SIM_DEINIT(handle);
	return error_code;
}

int sim_get_mnc(char **mnc)
{
	TelHandle *handle = NULL;
	TelSimImsiInfo sim_imsi_info = {"",};
	int error_code;

	SIM_CHECK_INPUT_PARAMETER(mnc);
	SIM_INIT(handle);

	error_code = __sim_get_imsi(handle, &sim_imsi_info);
	if (error_code != TEL_RETURN_SUCCESS) {
		LOGE("[%s] OPERATION_FAILED(0x%08x)", __FUNCTION__, error_code);
		*mnc = NULL;
	} else {
		*mnc = g_strdup(sim_imsi_info.mnc);
		LOGI("MNC: [%s]", *mnc);
	}

	SIM_DEINIT(handle);
	return error_code;
}

int sim_get_msin(char **msin)
{
	TelHandle *handle = NULL;
	TelSimImsiInfo sim_imsi_info = {"",};
	int error_code;

	SIM_CHECK_INPUT_PARAMETER(msin);
	SIM_INIT(handle);

	error_code = __sim_get_imsi(handle, &sim_imsi_info);
	if (error_code != TEL_RETURN_SUCCESS) {
		LOGE("[%s] OPERATION_FAILED(0x%08x)", __FUNCTION__, error_code);
		*msin = NULL;
	} else {
		*msin = g_strdup(sim_imsi_info.msin);
		LOGI("MSIN: [%s]", *msin);
	}

	SIM_DEINIT(handle);
	return error_code;
}

int sim_get_spn(char **spn)
{
	TelHandle *handle = NULL;
	int error_code;
	GVariant *sync_gv = NULL;

	SIM_CHECK_INPUT_PARAMETER(spn);
	*spn = NULL;
	SIM_INIT(handle);

	error_code = __sim_check_init_status(handle);
	if (error_code != SIM_ERROR_NONE) {
		LOGE("[%s] SIM NOT initialized (0x%08x)", __FUNCTION__, error_code);
		return error_code;
	}

	sync_gv = __sim_get_info("GetSpn");
	if (sync_gv) {
		gint result;
		gint display_condition;
		gchar *spn_str = NULL;

		g_variant_get(sync_gv, "(iis)", &result, &display_condition, &spn_str);
		if (result == TEL_SIM_RESULT_SUCCESS) {
			if (spn_str != NULL) {
				*spn = g_strdup(spn_str);
				g_free(spn_str);
				LOGI("SPN: [%s]", *spn);
			} else {
				LOGE("SPN is NULL");
			}
		} else {
			error_code = _convert_access_rt_to_sim_error(result);
			LOGE("[%s] Operation Failed!!! (0x%08x)", __FUNCTION__, error_code);
		}
		g_variant_unref(sync_gv);
	} else {
		error_code = SIM_ERROR_OPERATION_FAILED;
		LOGE("[%s] Operation Failed!!! (0x%08x)", __FUNCTION__, error_code);
	}

	SIM_DEINIT(handle);
	return error_code;
}

int sim_get_cphs_operator_name(char **full_name, char **short_name)
{
	TelHandle *handle = NULL;
	int error_code;
	GVariant *sync_gv = NULL;

	SIM_CHECK_INPUT_PARAMETER(full_name);
	SIM_CHECK_INPUT_PARAMETER(short_name);
	*full_name = NULL;
	*short_name = NULL;
	SIM_INIT(handle);

	error_code = __sim_check_init_status(handle);
	if (error_code != SIM_ERROR_NONE) {
		LOGE("[%s] SIM NOT initialized (0x%08x)", __FUNCTION__, error_code);
		return error_code;
	}

	sync_gv = __sim_get_info("GetCphsNetName");
	if (sync_gv) {
		gint result;
		gchar *full_str = NULL;
		gchar *short_str = NULL;

		g_variant_get(sync_gv, "(iss)", &result, &full_str, &short_str);
		if (result == TEL_SIM_RESULT_SUCCESS) {
			if (full_str != NULL) {
				*full_name = g_strdup(full_str);
				g_free(full_str);
				LOGI("Full name: [%s]", *full_name);
			} else {
				LOGE("full_name is NULL");
			}

			if (short_str != NULL) {
				*short_name = g_strdup(short_str);
				g_free(short_str);
				LOGI("Short name: [%s]", *short_name);
			} else {
				LOGE("short_name is NULL");
			}
		} else {
			error_code = _convert_access_rt_to_sim_error(result);
			LOGE("[%s] Operation Failed!!! (0x%08x)", __FUNCTION__, error_code);
		}
		g_variant_unref(sync_gv);
	} else {
		error_code = SIM_ERROR_OPERATION_FAILED;
		LOGE("[%s] Operation Failed!!! (0x%08x)", __FUNCTION__, error_code);
	}

	SIM_DEINIT(handle);
	return error_code;
}

int sim_get_state(sim_state_e *sim_state)
{
	TelSimCardStatusInfo sim_status_info = {0,};
	int error_code = SIM_ERROR_NONE;
	TelHandle *handle = NULL;

	SIM_CHECK_INPUT_PARAMETER(sim_state);
	SIM_INIT(handle);

	if (tapi_sim_get_init_info(handle, &sim_status_info) != TEL_RETURN_SUCCESS) {
		LOGE("[%s] OPERATION_FAILED(0x%08x)", __FUNCTION__, SIM_ERROR_OPERATION_FAILED);
		*sim_state = SIM_STATE_UNKNOWN;
		SIM_DEINIT(handle);
		return SIM_ERROR_OPERATION_FAILED;
	}

	switch (sim_status_info.status) {
	case TEL_SIM_STATUS_CARD_ERROR:
	case TEL_SIM_STATUS_SIM_CARD_POWEROFF:
	case TEL_SIM_STATUS_CARD_NOT_PRESENT:
	case TEL_SIM_STATUS_CARD_BLOCKED:
	case TEL_SIM_STATUS_CARD_REMOVED:
		*sim_state = SIM_STATE_UNAVAILABLE;
		break;
	case TEL_SIM_STATUS_UNKNOWN:
	case TEL_SIM_STATUS_SIM_INITIALIZING:
		*sim_state = SIM_STATE_UNKNOWN;
		break;
	case TEL_SIM_STATUS_SIM_INIT_COMPLETED:
		*sim_state = SIM_STATE_AVAILABLE;
		break;
	case TEL_SIM_STATUS_SIM_PIN_REQUIRED:
	case TEL_SIM_STATUS_SIM_PUK_REQUIRED:
	case TEL_SIM_STATUS_SIM_NCK_REQUIRED:
	case TEL_SIM_STATUS_SIM_NSCK_REQUIRED:
	case TEL_SIM_STATUS_SIM_SPCK_REQUIRED:
	case TEL_SIM_STATUS_SIM_CCK_REQUIRED:
	case TEL_SIM_STATUS_SIM_LOCK_REQUIRED:
		*sim_state = SIM_STATE_LOCKED;
		break;
	}

	LOGI("SIM state: [%d]", *sim_state);

	SIM_DEINIT(handle);
	return error_code;
}

int sim_get_subscriber_number(char **subscriber_number)
{
	TelHandle *handle = NULL;
	int error_code;
	GVariant *sync_gv = NULL;

	SIM_CHECK_INPUT_PARAMETER(subscriber_number);
	*subscriber_number = NULL;
	SIM_INIT(handle);

	error_code = __sim_check_init_status(handle);
	if (error_code != SIM_ERROR_NONE) {
		LOGE("[%s] SIM NOT initialized (0x%08x)", __FUNCTION__, error_code);
		return error_code;
	}

	sync_gv = __sim_get_info("GetMsisdn");
	if (sync_gv) {
		gint result;
		GVariant *var = NULL;
		TelSimMsisdnList msisdn_list = {0,};

		g_variant_get(sync_gv, "(iu@aa{sv})", &result, &msisdn_list.count, &var);
		if (result == TEL_SIM_RESULT_SUCCESS && msisdn_list.count) {
			int i = 0;
			GVariantIter *iter = NULL;
			GVariantIter *iter_row = NULL;

			LOGI("MSISDN list count: [%d]", msisdn_list.count);
			msisdn_list.list = g_malloc0(msisdn_list.count * sizeof(TelSimMsisdnList));
			g_variant_get(var, "aa{sv}", &iter);
			while (g_variant_iter_next(iter, "a{sv}", &iter_row)) {
				const gchar *key;
				GVariant *key_value;
				while (g_variant_iter_loop(iter_row, "{sv}", &key, &key_value)) {
					if (g_strcmp0(key, "alpha_id") == 0) {
						msisdn_list.list[i].alpha_id =
							g_strdup(g_variant_get_string(key_value, NULL));
					}

					if (g_strcmp0(key, "num") == 0) {
						msisdn_list.list[i].num =
							g_strdup(g_variant_get_string(key_value, NULL));
					}
				}
				i++;
				g_variant_iter_free(iter_row);
			}
			g_variant_iter_free(iter);

			/* Only set first number */
			if (msisdn_list.count) {
				*subscriber_number = g_strdup(msisdn_list.list[0].num);
				LOGI("Subscribe Number: [%s]", *subscriber_number);
			}

			/* Free resources */
			for (i = 0; i < msisdn_list.count; i++) {
				g_free(msisdn_list.list[i].alpha_id);
				g_free(msisdn_list.list[i].num);
			}
			g_free(msisdn_list.list);
		} else {
			error_code = _convert_access_rt_to_sim_error(result);
			LOGE("[%s] Operation Failed!!! (0x%08x)", __FUNCTION__, error_code);
		}
		g_variant_unref(sync_gv);
	} else {
		error_code = SIM_ERROR_OPERATION_FAILED;
		LOGE("[%s] Operation Failed!!! (0x%08x)", __FUNCTION__, error_code);
	}

	SIM_DEINIT(handle);
	return error_code;
}

static void on_noti_sim_status(TelHandle *handle, const char *noti_id,
		void *data, void *user_data)
{
	TelSimCardStatus *status = data;
	sim_state_e state = SIM_STATE_UNKNOWN;
	sim_state_changed_cb cb;

	LOGE("event(%s) receive with status[%d]", TEL_NOTI_SIM_STATUS, *status);

	switch (*status) {
	case TEL_SIM_STATUS_CARD_ERROR:
	case TEL_SIM_STATUS_SIM_CARD_POWEROFF:
	case TEL_SIM_STATUS_CARD_NOT_PRESENT:
	case TEL_SIM_STATUS_CARD_BLOCKED:
	case TEL_SIM_STATUS_CARD_REMOVED:
		state = SIM_STATE_UNAVAILABLE;
		break;
	case TEL_SIM_STATUS_UNKNOWN:
	case TEL_SIM_STATUS_SIM_INITIALIZING:
		state = SIM_STATE_UNKNOWN;
		break;
	case TEL_SIM_STATUS_SIM_INIT_COMPLETED:
		state = SIM_STATE_AVAILABLE;
		break;
	case TEL_SIM_STATUS_SIM_PIN_REQUIRED:
	case TEL_SIM_STATUS_SIM_PUK_REQUIRED:
	case TEL_SIM_STATUS_SIM_NCK_REQUIRED:
	case TEL_SIM_STATUS_SIM_NSCK_REQUIRED:
	case TEL_SIM_STATUS_SIM_SPCK_REQUIRED:
	case TEL_SIM_STATUS_SIM_CCK_REQUIRED:
	case TEL_SIM_STATUS_SIM_LOCK_REQUIRED:
		state = SIM_STATE_LOCKED;
		break;
	}

	if (!ccb->cb) {
		LOGE("[%s] callback is null", __FUNCTION__);
		return;
	}

	cb = ccb->cb;
	cb(state, ccb->user_data);
}

int sim_set_state_changed_cb(sim_state_changed_cb sim_cb, void *user_data)
{
	int error_code = SIM_ERROR_NONE;

	SIM_CHECK_INPUT_PARAMETER(sim_cb);

	if (handle == NULL)
		SIM_INIT(handle);

	if (ccb == NULL)
		ccb = g_malloc0(sizeof(sim_cb_data));

	ccb->th = handle;
	ccb->cb = (void *)sim_cb;
	ccb->user_data = user_data;

	error_code = tapi_register_event_id(handle, TEL_NOTI_SIM_STATUS, on_noti_sim_status, NULL);
	if (error_code != TEL_RETURN_SUCCESS) {
		error_code = SIM_ERROR_OPERATION_FAILED;
		SIM_DEINIT(handle);
		handle = NULL;
		g_free(ccb);
		ccb = NULL;
	}

	return error_code;
}

int sim_unset_state_changed_cb()
{
	int error_code = SIM_ERROR_NONE;

	if (handle == NULL)
		return SIM_ERROR_NONE;

	error_code = tapi_deregister_event_id(handle, TEL_NOTI_SIM_STATUS);
	if (error_code != TEL_RETURN_SUCCESS)
		error_code = SIM_ERROR_OPERATION_FAILED;

	SIM_DEINIT(handle);
	handle = NULL;
	g_free(ccb);
	ccb = NULL;
	return error_code;
}
