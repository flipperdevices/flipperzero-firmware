
let RECORD = ({
    open: function (api) {
        let ret = Object.create(this.prototype);
        ret.name = api.name;
        ret.ptr = ret.api.open(ret.name);
        return ret;
    },
    prototype: {
        api: {
            open: ffi("void* furi_record_open(char *)"),
            close: ffi("void furi_record_close(char *)"),
        },
        close: function () {
            this.api.close(this.name);
        }
    }
});

let GUI = ({
    name: "gui",
    align: {
        left: 0,
        right: 1,
        top: 2,
        bottom: 3,
        center: 4,
    },
});

let DIALOGS = ({
    name: "dialogs",
    show: function (record, message) {
        return ffi("int dialog_message_show(void*, void*)")(record.ptr, message.ptr);
    },
});

let DIALOG_MESSAGE = ({
    alloc: function () {
        let ret = Object.create(this.prototype);
        ret.ptr = ret.api.alloc();
        return ret;
    },
    button_type: {
        back: 0,
        left: 1,
        center: 2,
        right: 3,
    },
    prototype: {
        api: {
            alloc: ffi("void* dialog_message_alloc()"),
            free: ffi("void dialog_message_free(void*)"),
            set_header: ffi("void dialog_message_set_header(void*, char*, int, int, int, int)"),
            set_text: ffi("void dialog_message_set_text(void*, char*, int, int, int, int)"),
            set_buttons: ffi("void dialog_message_set_buttons(void*, char*, char*, char*)"),
            set_icon: ffi("void dialog_message_set_icon(void*, void*, int, int)"),
        },
        free: function () {
            this.api.free(this.ptr);
        },
        set_header: function (text, x, y, align_x, align_y) {
            this.api.set_header(this.ptr, text, x, y, align_x, align_y);
        },
        set_text: function (text, x, y, align_x, align_y) {
            this.api.set_text(this.ptr, text, x, y, align_x, align_y);
        },
        set_buttons: function (text_left, text_center, text_right) {
            this.api.set_buttons(this.ptr, text_left, text_center, text_right);
        },
        set_icon: function (icon, x, y) {
            this.api.set_icon(this.ptr, icon, x, y);
        },
        set_icon_from_fw: function (icon_name, x, y) {
            this.api.set_icon(this.ptr, ffi_address(icon_name), x, y);
        },
    }
});

let VIEW_DISPATCHER = ({
    alloc: function (record, type) {
        let ret = Object.create(this.prototype);
        ret.ptr = ret.api.alloc();
        ret.api.attach_to_gui(ret.ptr, record.ptr, type);
        return ret;
    },
    type: {
        desktop: 0,
        window: 1,
        fullscreen: 2,
    },
    prototype: {
        api: {
            alloc: ffi("void* view_dispatcher_alloc()"),
            free: ffi("void view_dispatcher_free(void*)"),
            add_view: ffi("void view_dispatcher_add_view(void*, int, void*)"),
            remove_view: ffi("void view_dispatcher_remove_view(void*, int)"),
            attach_to_gui: ffi("void view_dispatcher_attach_to_gui(void*, void*, int)"),
            switch_to_view: ffi("void view_dispatcher_switch_to_view(void*, int)"),
        },
        free: function () {
            this.api.free(this.ptr);
        },
        add_view: function (view) {
            this.api.add_view(this.ptr, view.index, view.get_view());
        },
        remove_view: function (view) {
            this.api.remove_view(this.ptr, view.index);
        },
        switch_to_view: function (view) {
            this.api.switch_to_view(this.ptr, view.index);
        },
    }
});

let EMPTY_SCREEN = ({
    alloc: function (index) {
        let ret = Object.create(this.prototype);
        ret.ptr = ret.api.alloc();
        ret.index = index;
        return ret;
    },
    prototype: {
        api: {
            alloc: ffi("void* empty_screen_alloc()"),
            free: ffi("void empty_screen_free(void*)"),
            get_view: ffi("void* empty_screen_get_view(void*)"),
        },
        free: function () {
            this.api.free(this.ptr);
        },
        get_view: function () {
            return this.api.get_view(this.ptr);
        },
    }
});

let STD = ({
    malloc: ffi('void *malloc(int)'),
    free: ffi('void free(void*)'),
});

let C_STR = ({
    create: function (str) {
        let len = str.length;
        let ret = Object.create(this.prototype);
        ret.mem = STD.malloc(len + 1);
        ret.text = mkstr(ret.mem, len);
        for (let i = 0; i < len; i++) {
            ret.mem[i] = str.at(i);
        }
        return ret;
    },
    prototype: {
        free: function () {
            STD.free(this.mem);
        },
        set: function (str) {
            let len = str.length;
            STD.free(this.mem);
            this.mem = STD.malloc(len + 1);
            this.text = mkstr(this.mem, len);
            for (let i = 0; i < len; i++) {
                this.mem[i] = str.at(i);
            }
        }
    }
});

let FURI_HAL_VERSION = ({
    get_name: ffi("char *furi_hal_version_get_name_ptr()"),
    get_hw_version: ffi("int furi_hal_version_get_hw_version()"),
    get_hw_target: ffi("int furi_hal_version_get_hw_target()"),
    get_hw_body: ffi("int furi_hal_version_get_hw_body()"),
    get_hw_connect: ffi("int furi_hal_version_get_hw_connect()"),
    get_hw_region_name: ffi("char* furi_hal_version_get_hw_region_name()"),
    get_region_name: ffi("char* furi_hal_region_get_name()"),
    version_uid: ffi("char* furi_hal_version_uid()"),
    version_uid_size: ffi("int furi_hal_version_uid_size()"),

    get_firmware_version: ffi("void* furi_hal_version_get_firmware_version()"),
    get_version: ffi("char* version_get_version(void*)"),
    get_builddate: ffi("char* version_get_builddate(void*)"),
    get_dirty_flag: ffi("int version_get_dirty_flag(void*)"),
    get_githash: ffi("char* version_get_githash(void*)"),
    get_gitbranchnum: ffi("char* version_get_gitbranchnum(void*)"),
    get_target: ffi("int version_get_target(void*)"),
    get_gitbranch: ffi("char* version_get_gitbranch(void*)"),
});

// init c-string
let text_back = C_STR.create("Back");
let text_next = C_STR.create("Next");
let text_header = C_STR.create("");
let text_body = C_STR.create("");

// open records
let dialogs = RECORD.open(DIALOGS);
let gui = RECORD.open(GUI);

// create objects
let message = DIALOG_MESSAGE.alloc();
let view_dispatcher = VIEW_DISPATCHER.alloc(gui, VIEW_DISPATCHER.type.fullscreen);
let empty_screen = EMPTY_SCREEN.alloc(0);

// configure objects
view_dispatcher.add_view(empty_screen);
view_dispatcher.switch_to_view(empty_screen);

// init result
let screen_index = 0;
let screen_result = DIALOG_MESSAGE.button_type.right;

let screens = [
    function () {
        text_header.set("Product: Flipper Zero\nModel: FZ.1\n");
        text_body.set("FCC ID: 2A2V6-FZ\nIC: 27624-FZ");
        message.set_header(text_header.text, 0, 0, GUI.align.left, GUI.align.top);
        message.set_text(text_body.text, 0, 26, GUI.align.left, GUI.align.top);
        let result = DIALOGS.show(dialogs, message);
        message.set_header(null, 0, 0, GUI.align.left, GUI.align.top);
        message.set_text(null, 0, 0, GUI.align.left, GUI.align.top);
        return result;
    },
    function () {
        text_body.set("Flipper Devices Inc\nSuite B #551, 2803\nPhiladelphia Pike, Claymont\nDE, USA 19703\n");
        message.set_text(text_body.text, 0, 0, GUI.align.left, GUI.align.top);
        let result = DIALOGS.show(dialogs, message);
        message.set_text(null, 0, 0, GUI.align.left, GUI.align.top);
        return result;
    },
    function () {
        text_body.set("For all compliance\ncertificates please visit:\nwww.flipp.dev/compliance");
        message.set_text(text_body.text, 0, 0, GUI.align.left, GUI.align.top);
        let result = DIALOGS.show(dialogs, message);
        message.set_text(null, 0, 0, GUI.align.left, GUI.align.top);
        return result;
    },
    function () {
        message.set_icon_from_fw("I_Certification1_103x56", 13, 0);
        let result = DIALOGS.show(dialogs, message);
        message.set_icon(null, 0, 0);
        return result;
    },
    function () {
        message.set_icon_from_fw("I_Certification2_46x33", 15, 10);
        let result = DIALOGS.show(dialogs, message);
        message.set_icon(null, 0, 0);
        return result;
    },
    function () {
        let my_name = FURI_HAL_VERSION.get_name();
        if (my_name === null) {
            my_name = "Unknown";
        }
        let str = to_string(FURI_HAL_VERSION.get_hw_version()) + ".";
        str += "F" + to_string(FURI_HAL_VERSION.get_hw_target());
        str += "B" + to_string(FURI_HAL_VERSION.get_hw_body());
        str += "C" + to_string(FURI_HAL_VERSION.get_hw_connect());
        str += " " + FURI_HAL_VERSION.get_hw_region_name() + ":" + FURI_HAL_VERSION.get_region_name();
        str += " " + my_name + "\n";
        str += "Serial Number:\n";


        let uid = FURI_HAL_VERSION.version_uid();
        let uid_size = FURI_HAL_VERSION.version_uid_size();
        for (let i = 0; i < uid_size; i++) {
            str += to_hex_string(uid.at(i));
        }

        text_header.set("HW Version Info:");
        text_body.set(str);
        message.set_header(text_header.text, 0, 0, GUI.align.left, GUI.align.top);
        message.set_text(text_body.text, 0, 13, GUI.align.left, GUI.align.top);
        let result = DIALOGS.show(dialogs, message);
        message.set_header(null, 0, 0, GUI.align.left, GUI.align.top);
        message.set_text(null, 0, 0, GUI.align.left, GUI.align.top);
        return result;
    },
    function () {
        let ver = FURI_HAL_VERSION.get_firmware_version();
        let str = "";

        if (ver === null) {
            str = "No info\n";
        } else {
            str = FURI_HAL_VERSION.get_version(ver) + " [";
            str += FURI_HAL_VERSION.get_builddate(ver) + "]\n";
            if (FURI_HAL_VERSION.get_dirty_flag(ver) > 0) {
                str += "[!]";
            }
            str += FURI_HAL_VERSION.get_githash(ver) + " [";
            str += FURI_HAL_VERSION.get_gitbranchnum(ver) + "]\n";

            str += "[" + to_string(FURI_HAL_VERSION.get_target(ver)) + "] ";
            str += FURI_HAL_VERSION.get_gitbranch(ver);
        }

        text_header.set("FW Version Info:");
        text_body.set(str);
        message.set_header(text_header.text, 0, 0, GUI.align.left, GUI.align.top);
        message.set_text(text_body.text, 0, 13, GUI.align.left, GUI.align.top);
        let result = DIALOGS.show(dialogs, message);
        message.set_header(null, 0, 0, GUI.align.left, GUI.align.top);
        message.set_text(null, 0, 0, GUI.align.left, GUI.align.top);
        return result;
    },
];

// app body
while (screen_result !== DIALOG_MESSAGE.button_type.back) {
    if (screen_index >= screens.length - 1) {
        message.set_buttons(text_back.text, null, null);
    } else {
        message.set_buttons(text_back.text, null, text_next.text);
    }

    screen_result = screens[screen_index]();

    if (screen_result === DIALOG_MESSAGE.button_type.left) {
        if (screen_index <= 0) {
            break;
        } else {
            screen_index--;
        }
    } else if (screen_result === DIALOG_MESSAGE.button_type.right) {
        if (screen_index < screens.length) {
            screen_index++;
        }
    } else if (screen_result === DIALOG_MESSAGE.button_type.back) {
        break;
    }
}

// free c-string
message.free();

// free objects
view_dispatcher.remove_view(empty_screen);
view_dispatcher.free();
empty_screen.free();

text_back.free();
text_next.free();
text_header.free();
text_body.free();

// close records
dialogs.close();
gui.close();