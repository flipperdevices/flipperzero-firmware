let record = ({
    open: ffi("void* furi_record_open(char *)"),
    close: ffi("void furi_record_close(char *)"),
});

let notification = ({
    name: "notification",
    message: ffi("void notification_message(void*, void*)"),
    success: ffi_address("sequence_success"),
});

let notification_app = record.open(notification.name);
notification.message(notification_app, notification.success);
record.close(notification.name);