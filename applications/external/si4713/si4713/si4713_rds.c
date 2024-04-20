#include <stdlib.h>

#include "si4713_rds.h"
#include "si4713_constants.h"

void si4713_begin_rds(SI4713Device* device, uint16_t program_id) {
    si4713_set_property(device, SI4713_PROP_TX_RDS_DEVIATION, 200);
    si4713_set_property(device, SI4713_PROP_TX_RDS_INTERRUPT_SOURCE, 0x0001);
    si4713_set_property(device, SI4713_PROP_TX_RDS_PI, program_id);
    si4713_set_property(device, SI4713_PROP_TX_RDS_PS_MIX, 0x03);
    si4713_set_property(device, SI4713_PROP_TX_RDS_PS_MISC, 0x1008);
    si4713_set_property(device, SI4713_PROP_TX_RDS_PS_REPEAT_COUNT, 3);
    si4713_set_property(device, SI4713_PROP_TX_RDS_MESSAGE_COUNT, 1);
    si4713_set_property(device, SI4713_PROP_TX_RDS_PS_AF, 0xE0E0);
    si4713_set_property(device, SI4713_PROP_TX_RDS_FIFO_SIZE, 0);
    si4713_set_property(device, SI4713_PROP_TX_COMPONENT_ENABLE, 0x0007);
}
