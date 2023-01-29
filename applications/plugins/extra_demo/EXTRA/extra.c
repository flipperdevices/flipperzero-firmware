// v1.1

// System libraries
#include <stdlib.h>

// Flipper Zero libraries
#include <furi.h>
#include <cli/cli.h>
#include <storage/storage.h>
#include <toolbox/path.h>
#include <cli/cli_i.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>
#include <bad_usb/bad_usb_script.h>

static FuriString* cwd;
static bool registered = false;
static bool initialized = false;

/*
   _____       _         _        _             
  / ____|     | |       | |      (_)            
 | (___  _   _| |__  ___| |_ _ __ _ _ __   __ _ 
  \___ \| | | | '_ \/ __| __| '__| | '_ \ / _` |
  ____) | |_| | |_) \__ \ |_| |  | | | | | (_| |
 |_____/ \__,_|_.__/|___/\__|_|  |_|_| |_|\__, |
                                           __/ |
                                          |___/ 
*/
static void furi_string_u_substr(FuriString* str, size_t start, size_t end) {
    FuriString* out = furi_string_alloc_set_str("");
    for(size_t i = start; i < end && i < furi_string_size(str); i++)
        furi_string_push_back(out, furi_string_get_char(str, i));
    furi_string_set(str, out);
    furi_string_free(out);
}

/*
  _____      _   _     
 |  __ \    | | | |    
 | |__) |_ _| |_| |__  
 |  ___/ _` | __| '_ \ 
 | |  | (_| | |_| | | |
 |_|   \__,_|\__|_| |_|
*/
static FuriString* path_parse(FuriString* path) {
    FuriString* parts[256]; // Max safe depth 256
    for(int i = 0; i < 256; i++) {
        parts[i] = furi_string_alloc_set_str("");
    }
    bool is_root;
    if(furi_string_cmp_str(path, "") == 0)
        is_root = false;
    else
        is_root = furi_string_get_char(path, 0) == '/';
    FuriString* part = furi_string_alloc_set_str("");
    size_t j = 0;
    furi_string_push_back(path, '/');
    for(size_t i = 0; i < furi_string_size(path); i++) {
        if(furi_string_get_char(path, i) == '/') {
            furi_string_set(parts[j], part);
            furi_string_set_str(part, "");
            j++;
        } else
            furi_string_push_back(part, furi_string_get_char(path, i));
    }
    furi_string_free(part);
    for(int i = 0; i < 256; i++) {
        if(furi_string_cmp_str(parts[i], "") == 0) continue;
        if(furi_string_cmp_str(parts[i], ".") == 0) furi_string_set_str(parts[i], "");
        if(furi_string_cmp_str(parts[i], "..") == 0) {
            furi_string_set_str(parts[i], "");
            if(i == 0) continue;
            int j = i - 1;
            while(j != -1) {
                if(furi_string_cmp_str(parts[j], "") != 0 &&
                   furi_string_cmp_str(parts[j], ".") != 0 &&
                   furi_string_cmp_str(parts[j], "..") != 0) {
                    furi_string_set_str(parts[j], "");
                    break;
                }
                j -= 1;
            }
        }
    }
    FuriString* res;
    if(is_root)
        res = furi_string_alloc_set_str("/");
    else
        res = furi_string_alloc_set_str("");
    bool flag = false;
    for(int i = 0; i < 256; i++) {
        if(furi_string_cmp_str(parts[i], "") == 0) continue;
        flag = true;
        furi_string_cat(res, parts[i]);
        furi_string_push_back(res, '/');
    }
    for(int i = 0; i < 256; i++) furi_string_free(parts[i]);
    if(furi_string_cmp_str(res, "") == 0)
        furi_string_set_str(res, "/");
    else if(flag)
        furi_string_set_char(res, furi_string_size(res) - 1, '\0');
    return res;
}
static FuriString* path_join(FuriString* path1, FuriString* path2) {
    if(furi_string_get_char(path2, 0) == '/') return path_parse(path2);
    FuriString* path = furi_string_alloc_set_str("");
    path_concat(furi_string_get_cstr(path1), furi_string_get_cstr(path2), path);
    return path_parse(path);
}
static FuriString* path_filename(FuriString* path) {
    FuriString* o = furi_string_alloc_set_str("");
    path_extract_filename(path, o, false);
    return o;
}
static void path_split(FuriString* src, FuriString* path1, FuriString* path2) {
    char search_for = ' ';
    size_t index = furi_string_search_char(src, search_for);
    furi_string_set(path1, src);
    furi_string_set(path2, src);

    if(index != 4294967295) furi_string_u_substr(path1, 0, index);
    if(index == 4294967295)
        furi_string_set_str(path2, "");
    else
        furi_string_u_substr(path2, index + 1, furi_string_size(src));

    furi_string_set(path1, path_parse(path1));
    furi_string_set(path2, path_parse(path2));
}

/*
   _____                                          _     
  / ____|                                        | |    
 | |     ___  _ __ ___  _ __ ___   __ _ _ __   __| |___ 
 | |    / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` / __|
 | |___| (_) | | | | | | | | | | | (_| | | | | (_| \__ \
  \_____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|___/
*/
static void extra_help_handler(Cli* cli, FuriString* args, void* context) {
    UNUSED(cli);
    UNUSED(context);

    bool empty = furi_string_cmp_str(args, "") == 0;
    if(empty) printf("Extra Commands help\r\n\r\n");
    if(empty || furi_string_cmp_str(args, "extra_help") == 0) printf("extra_help [command]\r\n");
    if(empty || furi_string_cmp_str(args, "extra_help") == 0)
        printf("Lists all commands and usages. Specify a command to only output its usage.\r\n");
    if(empty || furi_string_cmp_str(args, "cd") == 0) printf("cd [path]\r\n");
    if(empty || furi_string_cmp_str(args, "cd") == 0)
        printf(
            "Changes the current working directory to the specified direcotry. If no directory is specified, it changes it to /ext.\r\n");
    if(empty || furi_string_cmp_str(args, "cp") == 0) printf("cp <path1> <path2>\r\n");
    if(empty || furi_string_cmp_str(args, "cp") == 0)
        printf("Copies the specified file (<path1>) to the specified location (<path2>).\r\n");
    if(empty || furi_string_cmp_str(args, "echo") == 0) printf("echo <message>\r\n");
    if(empty || furi_string_cmp_str(args, "echo") == 0)
        printf("Prints the specified message.\r\n");
    if(empty || furi_string_cmp_str(args, "ls") == 0) printf("ls [path]\r\n");
    if(empty || furi_string_cmp_str(args, "ls") == 0)
        printf(
            "Lists all files and directories in the specified direcotry. If no directory is specified, it takes the current working directory.\r\n");
    if(empty || furi_string_cmp_str(args, "mkdir") == 0) printf("mkdir <path>\r\n");
    if(empty || furi_string_cmp_str(args, "mkdir") == 0)
        printf("Creates a directory at the specified path.\r\n");
    if(empty || furi_string_cmp_str(args, "mv") == 0) printf("mv <path1> <path2>\r\n");
    if(empty || furi_string_cmp_str(args, "mv") == 0)
        printf("Moves the specified file (<path1>) to a new location (<path2>).\r\n");
    if(empty || furi_string_cmp_str(args, "pwd") == 0) printf("pwd\r\n");
    if(empty || furi_string_cmp_str(args, "pwd") == 0)
        printf("Prints the current working directory.\r\n");
    if(empty || furi_string_cmp_str(args, "read") == 0) printf("read <path>\r\n");
    if(empty || furi_string_cmp_str(args, "read") == 0)
        printf("Read the contents of the specified file.\r\n");
    if(empty || furi_string_cmp_str(args, "rm") == 0) printf("rm <path>\r\n");
    if(empty || furi_string_cmp_str(args, "rm") == 0)
        printf("Deletes the file/directory (recursively) at the specified path.\r\n");
    if(empty || furi_string_cmp_str(args, "sequence") == 0) printf("sequence <sequence>\r\n");
    if(empty || furi_string_cmp_str(args, "sequence") == 0)
        printf("Plays a notification sequence.\r\n");
    if(empty || furi_string_cmp_str(args, "sequence") == 0)
        printf(
            "Takes sequence name (sequence defined as sequence_<sequence name> in notification/notification_messages.h) as the only argument.\r\n");
    if(empty || furi_string_cmp_str(args, "sleep") == 0) printf("sleep <time>\r\n");
    if(empty || furi_string_cmp_str(args, "sleep") == 0)
        printf("Freezes for <time> milliseconds.\r\n");
    if(empty || furi_string_cmp_str(args, "start") == 0) printf("start <path>\r\n");
    if(empty || furi_string_cmp_str(args, "start") == 0)
        printf("Execute the script at the specified path.\r\n");
    if(empty || furi_string_cmp_str(args, "start") == 0)
        printf(
            "Executes commands from a file line by line, igores lines starting with '#' as comments.\r\n");
    if(empty || furi_string_cmp_str(args, "touch") == 0) printf("touch <path>\r\n");
    if(empty || furi_string_cmp_str(args, "touch") == 0)
        printf("Creates a file at the specified path.\r\n");
    if(empty || furi_string_cmp_str(args, "write") == 0) printf("write <path>\r\n");
    if(empty || furi_string_cmp_str(args, "write") == 0)
        printf("Writes input text to the specified file.\r\n");
}
static void echo_handler(Cli* cli, FuriString* args, void* context) {
    UNUSED(cli);
    UNUSED(context);

    cli_write(cli, (uint8_t*)furi_string_get_cstr(args), furi_string_size(args));
    printf("\r\n");
}
static void ls_handler(Cli* cli, FuriString* args, void* context) {
    UNUSED(cli);
    UNUSED(context);

    Storage* storage = furi_record_open(RECORD_STORAGE);
    File* dir = storage_file_alloc(storage);
    if(cwd == NULL) cwd = furi_string_alloc_set_str("/");
    FuriString* path_local;
    if(furi_string_cmp_str(args, "") == 0)
        path_local = cwd;
    else
        path_local = path_join(cwd, args);
    if(!storage_dir_open(dir, furi_string_get_cstr(path_local))) {
        printf("Failed to read the directory\r\n");
        goto exit_ls;
    }
    if(!storage_file_is_dir(dir)) {
        printf("Not a directory\r\n");
        goto exit_ls;
    }
    char name[256];
    while(storage_dir_read(dir, NULL, name, 256)) {
        printf("%s\r\n", name);
    }
exit_ls:
    storage_dir_close(dir);
    storage_file_free(dir);
    furi_record_close(RECORD_STORAGE);
}
static void cd_handler(Cli* cli, FuriString* args, void* context) {
    UNUSED(cli);
    UNUSED(context);

    if(furi_string_cmp_str(args, "") == 0)
        furi_string_set_str(cwd, "/ext");
    else {
        FuriString* new_path = path_join(cwd, args);
        Storage* storage = furi_record_open(RECORD_STORAGE);
        File* dir = storage_file_alloc(storage);
        if(!storage_dir_open(dir, furi_string_get_cstr(new_path)))
            printf("Failed to open the directory. Maybe it doesn't exist.");
        else
            cwd = new_path;
        storage_dir_close(dir);
        storage_file_free(dir);
        furi_record_close(RECORD_STORAGE);
    }
}
static void pwd_handler(Cli* cli, FuriString* args, void* context) {
    UNUSED(cli);
    UNUSED(args);
    UNUSED(context);

    printf(furi_string_get_cstr(cwd));
}
static void read_handler(Cli* cli, FuriString* args, void* context) {
    UNUSED(cli);
    UNUSED(context);

    Storage* storage = furi_record_open(RECORD_STORAGE);
    File* file = storage_file_alloc(storage);
    args = path_join(cwd, args);
    if(!storage_file_exists(storage, furi_string_get_cstr(args))) {
        printf("File does not exist");
        goto exit_read;
    }
    if(!storage_file_open(file, furi_string_get_cstr(args), FSAM_READ, FSOM_OPEN_EXISTING)) {
        printf("Failed to open the file");
        goto exit_read;
    }
    uint8_t* content = malloc(256);
    uint16_t read_size = 0;
    do {
        read_size = storage_file_read(file, content, 256);
        for(int j = 0; j < read_size; j++) printf("%c", content[j]);
    } while(read_size > 0);
    printf("\r\n");

exit_read:
    storage_file_close(file);
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);
}
static void write_handler(Cli* cli, FuriString* args, void* context) {
    UNUSED(cli);
    UNUSED(context);

    Storage* storage = furi_record_open(RECORD_STORAGE);
    File* file = storage_file_alloc(storage);
    args = path_join(cwd, args);
    if(!storage_file_exists(storage, furi_string_get_cstr(args))) {
        printf("File does not exist");
        goto exit_write;
    }
    if(!storage_file_open(file, furi_string_get_cstr(args), FSAM_WRITE, FSOM_OPEN_EXISTING)) {
        printf("Failed to open the file");
        goto exit_write;
    }
    if(storage_file_is_dir(file)) {
        printf("The file is a directory!");
        goto exit_write;
    }
    storage_file_close(file);
    storage_simply_remove(storage, furi_string_get_cstr(args));
    if(!storage_file_open(file, furi_string_get_cstr(args), FSAM_WRITE, FSOM_OPEN_ALWAYS)) {
        printf("Failed to re-open the file");
        goto exit_write;
    }

    // Writing to file
    FuriString* content = furi_string_alloc_set_str("");
    printf("Type the new file contents here, then press Ctrl+C to exit...\r\n\r\n");
    while(true) {
        char input = cli_getc(cli);
        if(input == CliSymbolAsciiETX) break;
        if(input == CliSymbolAsciiBackspace) {
            furi_string_u_substr(content, 0, furi_string_size(content) - 1);
            printf("\b \b");
            fflush(stdout);
            continue;
        }
        printf("%c", input);
        fflush(stdout);
        furi_string_push_back(content, input);
    }
    uint16_t bytes_written =
        storage_file_write(file, furi_string_get_cstr(content), furi_string_size(content));
    printf("\r\nWrote %hu bytes to %s\r\n", bytes_written, furi_string_get_cstr(args));

exit_write:
    storage_file_close(file);
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);
}
static void touch_handler(Cli* cli, FuriString* args, void* context) {
    UNUSED(cli);
    UNUSED(context);

    Storage* storage = furi_record_open(RECORD_STORAGE);
    File* file = storage_file_alloc(storage);
    args = path_join(cwd, args);
    if(!storage_file_open(file, furi_string_get_cstr(args), FSAM_WRITE, FSOM_OPEN_ALWAYS)) {
        printf("Failed to create the file");
        goto exit_touch;
    }
    printf("File %s created", furi_string_get_cstr(args));

exit_touch:
    storage_file_close(file);
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);
}
static void mkdir_handler(Cli* cli, FuriString* args, void* context) {
    UNUSED(cli);
    UNUSED(context);

    Storage* storage = furi_record_open(RECORD_STORAGE);
    args = path_join(cwd, args);
    if(!storage_simply_mkdir(storage, furi_string_get_cstr(args))) {
        printf("Failed to create the folder");
        goto exit_mkdir;
    }
    printf("Folder %s created", furi_string_get_cstr(args));

exit_mkdir:
    furi_record_close(RECORD_STORAGE);
}
static void rm_handler(Cli* cli, FuriString* args, void* context) {
    UNUSED(cli);
    UNUSED(context);

    Storage* storage = furi_record_open(RECORD_STORAGE);
    args = path_join(cwd, args);
    if(!storage_simply_remove_recursive(storage, furi_string_get_cstr(args))) {
        printf("Failed to delete the file/folder");
        goto exit_rm;
    }
    printf("File/folder %s deleted", furi_string_get_cstr(args));

exit_rm:
    furi_record_close(RECORD_STORAGE);
}
static void mv_handler(Cli* cli, FuriString* args, void* context) {
    UNUSED(cli);
    UNUSED(context);

    FuriString* path1 = furi_string_alloc_set_str("");
    FuriString* path2 = furi_string_alloc_set_str("");
    path_split(args, path1, path2);
    path1 = path_join(cwd, path1);
    path2 = path_join(cwd, path2);
    if(furi_string_cmp(path1, path2) == 0) {
        printf("You cannot move a file into itself!");
        return;
    }

    Storage* storage = furi_record_open(RECORD_STORAGE);
    File* dir = storage_file_alloc(storage);
    if(storage_dir_open(dir, furi_string_get_cstr(path2)))
        path2 = path_join(path2, path_filename(path1));
    storage_dir_close(dir);
    storage_file_free(dir);

    FS_Error err =
        storage_common_rename(storage, furi_string_get_cstr(path1), furi_string_get_cstr(path2));
    if(err == FSE_OK)
        printf("Moved file successfully!");
    else if(err == FSE_EXIST)
        printf("File already exists!");
    else if(err == FSE_NOT_EXIST)
        printf("File doesn't exist!");
    else
        printf("Can't move the file!");
    furi_record_close(RECORD_STORAGE);
    furi_string_free(path1);
    furi_string_free(path2);
}
static void cp_handler(Cli* cli, FuriString* args, void* context) {
    UNUSED(cli);
    UNUSED(context);

    FuriString* path1 = furi_string_alloc_set_str("");
    FuriString* path2 = furi_string_alloc_set_str("");
    path_split(args, path1, path2);
    path1 = path_join(cwd, path1);
    path2 = path_join(cwd, path2);
    if(furi_string_cmp(path1, path2) == 0) {
        printf("You cannot copy a file into itself!");
        return;
    }

    Storage* storage = furi_record_open(RECORD_STORAGE);
    File* dir = storage_file_alloc(storage);
    if(storage_dir_open(dir, furi_string_get_cstr(path2)))
        path2 = path_join(path2, path_filename(path1));
    storage_dir_close(dir);
    storage_file_free(dir);

    FS_Error err =
        storage_common_copy(storage, furi_string_get_cstr(path1), furi_string_get_cstr(path2));
    if(err == FSE_OK)
        printf("Copied file successfully!");
    else if(err == FSE_EXIST)
        printf("File already exists!");
    else if(err == FSE_NOT_EXIST)
        printf("File doesn't exist!");
    else if(err == FSE_NOT_READY)
        printf("FSE_NOT_READY");
    else if(err == FSE_INVALID_PARAMETER)
        printf("FSE_INVALID_PARAMETER");
    else if(err == FSE_DENIED)
        printf("FSE_DENIED");
    else if(err == FSE_INVALID_NAME)
        printf("FSE_INVALID_NAME");
    else if(err == FSE_INTERNAL)
        printf("FSE_INTERNAL");
    else if(err == FSE_NOT_IMPLEMENTED)
        printf("FSE_NOT_IMPLEMENTED");
    else if(err == FSE_ALREADY_OPEN)
        printf("FSE_ALREADY_OPEN");
    else
        printf("Can't copy the file!");
    furi_record_close(RECORD_STORAGE);
    furi_string_free(path1);
    furi_string_free(path2);
}
static void start_handler(Cli* cli, FuriString* args, void* context) {
    UNUSED(cli);
    UNUSED(context);

    Storage* storage = furi_record_open(RECORD_STORAGE);
    File* file = storage_file_alloc(storage);
    args = path_join(cwd, args);
    if(!storage_file_exists(storage, furi_string_get_cstr(args))) {
        printf("File does not exist");
        goto exit_start;
    }
    if(!storage_file_open(file, furi_string_get_cstr(args), FSAM_READ, FSOM_OPEN_EXISTING)) {
        printf("Failed to open the file");
        goto exit_start;
    }
    FuriString* lines[256];
    for(int i = 0; i < 256; i++) lines[i] = furi_string_alloc_set_str("");
    int cur_line = 0;
    uint8_t* content = malloc(256);
    FuriString* line = furi_string_alloc_set_str("");
    uint16_t read_size = 0;
    do {
        read_size = storage_file_read(file, content, 256);
        for(int i = 0; i < read_size; i++) {
            if(content[i] == '\r')
                continue;
            else if(content[i] == '\n') {
                furi_string_set(lines[cur_line], line);
                furi_string_set_str(line, "");
                cur_line++;
            } else
                furi_string_push_back(line, content[i]);
        }
    } while(read_size > 0);
    furi_string_set(lines[cur_line], line);
    furi_string_free(line);
    for(int i = 0; i < 256; i++) {
        if(furi_string_start_with_str(lines[i], "#")) continue;
        furi_string_trim(lines[i]);
        if(furi_string_cmp_str(lines[i], "") == 0) continue;
        char search_for = ' ';
        size_t index = furi_string_search_char(lines[i], search_for);
        FuriString* command = furi_string_alloc_set(lines[i]);
        if(index != 4294967295) furi_string_u_substr(command, 0, index);
        FuriString* arguments = furi_string_alloc_set(lines[i]);
        if(index == 4294967295)
            furi_string_set_str(arguments, "");
        else
            furi_string_u_substr(arguments, index + 1, furi_string_size(lines[i]));

        /* Uncomment for debugging
		printf("L %d\r\n", i);
		printf("I %d\r\n", index);
		printf("C %s\r\n", furi_string_get_cstr(command));
		printf("A %s\r\n", furi_string_get_cstr(arguments));
		*/

        CliCommand* cli_command = CliCommandTree_get(cli->commands, command);
        cli_command->callback(cli, arguments, cli_command->context);
    }
    for(int i = 0; i < 256; i++) furi_string_free(lines[i]);

exit_start:
    storage_file_close(file);
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);
}
static void sleep_handler(Cli* cli, FuriString* args, void* context) {
    UNUSED(cli);
    UNUSED(context);

    int delay = atoi(furi_string_get_cstr(args));
    furi_delay_ms(delay);
}
static void sequence_handler(Cli* cli, FuriString* args, void* context) {
    UNUSED(cli);
    UNUSED(context);

    NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);
    if(furi_string_cmp_str(args, "reset_red") == 0)
        notification_message(notification, &sequence_reset_red);
    else if(furi_string_cmp_str(args, "reset_green") == 0)
        notification_message(notification, &sequence_reset_green);
    else if(furi_string_cmp_str(args, "reset_blue") == 0)
        notification_message(notification, &sequence_reset_blue);
    else if(furi_string_cmp_str(args, "reset_rgb") == 0)
        notification_message(notification, &sequence_reset_rgb);
    else if(furi_string_cmp_str(args, "reset_display") == 0)
        notification_message(notification, &sequence_reset_display);
    else if(furi_string_cmp_str(args, "reset_sound") == 0)
        notification_message(notification, &sequence_reset_sound);
    else if(furi_string_cmp_str(args, "reset_vibro") == 0)
        notification_message(notification, &sequence_reset_vibro);
    else if(furi_string_cmp_str(args, "set_vibro_on") == 0)
        notification_message(notification, &sequence_set_vibro_on);
    else if(furi_string_cmp_str(args, "display_backlight_on") == 0)
        notification_message(notification, &sequence_display_backlight_on);
    else if(furi_string_cmp_str(args, "display_backlight_off") == 0)
        notification_message(notification, &sequence_display_backlight_off);
    else if(furi_string_cmp_str(args, "display_backlight_off_delay_1000") == 0)
        notification_message(notification, &sequence_display_backlight_off_delay_1000);
    else if(furi_string_cmp_str(args, "display_backlight_enforce_on") == 0)
        notification_message(notification, &sequence_display_backlight_enforce_on);
    else if(furi_string_cmp_str(args, "display_backlight_enforce_auto") == 0)
        notification_message(notification, &sequence_display_backlight_enforce_auto);
    else if(furi_string_cmp_str(args, "charging") == 0)
        notification_message(notification, &sequence_charging);
    else if(furi_string_cmp_str(args, "charged") == 0)
        notification_message(notification, &sequence_charging);
    else if(furi_string_cmp_str(args, "not_charging") == 0)
        notification_message(notification, &sequence_not_charging);
    else if(furi_string_cmp_str(args, "set_only_red_255") == 0)
        notification_message(notification, &sequence_set_only_red_255);
    else if(furi_string_cmp_str(args, "set_only_green_255") == 0)
        notification_message(notification, &sequence_set_only_green_255);
    else if(furi_string_cmp_str(args, "set_only_blue_255") == 0)
        notification_message(notification, &sequence_set_only_blue_255);
    else if(furi_string_cmp_str(args, "set_red_255") == 0)
        notification_message(notification, &sequence_set_red_255);
    else if(furi_string_cmp_str(args, "set_green_255") == 0)
        notification_message(notification, &sequence_set_green_255);
    else if(furi_string_cmp_str(args, "set_blue_255") == 0)
        notification_message(notification, &sequence_set_blue_255);
    else if(furi_string_cmp_str(args, "solid_yellow") == 0)
        notification_message(notification, &sequence_solid_yellow);
    else if(furi_string_cmp_str(args, "blink_blue_10") == 0)
        notification_message(notification, &sequence_blink_blue_10);
    else if(furi_string_cmp_str(args, "blink_red_10") == 0)
        notification_message(notification, &sequence_blink_red_10);
    else if(furi_string_cmp_str(args, "blink_green_10") == 0)
        notification_message(notification, &sequence_blink_green_10);
    else if(furi_string_cmp_str(args, "blink_yellow_10") == 0)
        notification_message(notification, &sequence_blink_yellow_10);
    else if(furi_string_cmp_str(args, "blink_cyan_10") == 0)
        notification_message(notification, &sequence_blink_cyan_10);
    else if(furi_string_cmp_str(args, "blink_magenta_10") == 0)
        notification_message(notification, &sequence_blink_magenta_10);
    else if(furi_string_cmp_str(args, "blink_blue_100") == 0)
        notification_message(notification, &sequence_blink_blue_100);
    else if(furi_string_cmp_str(args, "blink_red_100") == 0)
        notification_message(notification, &sequence_blink_red_100);
    else if(furi_string_cmp_str(args, "blink_green_100") == 0)
        notification_message(notification, &sequence_blink_green_100);
    else if(furi_string_cmp_str(args, "blink_yellow_100") == 0)
        notification_message(notification, &sequence_blink_yellow_100);
    else if(furi_string_cmp_str(args, "blink_cyan_100") == 0)
        notification_message(notification, &sequence_blink_cyan_100);
    else if(furi_string_cmp_str(args, "blink_magenta_100") == 0)
        notification_message(notification, &sequence_blink_magenta_100);
    else if(furi_string_cmp_str(args, "blink_white_100") == 0)
        notification_message(notification, &sequence_blink_white_100);
    else if(furi_string_cmp_str(args, "blink_start_blue") == 0)
        notification_message(notification, &sequence_blink_start_blue);
    else if(furi_string_cmp_str(args, "blink_start_red") == 0)
        notification_message(notification, &sequence_blink_start_red);
    else if(furi_string_cmp_str(args, "blink_start_green") == 0)
        notification_message(notification, &sequence_blink_start_green);
    else if(furi_string_cmp_str(args, "blink_start_yellow") == 0)
        notification_message(notification, &sequence_blink_start_yellow);
    else if(furi_string_cmp_str(args, "blink_start_cyan") == 0)
        notification_message(notification, &sequence_blink_start_cyan);
    else if(furi_string_cmp_str(args, "blink_start_magenta") == 0)
        notification_message(notification, &sequence_blink_start_magenta);
    else if(furi_string_cmp_str(args, "blink_stop") == 0)
        notification_message(notification, &sequence_blink_stop);
    else if(furi_string_cmp_str(args, "single_vibro") == 0)
        notification_message(notification, &sequence_single_vibro);
    else if(furi_string_cmp_str(args, "double_vibro") == 0)
        notification_message(notification, &sequence_double_vibro);
    else if(furi_string_cmp_str(args, "success") == 0)
        notification_message(notification, &sequence_success);
    else if(furi_string_cmp_str(args, "error") == 0)
        notification_message(notification, &sequence_error);
    else if(furi_string_cmp_str(args, "audiovisual_alert") == 0)
        notification_message(notification, &sequence_audiovisual_alert);
    else
        printf("Usage: sequence <sequence>\r\nSee `extra_help` for more info\r\n");
    furi_record_close(RECORD_NOTIFICATION);
}

/*
   _____            _             _ 
  / ____|          | |           | |
 | |     ___  _ __ | |_ _ __ ___ | |
 | |    / _ \| '_ \| __| '__/ _ \| |
 | |___| (_) | | | | |_| | | (_) | |
  \_____\___/|_| |_|\__|_|  \___/|_|
*/
void extra_init() {
    if(initialized) return;
    cwd = furi_string_alloc_set_str("/ext");
    initialized = true;
}
void extra_register() {
    Cli* cli_ = furi_record_open(RECORD_CLI);
    cli_add_command(cli_, "extra_help", CliCommandFlagParallelSafe, extra_help_handler, NULL);
    cli_add_command(cli_, "echo", CliCommandFlagParallelSafe, echo_handler, NULL);
    cli_add_command(cli_, "ls", CliCommandFlagParallelSafe, ls_handler, NULL);
    cli_add_command(cli_, "cd", CliCommandFlagParallelSafe, cd_handler, NULL);
    cli_add_command(cli_, "pwd", CliCommandFlagParallelSafe, pwd_handler, NULL);
    cli_add_command(cli_, "read", CliCommandFlagParallelSafe, read_handler, NULL);
    cli_add_command(cli_, "write", CliCommandFlagParallelSafe, write_handler, NULL);
    cli_add_command(cli_, "touch", CliCommandFlagParallelSafe, touch_handler, NULL);
    cli_add_command(cli_, "mkdir", CliCommandFlagParallelSafe, mkdir_handler, NULL);
    cli_add_command(cli_, "rm", CliCommandFlagParallelSafe, rm_handler, NULL);
    cli_add_command(cli_, "mv", CliCommandFlagParallelSafe, mv_handler, NULL);
    cli_add_command(cli_, "cp", CliCommandFlagParallelSafe, cp_handler, NULL);
    cli_add_command(cli_, "start", CliCommandFlagParallelSafe, start_handler, NULL);
    cli_add_command(cli_, "sleep", CliCommandFlagParallelSafe, sleep_handler, NULL);
    cli_add_command(cli_, "sequence", CliCommandFlagParallelSafe, sequence_handler, NULL);
    furi_record_close(RECORD_CLI);
    registered = true;
}
void extra_init_register() {
    extra_init();
    extra_register();
}
void extra_deinit() {
    if(!initialized) return;
    furi_string_free(cwd);
    initialized = false;
}
void extra_unregister() {
    registered = false;
    Cli* cli_ = furi_record_open(RECORD_CLI);
    cli_delete_command(cli_, "extra_help");
    cli_delete_command(cli_, "echo");
    cli_delete_command(cli_, "ls");
    cli_delete_command(cli_, "cd");
    cli_delete_command(cli_, "pwd");
    cli_delete_command(cli_, "read");
    cli_delete_command(cli_, "write");
    cli_delete_command(cli_, "touch");
    cli_delete_command(cli_, "mkdir");
    cli_delete_command(cli_, "rm");
    cli_delete_command(cli_, "mv");
    cli_delete_command(cli_, "cp");
    cli_delete_command(cli_, "start");
    cli_delete_command(cli_, "sleep");
    cli_delete_command(cli_, "sequence");
    furi_record_close(RECORD_CLI);
}
void extra_deinit_unregister() {
    extra_unregister();
    extra_deinit();
}
bool extra_registered() {
    return registered;
}
bool extra_initialized() {
    return initialized;
}
