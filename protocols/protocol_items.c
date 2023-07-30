#include "protocol_items.h"

const SubGhzProtocol* tpms_protocol_registry_items[] = {
    // &tpms_protocol_infactory,
    // &tpms_protocol_thermopro_tx4,
    // &tpms_protocol_nexus_th,
    // &tpms_protocol_gt_wt_02,
    // &tpms_protocol_gt_wt_03,
    // &tpms_protocol_acurite_606tx,
    // &tpms_protocol_acurite_609txc,
    // &tpms_protocol_lacrosse_tx,
    // &tpms_protocol_lacrosse_tx141thbv2,
    // &tpms_protocol_oregon2,
    // &tpms_protocol_oregon3,
    &tpms_protocol_abarth124,
    // &tpms_protocol_ambient_weather,
    // &tpms_protocol_auriol_th,
    // &tpms_protocol_oregon_v1,
    // &tpms_protocol_tx_8300,
    // &tpms_protocol_wendox_w6726,
};

const SubGhzProtocolRegistry tpms_protocol_registry = {
    .items = tpms_protocol_registry_items,
    .size = COUNT_OF(tpms_protocol_registry_items)};
