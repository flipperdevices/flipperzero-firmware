#include <furi.h>
#include <cli/cli.h>
#include <args.h>
#include <storage/storage.h>
#include <storage/storage-sd-api.h>
#include <api-hal-version.h>

#define MAX_NAME_LENGTH 64

void storage_cli(Cli* cli, string_t args, void* context);

// app cli function
void storage_cli_init() {
    Cli* cli = furi_record_open("cli");
    cli_add_command(cli, "storage", storage_cli, NULL);
    furi_record_close("cli");
}

void storage_cli_print_usage() {
    printf("Usage:\r\n");
    printf("storage <cmd> <path> <args>\r\n");
    printf("The path must start with /int or /ext\r\n");
    printf("Cmd list:\r\n");
    printf("\tinfo\t - get FS info\r\n");
    printf("\tformat\t - format filesystem\r\n");
    printf("\tlist\t - list files and dirs\r\n");
};

void storage_cli_print_error(string_t path, FS_Error error) {
    printf(
        "Storage error for path \"%s\": %s\r\n",
        string_get_cstr(path),
        storage_error_get_desc(error));
}

void storage_cli_print_file_error(string_t path, File* file) {
    printf(
        "Storage error for path \"%s\": %s\r\n",
        string_get_cstr(path),
        storage_file_get_error_desc(file));
}

void storage_cli_info(Cli* cli, string_t path) {
    StorageApp* api = furi_record_open("storage");

    if(string_cmp_str(path, "/int") == 0) {
        uint64_t total_space;
        uint64_t free_space;
        FS_Error error = storage_common_fs_info(api, "/int", &total_space, &free_space);

        if(error != FSE_OK) {
            storage_cli_print_error(path, error);
        } else {
            printf(
                "Label: %s\r\nType: LittleFS\r\n%lu KB total\r\n%lu KB free\r\n",
                api_hal_version_get_name_ptr(),
                (uint32_t)(total_space / 1024),
                (uint32_t)(free_space / 1024));
        }
    } else if(string_cmp_str(path, "/ext") == 0) {
        SDInfo sd_info;
        FS_Error error = storage_sd_info(api, &sd_info);

        if(error != FSE_OK) {
            storage_cli_print_error(path, error);
        } else {
            printf(
                "Label: %s\r\nType: %s\r\n%lu KB total\r\n%lu KB free\r\n",
                sd_info.label,
                sd_api_get_fs_type_text(sd_info.fs_type),
                sd_info.kb_total,
                sd_info.kb_free);
        }
    } else {
        storage_cli_print_usage();
    }

    furi_record_close("storage");
};

void storage_cli_format(Cli* cli, string_t path) {
    if(string_cmp_str(path, "/int") == 0) {
        storage_cli_print_error(path, FSE_NOT_IMPLEMENTED);
    } else if(string_cmp_str(path, "/ext") == 0) {
        printf("Formatting SD card, all data will be lost. Are you sure (y/n)?\r\n");
        char answer = cli_getc(cli);
        if(answer == 'y' || answer == 'Y') {
            StorageApp* api = furi_record_open("storage");
            printf("Formatting, please wait...\r\n");

            FS_Error error = storage_sd_format(api);

            if(error != FSE_OK) {
                storage_cli_print_error(path, error);
            } else {
                printf("SD card was successfully formatted.\r\n");
            }
            furi_record_close("storage");
        } else {
            printf("Cancelled.\r\n");
        }
    } else {
        storage_cli_print_usage();
    }
};

void storage_cli_list(Cli* cli, string_t path) {
    StorageApp* api = furi_record_open("storage");
    File* file = storage_file();

    if(storage_dir_open(api, &file, string_get_cstr(path))) {
        FileInfo fileinfo;
        char name[MAX_NAME_LENGTH];
        bool readed = false;

        while(storage_dir_read(file, &fileinfo, name, MAX_NAME_LENGTH)) {
            readed = true;
            if(fileinfo.flags & FSF_DIRECTORY) {
                printf("\t[D] %s\r\n", name);
            } else {
                printf("\t[F] %s %lub\r\n", name, (uint32_t)(fileinfo.size));
            }
        }

        if(!readed) {
            printf("\tEmpty\r\n");
        }
    } else {
        storage_cli_print_file_error(path, file);
    }

    storage_dir_close(&file);
    furi_record_close("storage");
}

void storage_cli(Cli* cli, string_t args, void* context) {
    string_t cmd;
    string_t path;
    string_init(cmd);
    string_init(path);

    do {
        if(!args_read_string_and_trim(args, cmd)) {
            storage_cli_print_usage();
            break;
        }

        if(!args_read_string_and_trim(args, path)) {
            storage_cli_print_usage();
            break;
        }

        if(string_cmp_str(cmd, "info") == 0) {
            storage_cli_info(cli, path);
            break;
        }

        if(string_cmp_str(cmd, "format") == 0) {
            storage_cli_format(cli, path);
            break;
        }

        if(string_cmp_str(cmd, "list") == 0) {
            storage_cli_list(cli, path);
            break;
        }

        storage_cli_print_usage();
    } while(false);

    string_clear(path);
    string_clear(cmd);
}