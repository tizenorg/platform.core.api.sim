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
#include <tapi_common.h>
#include <TapiUtility.h>
#include <ITapiSim.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dlog.h>

#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "TIZEN_N_SIM"

struct tapi_handle {
	gpointer dbus_connection;
	char *path;
	char *cp_name;
	GHashTable *evt_list;
	char cookie[20];
};

typedef struct sim_cb_data {
	sim_state_e previous_state;
	struct tapi_handle *th;
	void* cb;
	void* user_data;
} sim_cb_data;

static struct tapi_handle *ghandle = NULL;

// Internal Macros
#define SIM_CHECK_INPUT_PARAMETER(arg) \
	if( arg == NULL ) \
	{ \
		LOGE("[%s] INVALID_PARAMETER(0x%08x)", __FUNCTION__, SIM_ERROR_INVALID_PARAMETER); \
		return SIM_ERROR_INVALID_PARAMETER; \
	}

#define SIM_INIT(th) \
	th = tel_init(NULL); \
	if (!th) { \
		LOGE("[%s] OPERATION_FAILED(0x%08x)", __FUNCTION__, SIM_ERROR_OPERATION_FAILED); \
		return SIM_ERROR_OPERATION_FAILED; \
	}

#define SIM_MAKE_CB(ccb,th,callback,user_data)  \
	ccb = (sim_cb_data*) calloc(sizeof(sim_cb_data), 1);\
	ccb->th = th; \
	ccb->cb = (void*) callback;\
	ccb->user_data = user_data

static sim_error_e _convert_access_rt_to_sim_error(TelSimAccessResult_t access_rt)
{
	sim_error_e error = SIM_ERROR_NONE;
	switch (access_rt) {
		case TAPI_SIM_ACCESS_SUCCESS:
		case TAPI_SIM_ACCESS_FILE_NOT_FOUND:
			error = SIM_ERROR_NONE;
			break;
		case TAPI_SIM_ACCESS_ACCESS_CONDITION_NOT_SATISFIED:
		case TAPI_SIM_ACCESS_CARD_ERROR:
			error = SIM_ERROR_NOT_AVAILABLE;
			break;
		case TAPI_SIM_ACCESS_FAILED:
		default:
			error = SIM_ERROR_OPERATION_FAILED;
			break;
	}
	return error;
}

int sim_get_icc_id(char** icc_id)
{
	int error_code = SIM_ERROR_NONE;
	int card_changed = 0;
	TelSimCardStatus_t sim_card_state = 0x00;
	struct tapi_handle *th = NULL;
	GError *gerr = NULL;
	GVariant *sync_gv = NULL;
	gchar *iccid = NULL;
	TelSimAccessResult_t result = TAPI_SIM_ACCESS_SUCCESS;

	SIM_CHECK_INPUT_PARAMETER(icc_id);
	SIM_INIT(th);

	if (tel_get_sim_init_info(th, &sim_card_state, &card_changed) != 0 || sim_card_state != TAPI_SIM_STATUS_SIM_INIT_COMPLETED) {
		LOGE("[%s] NOT_AVAILABLE(0x%08x)", __FUNCTION__, SIM_ERROR_NOT_AVAILABLE);
		error_code = SIM_ERROR_NOT_AVAILABLE;
		*icc_id = NULL;
	} else {
		sync_gv = g_dbus_connection_call_sync(th->dbus_connection, DBUS_TELEPHONY_SERVICE, th->path,
				DBUS_TELEPHONY_SIM_INTERFACE, "GetICCID", NULL, NULL, G_DBUS_CALL_FLAGS_NONE, -1,
				NULL, &gerr);

		if (sync_gv) {
			g_variant_get(sync_gv, "(is)", &result, &iccid);
			if (result == TAPI_SIM_ACCESS_SUCCESS) {
				if (iccid != NULL && iccid[0] != '\0') {
					*icc_id = g_strdup(iccid);
				} else {
					*icc_id = NULL;
				}
			} else {
				error_code = _convert_access_rt_to_sim_error(result);
				*icc_id = NULL;
			}
			g_free(iccid);
		} else {
			LOGE("g_dbus_conn failed. error (%s)", gerr->message);
			g_error_free(gerr);
			error_code = SIM_ERROR_OPERATION_FAILED;
			*icc_id = NULL;
		}
		g_variant_unref(sync_gv);
	}
	tel_deinit(th);
	return error_code;
}

int sim_get_mcc(char** mcc)
{
	TelSimImsiInfo_t sim_imsi_info;
	int error_code = SIM_ERROR_NONE;
	int card_changed = 0;
	TelSimCardStatus_t sim_card_state = 0x00;
	struct tapi_handle *th = NULL;

	SIM_CHECK_INPUT_PARAMETER(mcc);
	SIM_INIT(th);

	if (tel_get_sim_init_info(th, &sim_card_state, &card_changed) != 0 || sim_card_state != TAPI_SIM_STATUS_SIM_INIT_COMPLETED) {
		LOGE("[%s] NOT_AVAILABLE(0x%08x)", __FUNCTION__, SIM_ERROR_NOT_AVAILABLE);
		error_code = SIM_ERROR_NOT_AVAILABLE;
		*mcc = NULL;
	} else {
		if (tel_get_sim_imsi(th, &sim_imsi_info) != 0) {
			LOGE("[%s] OPERATION_FAILED(0x%08x)", __FUNCTION__, SIM_ERROR_OPERATION_FAILED);
			error_code = SIM_ERROR_OPERATION_FAILED;
		} else {
			*mcc = g_strdup(sim_imsi_info.szMcc);
		}
	}
	tel_deinit(th);
	return error_code;
}

int sim_get_mnc(char** mnc)
{
	TelSimImsiInfo_t sim_imsi_info;
	int error_code = SIM_ERROR_NONE;
	int card_changed = 0;
	TelSimCardStatus_t sim_card_state = 0x00;
	struct tapi_handle *th = NULL;

	SIM_CHECK_INPUT_PARAMETER(mnc);
	SIM_INIT(th);

	if (tel_get_sim_init_info(th, &sim_card_state, &card_changed) != 0 || sim_card_state != TAPI_SIM_STATUS_SIM_INIT_COMPLETED) {
		LOGE("[%s] NOT_AVAILABLE(0x%08x)", __FUNCTION__, SIM_ERROR_NOT_AVAILABLE);
		error_code = SIM_ERROR_NOT_AVAILABLE;
		*mnc = NULL;
	} else {
		if (tel_get_sim_imsi(th, &sim_imsi_info) != 0) {
			LOGE("[%s] OPERATION_FAILED(0x%08x)", __FUNCTION__, SIM_ERROR_OPERATION_FAILED);
			error_code = SIM_ERROR_OPERATION_FAILED;
		} else {
			*mnc = g_strdup(sim_imsi_info.szMnc);
		}
	}
	tel_deinit(th);
	return SIM_ERROR_NONE;
}

int sim_get_msin(char** msin)
{
	TelSimImsiInfo_t sim_imsi_info;
	int error_code = SIM_ERROR_NONE;
	int card_changed = 0;
	TelSimCardStatus_t sim_card_state = 0x00;
	struct tapi_handle *th = NULL;

	SIM_CHECK_INPUT_PARAMETER(msin);
	SIM_INIT(th);

	if (tel_get_sim_init_info(th, &sim_card_state, &card_changed) != 0 || sim_card_state != TAPI_SIM_STATUS_SIM_INIT_COMPLETED) {
		LOGE("[%s] NOT_AVAILABLE(0x%08x)", __FUNCTION__, SIM_ERROR_NOT_AVAILABLE);
		error_code = SIM_ERROR_NOT_AVAILABLE;
		*msin = NULL;
	} else {
		if (tel_get_sim_imsi(th, &sim_imsi_info) != 0) {
			LOGE("[%s] OPERATION_FAILED(0x%08x)", __FUNCTION__, SIM_ERROR_OPERATION_FAILED);
			error_code = SIM_ERROR_OPERATION_FAILED;
		} else {
			*msin = g_strdup(sim_imsi_info.szMsin);
		}
	}
	tel_deinit(th);
	return error_code;
}

int sim_get_spn(char** spn)
{
	int error_code = SIM_ERROR_NONE;
	int card_changed = 0;
	TelSimCardStatus_t sim_card_state = 0x00;
	struct tapi_handle *th = NULL;
	GError *gerr = NULL;
	GVariant *sync_gv = NULL;
	TelSimAccessResult_t result = TAPI_SIM_ACCESS_SUCCESS;
	gchar *spn_str = NULL;
	guchar dc = 0;

	SIM_CHECK_INPUT_PARAMETER(spn);
	SIM_INIT(th);

	if (tel_get_sim_init_info(th, &sim_card_state, &card_changed) != 0 || sim_card_state != TAPI_SIM_STATUS_SIM_INIT_COMPLETED) {
		LOGE("[%s] NOT_AVAILABLE(0x%08x)", __FUNCTION__, SIM_ERROR_NOT_AVAILABLE);
		error_code = SIM_ERROR_NOT_AVAILABLE;
		*spn = NULL;
	} else {
		sync_gv = g_dbus_connection_call_sync(th->dbus_connection, DBUS_TELEPHONY_SERVICE, th->path,
				DBUS_TELEPHONY_SIM_INTERFACE, "GetSpn", NULL, NULL, G_DBUS_CALL_FLAGS_NONE, -1,
				NULL, &gerr);

		if (sync_gv) {
			g_variant_get(sync_gv, "(iys)", &result, &dc, &spn_str);
			if (result == TAPI_SIM_ACCESS_SUCCESS) {
				if (spn_str != NULL && spn_str[0] != '\0') {
					*spn = g_strdup(spn_str);
				} else {
					*spn = NULL;
				}
			} else {
				error_code = _convert_access_rt_to_sim_error(result);
				*spn = NULL;
			}
			g_free(spn_str);
		} else {
			LOGE("g_dbus_conn failed. error (%s)", gerr->message);
			g_error_free(gerr);
			error_code = SIM_ERROR_OPERATION_FAILED;
			*spn = NULL;
		}
		g_variant_unref(sync_gv);
	}
	tel_deinit(th);
	return error_code;
}

int sim_get_cphs_operator_name(char** full_name, char** short_name)
{
	int error_code = SIM_ERROR_NONE;
	int card_changed = 0;
	TelSimCardStatus_t sim_card_state = 0x00;
	struct tapi_handle *th = NULL;
	GError *gerr = NULL;
	GVariant *sync_gv = NULL;
	TelSimAccessResult_t result = TAPI_SIM_ACCESS_SUCCESS;
	gchar *full_str = NULL;
	gchar *short_str = NULL;

	SIM_CHECK_INPUT_PARAMETER(full_name);
	SIM_CHECK_INPUT_PARAMETER(short_name);
	SIM_INIT(th);

	if (tel_get_sim_init_info(th, &sim_card_state, &card_changed) != 0 || sim_card_state != TAPI_SIM_STATUS_SIM_INIT_COMPLETED) {
		LOGE("[%s] NOT_AVAILABLE(0x%08x)", __FUNCTION__, SIM_ERROR_NOT_AVAILABLE);
		error_code = SIM_ERROR_NOT_AVAILABLE;
		*full_name = NULL;
		*short_name = NULL;
	} else {
		sync_gv = g_dbus_connection_call_sync(th->dbus_connection, DBUS_TELEPHONY_SERVICE, th->path,
				DBUS_TELEPHONY_SIM_INTERFACE, "GetCphsNetName", NULL, NULL, G_DBUS_CALL_FLAGS_NONE,
				-1, NULL, &gerr);

		if (sync_gv) {
			g_variant_get(sync_gv, "(iss)", &result, &full_str, &short_str);
			if (result == TAPI_SIM_ACCESS_SUCCESS) {
				if (full_str != NULL && full_str[0] != '\0') {
					*full_name = g_strdup(full_str);
				} else {
					*full_name = NULL;
				}

				if (short_str != NULL && short_str[0] != '\0') {
					*full_name = g_strdup(short_str);
				} else {
					*short_name = NULL;
				}
			} else {
				error_code = _convert_access_rt_to_sim_error(result);
				*full_name = NULL;
				*short_name = NULL;
			}
			g_free(full_str);
			g_free(short_str);
		} else {
			LOGE("g_dbus_conn failed. error (%s)", gerr->message);
			g_error_free(gerr);
			error_code = SIM_ERROR_OPERATION_FAILED;
			*full_name = NULL;
			*short_name = NULL;
		}
		g_variant_unref(sync_gv);
	}
	tel_deinit(th);
	return error_code;
}

int sim_get_state(sim_state_e* sim_state)
{
	int card_changed = 0;
	TelSimCardStatus_t sim_card_state = 0x00;
	int error_code = SIM_ERROR_NONE;
	struct tapi_handle *th = NULL;

	SIM_CHECK_INPUT_PARAMETER(sim_state);
	SIM_INIT(th);

	if (tel_get_sim_init_info(th, &sim_card_state, &card_changed) != 0) {
		LOGE("[%s] OPERATION_FAILED(0x%08x)", __FUNCTION__, SIM_ERROR_OPERATION_FAILED);
		error_code = SIM_ERROR_OPERATION_FAILED;
	} else {
		switch (sim_card_state) {
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

	tel_deinit(th);
	return error_code;
}

int sim_get_subscriber_number(char** subscriber_number)
{
	int error_code = SIM_ERROR_NONE;
	int card_changed = 0;
	TelSimCardStatus_t sim_card_state = 0x00;
	struct tapi_handle *th = NULL;
	GError *gerr = NULL;
	GVariant *sync_gv = NULL;
	GVariant *value = NULL;
	GVariantIter *iter = NULL;
	GVariantIter *iter_row = NULL;
	const gchar *key = NULL;
	const gchar *str_value = NULL;
	TelSimAccessResult_t result = TAPI_SIM_ACCESS_SUCCESS;
	TelSimMsisdnList_t list;
	int i = 0;

	SIM_CHECK_INPUT_PARAMETER(subscriber_number);
	SIM_INIT(th);

	if (tel_get_sim_init_info(th, &sim_card_state, &card_changed) != 0 || sim_card_state != TAPI_SIM_STATUS_SIM_INIT_COMPLETED) {
		LOGE("[%s] NOT_AVAILABLE(0x%08x)", __FUNCTION__, SIM_ERROR_NOT_AVAILABLE);
		error_code = SIM_ERROR_NOT_AVAILABLE;
		*subscriber_number = NULL;
	} else {
		sync_gv = g_dbus_connection_call_sync(th->dbus_connection, DBUS_TELEPHONY_SERVICE, th->path,
				DBUS_TELEPHONY_SIM_INTERFACE, "GetMSISDN", NULL, NULL, G_DBUS_CALL_FLAGS_NONE, -1,
				NULL, &gerr);

		if (sync_gv) {
			memset(&list, 0, sizeof(TelSimMsisdnList_t));
			g_variant_get(sync_gv, "(iaa{sv})", &result, &iter);
			list.count = g_variant_iter_n_children(iter);

			if (result == TAPI_SIM_ACCESS_SUCCESS) {
				i = 0;
				while (g_variant_iter_next(iter, "a{sv}", &iter_row)) {
					while (g_variant_iter_loop(iter_row, "{sv}", &key, &value)) {
						if (!g_strcmp0(key, "name")) {
							str_value = g_variant_get_string(value, NULL);
							snprintf(list.list[i].name, strlen(str_value) + 1, "%s", str_value);
						}
						if (!g_strcmp0(key, "number")) {
							str_value = g_variant_get_string(value, NULL);
							snprintf(list.list[i].num, strlen(str_value) + 1, "%s", str_value);
						}
					}
					i++;
					g_variant_iter_free(iter_row);
				}
				g_variant_iter_free(iter);

				if (strlen(list.list[0].num) != 0) {
					*subscriber_number = (char*) malloc(strlen(list.list[0].num) + 1);
					if (*subscriber_number == NULL) {
						LOGE("[%s] OUT_OF_MEMORY(0x%08x)", __FUNCTION__, SIM_ERROR_OUT_OF_MEMORY);
						error_code = SIM_ERROR_OUT_OF_MEMORY;
					} else {
						snprintf(*subscriber_number, strlen(list.list[0].num) + 1, "%s",
								list.list[0].num);
					}
				} else {
					*subscriber_number = NULL;
				}

			} else {
				error_code = _convert_access_rt_to_sim_error(result);
				*subscriber_number = NULL;
			}
		} else {
			LOGE("g_dbus_conn failed. error (%s)", gerr->message);
			g_error_free(gerr);
			error_code = SIM_ERROR_OPERATION_FAILED;
			*subscriber_number = NULL;
		}
	}
	tel_deinit(th);
	return error_code;
}

static void on_noti_sim_status(struct tapi_handle *handle, const char *noti_id, void *data,
		void *user_data)
{
	TelSimCardStatus_t *status = data;
	sim_cb_data *ccb = user_data;
	sim_state_e state = SIM_STATE_UNKNOWN;
	sim_state_changed_cb cb;
	LOGE("event(%s) receive with status[%d]", TAPI_NOTI_SIM_STATUS, *status);

	switch (*status) {
		case TAPI_SIM_STATUS_CARD_ERROR:
			state = SIM_STATE_UNAVAILABLE;
			break;
		case TAPI_SIM_STATUS_CARD_NOT_PRESENT:
			state = SIM_STATE_UNAVAILABLE;
			break;
		case TAPI_SIM_STATUS_SIM_INITIALIZING:
			state = SIM_STATE_UNKNOWN;
			break;
		case TAPI_SIM_STATUS_SIM_INIT_COMPLETED:
			state = SIM_STATE_AVAILABLE;
			break;
		case TAPI_SIM_STATUS_SIM_PIN_REQUIRED:
			state = SIM_STATE_LOCKED;
			break;
		case TAPI_SIM_STATUS_SIM_PUK_REQUIRED:
			state = SIM_STATE_LOCKED;
			break;
		case TAPI_SIM_STATUS_CARD_BLOCKED:
			state = SIM_STATE_UNAVAILABLE;
			break;
		case TAPI_SIM_STATUS_SIM_NCK_REQUIRED:
			state = SIM_STATE_LOCKED;
			break;
		case TAPI_SIM_STATUS_SIM_NSCK_REQUIRED:
			state = SIM_STATE_LOCKED;
			break;
		case TAPI_SIM_STATUS_SIM_SPCK_REQUIRED:
			state = SIM_STATE_LOCKED;
			break;
		case TAPI_SIM_STATUS_SIM_CCK_REQUIRED:
			state = SIM_STATE_LOCKED;
			break;
		case TAPI_SIM_STATUS_CARD_REMOVED:
			state = SIM_STATE_UNAVAILABLE;
			break;
		case TAPI_SIM_STATUS_SIM_LOCK_REQUIRED:
			state = SIM_STATE_LOCKED;
			break;
		default:
			state = SIM_STATE_UNAVAILABLE;
			break;
	}
	if (!ccb->cb) {
		LOGE("[%s] callback is null", __FUNCTION__);
		return;
	}
	cb = ccb->cb;
	cb(state, ccb->user_data);
}

int sim_set_state_changed_cb(sim_state_changed_cb sim_cb, void* user_data)
{
	int error_code = SIM_ERROR_NONE;
	int ret;
	sim_cb_data *ccb = NULL;

	SIM_CHECK_INPUT_PARAMETER(sim_cb);

	ghandle = tel_init(NULL);
	if (!ghandle) {
		LOGE("[%s] OPERATION_FAILED(0x%08x)", __FUNCTION__, SIM_ERROR_OPERATION_FAILED);
		return SIM_ERROR_OPERATION_FAILED;
	}

	ccb = (sim_cb_data*) calloc(sizeof(sim_cb_data), 1);
	ccb->th = ghandle;
	ccb->cb = (void*) sim_cb;
	ccb->user_data = user_data;

	ret = tel_register_noti_event(ghandle, TAPI_NOTI_SIM_STATUS, on_noti_sim_status, ccb);
	if (ret != TAPI_API_SUCCESS) error_code = SIM_ERROR_OPERATION_FAILED;
	return error_code;
}

int sim_unset_state_changed_cb()
{
	int error_code = SIM_ERROR_NONE;
	int ret;

	if (ghandle == NULL) {
		ghandle = tel_init(NULL);
		if (!ghandle) {
			LOGE("[%s] OPERATION_FAILED(0x%08x)", __FUNCTION__, SIM_ERROR_OPERATION_FAILED);
			return SIM_ERROR_OPERATION_FAILED;
		}
	}
	ret = tel_deregister_noti_event(ghandle, TAPI_NOTI_SIM_STATUS);
	if (ret != TAPI_API_SUCCESS) error_code = SIM_ERROR_OPERATION_FAILED;
	return error_code;
}
