export const FlashStateType = {
    INITIALIZING: "initializing",
    PREPARING: "preparing",
    ERASING: "erasing",
    WRITING: "writing",
    FINISHED: "finished",
    ERROR: "error",
}
export const FlashError = {
    FAILED_INITIALIZING: "failed_initialize",
    FAILED_MANIFEST_FETCH: "fetch_manifest_failed",
    NOT_SUPPORTED: "not_supported",
    FAILED_FIRMWARE_DOWNLOAD: "failed_firmware_download",
    WRITE_FAILED: "write_failed",
}