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


#include <tet_api.h>
#include <sim.h>
#include <stdlib.h>

enum {
	POSITIVE_TC_IDX = 0x01,
	NEGATIVE_TC_IDX,
};

static void startup(void);
static void cleanup(void);

void (*tet_startup)(void) = startup;
void (*tet_cleanup)(void) = cleanup;

static void utc_telephony_sim_get_icc_id_n(void);
static void utc_telephony_sim_get_icc_id_p(void);
static void utc_telephony_sim_get_mcc_n(void);
static void utc_telephony_sim_get_mcc_p(void);
static void utc_telephony_sim_get_mnc_n(void);
static void utc_telephony_sim_get_mnc_p(void);
static void utc_telephony_sim_get_msin_n(void);
static void utc_telephony_sim_get_msin_p(void);
static void utc_telephony_sim_get_spn_n(void);
static void utc_telephony_sim_get_spn_p(void);
static void utc_telephony_sim_get_cphs_operator_name_n(void);
static void utc_telephony_sim_get_cphs_operator_name_p(void);
static void utc_telephony_sim_get_state_n(void);
static void utc_telephony_sim_get_state_p(void);
static void utc_telephony_sim_get_subscriber_number_n(void);
static void utc_telephony_sim_get_subscriber_number_p(void);
static void utc_telephony_sim_set_state_changed_cb_p(void);
static void utc_telephony_sim_set_state_changed_cb_n(void);
static void utc_telephony_sim_unset_state_changed_cb_p(void);



struct tet_testlist tet_testlist[] = {
	{ utc_telephony_sim_get_icc_id_n, NEGATIVE_TC_IDX },                        
	{ utc_telephony_sim_get_icc_id_p, POSITIVE_TC_IDX },
	{ utc_telephony_sim_get_mcc_n, NEGATIVE_TC_IDX },
	{ utc_telephony_sim_get_mcc_p, POSITIVE_TC_IDX },	
	{ utc_telephony_sim_get_mnc_n, NEGATIVE_TC_IDX },	
	{ utc_telephony_sim_get_mnc_p, POSITIVE_TC_IDX },
	{ utc_telephony_sim_get_msin_n, NEGATIVE_TC_IDX },
	{ utc_telephony_sim_get_msin_p, POSITIVE_TC_IDX },
	{ utc_telephony_sim_get_spn_n, NEGATIVE_TC_IDX },	
	{ utc_telephony_sim_get_spn_p, POSITIVE_TC_IDX },
	{ utc_telephony_sim_get_cphs_operator_name_n, NEGATIVE_TC_IDX },
	{ utc_telephony_sim_get_cphs_operator_name_p, POSITIVE_TC_IDX },
	{ utc_telephony_sim_get_state_n, NEGATIVE_TC_IDX },
	{ utc_telephony_sim_get_state_p, POSITIVE_TC_IDX },
	{ utc_telephony_sim_get_subscriber_number_n, NEGATIVE_TC_IDX },	
	{ utc_telephony_sim_get_subscriber_number_p, POSITIVE_TC_IDX },
	{ utc_telephony_sim_set_state_changed_cb_p, POSITIVE_TC_IDX },
	{ utc_telephony_sim_set_state_changed_cb_n, NEGATIVE_TC_IDX },
	{ utc_telephony_sim_unset_state_changed_cb_p, POSITIVE_TC_IDX },
	{ NULL, 0 },
};


static void startup(void)
{
	/* start of TC */
	tet_printf("\n TC start");
}


static void cleanup(void)
{
	/* end of TC */
	tet_printf("\n TC end");
}

/**
 * @brief Callback function
 */
void sim_state_changed(sim_state_e state, void* user_data)
{

}

/**
 * @brief Negative test case of sim_get_icc_id()
 */
static void utc_telephony_sim_get_icc_id_n(void)
{
        int ret = SIM_ERROR_NONE;

        //  Invalid parameter test
        ret = sim_get_icc_id(NULL);
        dts_check_eq("sim_get_icc_id", ret, SIM_ERROR_INVALID_PARAMETER, "Must return SIM_ERROR_INVALID_PARAMETER in case of invalid parameter"); 
}


/**
 * @brief Positive test case of sim_get_icc_id()
 */
static void utc_telephony_sim_get_icc_id_p(void)
{
        char* icc_id = NULL;
	int ret = SIM_ERROR_NONE;

        //  Normal operation test
	ret = sim_get_icc_id(&icc_id);
        if( icc_id != NULL )
                free(icc_id);
        dts_check_eq("sim_get_icc_id", ret, SIM_ERROR_NONE, "Failed to get ICC ID");
}


/**
 * @brief Negative test case of sim_get_mcc()
 */
static void utc_telephony_sim_get_mcc_n(void)
{
        int ret = SIM_ERROR_NONE;

        //  Invalid parameter test
        ret = sim_get_mcc(NULL);
        dts_check_eq("sim_get_mcc", ret, SIM_ERROR_INVALID_PARAMETER, "Must return SIM_ERROR_INVALID_PARAMETER in case of invalid parameter"); 
}


/**
 * @brief Positive test case of sim_get_mcc()
 */
static void utc_telephony_sim_get_mcc_p(void)
{
        char* mcc = NULL; 
        int ret = SIM_ERROR_NONE;

        //  Normal operation test
        ret = sim_get_mcc(&mcc);
        if( ret == SIM_ERROR_NONE )
                free(mcc);
        dts_check_eq("sim_get_mcc", ret, SIM_ERROR_NONE, "Failed to get MCC"); 
}


/**
 * @brief Negative test case of sim_get_mnc()
 */
static void utc_telephony_sim_get_mnc_n(void)
{
        int ret = SIM_ERROR_NONE;

        //  Invalid parameter test 
        ret = sim_get_mnc(NULL);
        dts_check_eq("sim_get_mnc", ret, SIM_ERROR_INVALID_PARAMETER, "Must return SIM_ERROR_INVALID_PARAMETER in case of invalid parameter"); 
}


/**
 * @brief Positive test case of sim_get_mnc()
 */
static void utc_telephony_sim_get_mnc_p(void)
{
        char* mnc = NULL; 
        int ret = SIM_ERROR_NONE;

        //  Normal operation test
        ret = sim_get_mnc(&mnc);
        if( ret == SIM_ERROR_NONE )
                free(mnc);
        dts_check_eq("sim_get_mnc", ret, SIM_ERROR_NONE, "Failed to get MNC"); 
}


/**
 * @brief Negative test case of sim_get_msin()
 */
static void utc_telephony_sim_get_msin_n(void)
{
        int ret = SIM_ERROR_NONE;

        //  Invalid parameter test
        ret = sim_get_msin(NULL);
        dts_check_eq("sim_get_msin", ret, SIM_ERROR_INVALID_PARAMETER, "Must return SIM_ERROR_INVALID_PARAMETER in case of invalid parameter");
}


/**
 * @brief Positive test case of sim_get_msin()
 */
static void utc_telephony_sim_get_msin_p(void)
{
        char* msin = NULL;
        int ret = SIM_ERROR_NONE;

        //  Normal operation test
        ret = sim_get_msin(&msin);
        if( ret == SIM_ERROR_NONE )
                free(msin);
        dts_check_eq("sim_get_msin", ret, SIM_ERROR_NONE, "Failed to get MSIN");
}


/**
 * @brief Negative test case of sim_get_spn()
 */
static void utc_telephony_sim_get_spn_n(void)
{
        int ret = SIM_ERROR_NONE;

        //  Invalid parameter test    
        ret = sim_get_spn(NULL);
        dts_check_eq("sim_get_spn", ret, SIM_ERROR_INVALID_PARAMETER, "Must return SIM_ERROR_INVALID_PARAMETER in case of invalid parameter"); 
}


/**
 * @brief Positive test case of sim_get_spn()
 */
static void utc_telephony_sim_get_spn_p(void)
{
        char* spn = NULL; 
        int ret = SIM_ERROR_NONE;

        //  Normal operation test
        ret = sim_get_spn(&spn);
        if( ret == SIM_ERROR_NONE )
                free(spn);
        dts_check_eq("sim_get_spn", ret, SIM_ERROR_NONE, "Failed to get SPN");
}


/**
 * @brief Negative test case of sim_get_cphs_operator_name()
 */
static void utc_telephony_sim_get_cphs_operator_name_n(void)
{
        int ret = SIM_ERROR_NONE;

        //  Invalid parameter test
        ret = sim_get_cphs_operator_name(NULL, NULL);
        dts_check_eq("sim_get_cphs_operator_name", ret, SIM_ERROR_INVALID_PARAMETER, "Must return SIM_ERROR_INVALID_PARAMETER in case of invalid parameter");
}


/**
 * @brief Positive test case of sim_get_cphs_operator_name()
 */
static void utc_telephony_sim_get_cphs_operator_name_p(void)
{
        char* full_name = NULL;
        char* short_name = NULL;
        int ret = SIM_ERROR_NONE;

        //  Normal operation test
        ret = sim_get_cphs_operator_name(&full_name, &short_name);
        if( ret == SIM_ERROR_NONE ) {
                free(full_name);
                free(short_name);
        }
        dts_check_eq("sim_get_cphs_operator_name", ret, SIM_ERROR_NONE, "Failed to get CPHS full/short operator name");
}


/**
 * @brief Negative test case of sim_get_state()
 */
static void utc_telephony_sim_get_state_n(void)
{
        int ret = SIM_ERROR_NONE;

        //  Invalid parameter test    
        ret = sim_get_state(NULL);
        dts_check_eq("sim_get_state", ret, SIM_ERROR_INVALID_PARAMETER, "Must return SIM_ERROR_INVALID_PARAMETER in case of invalid parameter"); 
}


/**
 * @brief Positive test case of sim_get_state()
 */
static void utc_telephony_sim_get_state_p(void)
{
        sim_state_e sim_status; 
        int ret = SIM_ERROR_NONE;

        //  Normal operation test
        ret = sim_get_state(&sim_status);
        dts_check_eq("sim_get_state", ret, SIM_ERROR_NONE, "Failed to get the current status of SIM");
}


/**
 * @brief Negative test case of sim_get_subscriber_number()
 */
static void utc_telephony_sim_get_subscriber_number_n(void)
{
        int ret = SIM_ERROR_NONE;

        // Invalid parameter test
        ret = sim_get_subscriber_number(NULL);
        dts_check_eq("sim_get_subscriber_number", ret, SIM_ERROR_INVALID_PARAMETER, "Must return SIM_ERROR_INVALID_PARAMETER in case of invalid parameter"); 
}


/**
 * @brief Positive test case of sim_get_subscriber_number()
 */
static void utc_telephony_sim_get_subscriber_number_p(void)
{
        char* subscriber_number = NULL;
        int ret = SIM_ERROR_NONE;

        //  Normal operation test
        ret = sim_get_subscriber_number(&subscriber_number);
        if( ret == SIM_ERROR_NONE )
        {
			free(subscriber_number);
			dts_pass("sim_get_subscriber_number", "Succeeded to get subscriber number.");
        }
		else
		{
			dts_fail("sim_get_subscriber_number", "Failed to get subscriber number.");
		}
}


static void utc_telephony_sim_set_state_changed_cb_p(void)
{
    sim_error_e ret = sim_set_state_changed_cb(sim_state_changed, NULL);
    dts_check_eq("sim_set_state_changed_cb", ret, SIM_ERROR_NONE, 
		"sim_set_state_changed_cb failed.");
}

static void utc_telephony_sim_set_state_changed_cb_n(void)
{
    sim_error_e ret = sim_set_state_changed_cb(NULL, NULL);
    dts_check_eq("sim_set_state_changed_cb", ret, SIM_ERROR_INVALID_PARAMETER,  
		"SIM_ERROR_INVALID_PARAMETER must be returned when parameter is NULL.");
}

static void utc_telephony_sim_unset_state_changed_cb_p(void)
{
    sim_error_e ret = sim_unset_state_changed_cb();
    dts_check_eq("sim_unset_state_changed_cb", ret, SIM_ERROR_NONE, 
		"sim_unset_state_changed_cb failed.");
}


