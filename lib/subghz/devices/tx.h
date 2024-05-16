#pragma once

/** TX Allowed or Blocked Reasons */
typedef enum {
    SubGhzTxAllowed, /**< TX is allowed with this configuration */
    SubGhzTxBlockedRegionNotProvisioned, /**< Region not provisioned and not bypassed */
    SubGhzTxBlockedRegion, /**< Outside region, needs bypass region */
    SubGhzTxBlockedDefault, /**< Outisde default, needs extended range */
    SubGhzTxUnsupported, /**< No configuration would allow this */
} SubGhzTx;
