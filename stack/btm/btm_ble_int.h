/******************************************************************************
 *
 *  Copyright (C) 1999-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/******************************************************************************
 *
 *  this file contains the main Bluetooth Manager (BTM) internal
 *  definitions.
 *
 ******************************************************************************/

#ifndef BTM_BLE_INT_H
#define BTM_BLE_INT_H

#include "bt_target.h"
#include "gki.h"
#include "hcidefs.h"
#include "btm_ble_api.h"
#include "btm_int.h"

#if BLE_INCLUDED == TRUE && SMP_INCLUDED == TRUE
#include "smp_api.h"
#endif


/* scanning enable status */
#define BTM_BLE_SCAN_ENABLE      0x01
#define BTM_BLE_SCAN_DISABLE     0x00

/* advertising enable status */
#define BTM_BLE_ADV_ENABLE     0x01
#define BTM_BLE_ADV_DISABLE    0x00

/* use the high 4 bits unused by inquiry mode */
#define BTM_BLE_SELECT_SCAN     0x20
#define BTM_BLE_NAME_REQUEST    0x40
#define BTM_BLE_OBSERVE         0x80

#define BTM_BLE_MAX_WL_ENTRY        1
#define BTM_BLE_AD_DATA_LEN         31

#define BTM_BLE_ENC_MASK    0x03

#define BTM_BLE_DUPLICATE_ENABLE        1
#define BTM_BLE_DUPLICATE_DISABLE       0

#define BTM_BLE_GAP_DISC_SCAN_INT   18         /* Interval(scan_int) = 11.25 ms= 0x0010 * 0.625 ms */
#define BTM_BLE_GAP_DISC_SCAN_WIN   18         /* scan_window = 11.25 ms= 0x0010 * 0.625 ms */
#define BTM_BLE_GAP_ADV_INT         512         /* Tgap(gen_disc) = 1.28 s= 512 * 0.625 ms */
#define BTM_BLE_GAP_LIM_TOUT        30          /* Tgap(lim_timeout) = 30.72 s max, round down to 30 */


#define BTM_BLE_SEC_REQ_ACT_NONE           0
#define BTM_BLE_SEC_REQ_ACT_ENCRYPT        1 /* encrypt the link using current key or key refresh */
#define BTM_BLE_SEC_REQ_ACT_PAIR           2
#define BTM_BLE_SEC_REQ_ACT_DISCARD        3 /* discard the sec request while encryption is started but not completed */
typedef UINT8   tBTM_BLE_SEC_REQ_ACT;

#define BLE_STATIC_PRIVATE_MSB_MASK          0x3f
#define BLE_RESOLVE_ADDR_MSB                 0x40   /*  most significant bit, bit7, bit6 is 01 to be resolvable random */
#define BLE_RESOLVE_ADDR_MASK                0xc0   /* bit 6, and bit7 */
#define BTM_BLE_IS_RESOLVE_BDA(x)           ((x[0] & BLE_RESOLVE_ADDR_MASK) == BLE_RESOLVE_ADDR_MSB)

typedef struct
{
    UINT16              data_mask;
    UINT8               *p_flags;
    UINT8               ad_data[BTM_BLE_AD_DATA_LEN];
    UINT8               *p_pad;
}tBTM_BLE_LOCAL_ADV_DATA;

typedef struct
{
    UINT32          inq_count;          /* Used for determining if a response has already been      */
                                        /* received for the current inquiry operation. (We do not   */
                                        /* want to flood the caller with multiple responses from    */
                                        /* the same device.                                         */
    BOOLEAN         scan_rsp;
    tBLE_BD_ADDR    le_bda;
} tINQ_LE_BDADDR;

#define BTM_BLE_ADV_DATA_LEN_MAX        31
#define BTM_BLE_CACHE_ADV_DATA_MAX      62

#define BTM_BLE_VALID_PRAM(x, min, max)  (((x) >= (min) && (x) <= (max)) || ((x) == BTM_BLE_CONN_PARAM_UNDEF))

typedef struct
{

    UINT16           discoverable_mode;
    UINT16           connectable_mode;
    UINT16           br_edr_supported_flag;  /* combined BR EDR discoverable and connectable mode */
                                             /* only meaningful when it is zero. This means
                                                BR EDR is not supported*/
    UINT8            proc_mode;        /* current procedure mode : inquiry or discovery */

    UINT16           scan_window;
    UINT16           scan_interval;
    UINT8            scan_type;        /* current scan type: active or passive */
    UINT16           adv_interval_min;
    UINT16           adv_interval_max;
    tBTM_BLE_AFP     afp;               /* advertising filter policy */
    tBTM_BLE_SFP     sfp;               /* scanning filter policy */

    tBLE_ADDR_TYPE   adv_addr_type;
    UINT8            evt_type;
    UINT8            adv_mode;
    tBLE_BD_ADDR     direct_bda;
    BOOLEAN          directed_conn;

    UINT8            adv_len;
    UINT8            adv_data_cache[BTM_BLE_CACHE_ADV_DATA_MAX];

    /* inquiry BD addr database */
    UINT8               num_bd_entries;
    UINT8               max_bd_entries;


    tBTM_BLE_LOCAL_ADV_DATA   adv_data;
    tBTM_BLE_ADV_CHNL_MAP     adv_chnl_map;

    TIMER_LIST_ENT   inq_timer_ent;
    BOOLEAN          scan_rsp;
    UINT8            state;             /* Current state that the inquiry process is in */
    INT8             tx_power;
} tBTM_BLE_INQ_CB;


/* random address resolving complete callback */
typedef void (tBTM_BLE_RESOLVE_CBACK) (void * match_rec, void *p);

typedef void (tBTM_BLE_ADDR_CBACK) (BD_ADDR_PTR static_random, void *p);

/* random address management control block */
typedef struct
{
    tBLE_ADDR_TYPE	            own_addr_type;         /* local device LE address type */
    BD_ADDR			            private_addr;
    BD_ADDR                     random_bda;
    BOOLEAN                     busy;
    UINT16                       index;
    tBTM_BLE_RESOLVE_CBACK      *p_resolve_cback;
    tBTM_BLE_ADDR_CBACK         *p_generate_cback;
    void                        *p;
    TIMER_LIST_ENT              raddr_timer_ent;
} tBTM_LE_RANDOM_CB;

#define BTM_BLE_MAX_BG_CONN_DEV_NUM    10

typedef struct
{
    UINT16              min_conn_int;
    UINT16              max_conn_int;
    UINT16              slave_latency;
    UINT16              supervision_tout;

}tBTM_LE_CONN_PRAMS;


typedef struct
{
    BD_ADDR     bd_addr;
    UINT8       attr;
    BOOLEAN     is_connected;
    BOOLEAN     in_use;
}tBTM_LE_BG_CONN_DEV;

  /* white list using state as a bit mask */
#define BTM_BLE_WL_IDLE         0
#define BTM_BLE_WL_INIT         1
#define BTM_BLE_WL_SCAN         2
#define BTM_BLE_WL_ADV          4
typedef UINT8 tBTM_BLE_WL_STATE;

/* BLE connection state */
#define BLE_CONN_IDLE    0
#define BLE_DIR_CONN     1
#define BLE_BG_CONN      2
typedef UINT8 tBTM_BLE_CONN_ST;

typedef struct
{
    void    *p_param;
}tBTM_BLE_CONN_REQ;
/* Define BLE Device Management control structure
*/
typedef struct
{
    /*****************************************************
    **      BLE Inquiry
    *****************************************************/
    tBTM_BLE_INQ_CB     inq_var;

    /* background connection procedure cb value */
    tBTM_BLE_CONN_TYPE  bg_conn_type;
    UINT16              scan_int;
    UINT16              scan_win;
    tBTM_BLE_SEL_CBACK  *p_select_cback;
    TIMER_LIST_ENT      scan_param_idle_timer;

    /* white list information */
    UINT8                   num_empty_filter;      /* Number of entries in white list */
    UINT8                   max_filter_entries;    /* Maximum number of entries that can be stored */
    tBTM_BLE_WL_STATE       wl_state;
    UINT8                   bg_dev_num;
    tBTM_LE_BG_CONN_DEV     bg_dev_list[BTM_BLE_MAX_BG_CONN_DEV_NUM];

    BUFFER_Q                conn_pending_q;
    tBTM_BLE_CONN_ST        conn_state;

    /* random address management control block */
    tBTM_LE_RANDOM_CB   addr_mgnt_cb;

    BOOLEAN          enabled;

#ifdef BTM_BLE_PC_ADV_TEST_MODE
    tBTM_BLE_SCAN_REQ_CBACK *p_scan_req_cback;
#endif

    BOOLEAN         scatternet_enable;
} tBTM_BLE_CB;

#ifdef __cplusplus
extern "C" {
#endif

extern void btm_ble_timeout(TIMER_LIST_ENT *p_tle);
extern void btm_ble_process_adv_pkt (UINT8 *p);
extern void btm_ble_proc_scan_rsp_rpt (UINT8 *p);
extern tBTM_STATUS btm_ble_read_remote_name(BD_ADDR remote_bda, tBTM_INQ_INFO *p_cur, tBTM_CMPL_CB *p_cb);
extern BOOLEAN btm_ble_cancel_remote_name(BD_ADDR remote_bda);

extern tBTM_STATUS btm_ble_set_discoverability(UINT16 combined_mode);
extern tBTM_STATUS btm_ble_set_connectability(UINT16 combined_mode);
extern tBTM_STATUS btm_ble_start_inquiry (UINT8 mode, UINT8   duration);
extern void btm_ble_dir_adv_tout(void);

extern void btm_ble_stop_scan(void);
extern void btm_ble_att_db_init(void);
extern void btm_ble_init (void);
extern void btm_ble_connected (UINT8 *bda, UINT16 handle, UINT8 enc_mode, UINT8 role, tBLE_ADDR_TYPE addr_type, BOOLEAN addr_matched);
extern void btm_ble_read_remote_features_complete(UINT8 *p);
extern void btm_ble_write_adv_enable_complete(UINT8 * p);
extern void btm_ble_conn_complete(UINT8 *p, UINT16 evt_len);
extern tBTM_BLE_CONN_ST btm_ble_get_conn_st(void);
extern void btm_ble_set_conn_st(tBTM_BLE_CONN_ST new_st);


/* LE security function from btm_sec.c */
#if SMP_INCLUDED == TRUE
extern void btm_ble_link_sec_check(BD_ADDR bd_addr, tBTM_LE_AUTH_REQ auth_req, tBTM_BLE_SEC_REQ_ACT *p_sec_req_act);
extern void btm_ble_ltk_request_reply(BD_ADDR bda,  BOOLEAN use_stk, BT_OCTET16 stk);
extern UINT8 btm_proc_smp_cback(tSMP_EVT event, BD_ADDR bd_addr, tSMP_EVT_DATA *p_data);
extern tBTM_STATUS btm_ble_set_encryption (BD_ADDR bd_addr, void *p_ref_data, UINT8 link_role);
extern void btm_ble_ltk_request(UINT16 handle, UINT8 rand[8], UINT16 ediv);
extern BOOLEAN btm_ble_start_encrypt(BD_ADDR bda, BOOLEAN use_stk, BT_OCTET16 stk);
extern void btm_ble_link_encrypted(BD_ADDR bd_addr, UINT8 encr_enable);
#endif

/* LE device management functions */
extern void btm_ble_reset_id( void );

/* security related functions */
extern void btm_ble_increment_sign_ctr(BD_ADDR bd_addr, BOOLEAN is_local );
extern BOOLEAN btm_get_local_div (BD_ADDR bd_addr, UINT16 *p_div);
extern BOOLEAN btm_ble_check_link_type (BD_ADDR bd_addr);
extern BOOLEAN btm_ble_get_enc_key_type(BD_ADDR bd_addr, UINT8 *p_key_types);

extern void btm_ble_rand_enc_complete (UINT8 *p, UINT16 op_code, tBTM_RAND_ENC_CB *p_enc_cplt_cback);
extern void btm_sec_save_le_key(BD_ADDR bd_addr, tBTM_LE_KEY_TYPE key_type, tBTM_LE_KEY_VALUE *p_keys, BOOLEAN pass_to_application);
extern void btm_ble_update_sec_key_size(BD_ADDR bd_addr, UINT8 enc_key_size);
extern UINT8 btm_ble_read_sec_key_size(BD_ADDR bd_addr);

/* white list function */
extern BOOLEAN btm_update_dev_to_white_list(BOOLEAN to_add, BD_ADDR bd_addr,UINT8 wl_type);
extern BOOLEAN btm_update_bg_conn_list(BOOLEAN to_add, BD_ADDR bd_addr, UINT8 *p_attr_tag);
extern void btm_update_scanner_filter_policy(tBTM_BLE_SFP scan_policy);
extern void btm_update_adv_filter_policy(tBTM_BLE_AFP adv_policy);
extern void btm_ble_clear_white_list (void);

/* background connection function */
extern void btm_ble_suspend_bg_conn(void);
extern BOOLEAN btm_ble_resume_bg_conn(void);
extern void btm_ble_initiate_select_conn(BD_ADDR bda);
extern BOOLEAN btm_ble_start_auto_conn(BOOLEAN start);
extern BOOLEAN btm_ble_start_select_conn(BOOLEAN start,tBTM_BLE_SEL_CBACK   *p_select_cback);
extern BOOLEAN btm_ble_renew_bg_conn_params(BOOLEAN add, BD_ADDR bd_addr);
extern UINT8 btm_ble_count_unconn_dev_in_whitelist(void);
extern void btm_write_dir_conn_wl(BD_ADDR target_addr);
extern void btm_ble_update_mode_operation(UINT8 link_role, BD_ADDR bda, BOOLEAN conn_ccancel);

/* direct connection utility */
extern BOOLEAN btm_send_pending_direct_conn(void);
extern void btm_ble_enqueue_direct_conn_req(void *p_param);

/* BLE address management */
extern void btm_gen_resolvable_private_addr (void);
extern void btm_gen_non_resolvable_private_addr (tBTM_BLE_ADDR_CBACK *p_cback, void *p);
extern void btm_ble_resolve_random_addr(BD_ADDR random_bda, tBTM_BLE_RESOLVE_CBACK * p_cback, void *p);
extern void btm_ble_update_reconnect_address(BD_ADDR bd_addr);

#if BTM_BLE_CONFORMANCE_TESTING == TRUE
BT_API extern void btm_ble_set_no_disc_if_pair_fail (BOOLEAN disble_disc);
BT_API extern void btm_ble_set_test_mac_value (BOOLEAN enable, UINT8 *p_test_mac_val);
BT_API extern void btm_ble_set_test_local_sign_cntr_value(BOOLEAN enable, UINT32 test_local_sign_cntr);
BT_API extern void btm_set_random_address(BD_ADDR random_bda);
#endif


#ifdef __cplusplus
}
#endif

#endif
