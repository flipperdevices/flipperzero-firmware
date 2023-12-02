// v2.1

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

#define MAX_SCRIPT_LINES 256
#define READ_CHUNK_SIZE 256
#define PATH_SPLIT_SIZE 256
#define LS_NAME_SIZE 256
#define ENV_VAR_COUNT 64

static FuriString* cwd;
static bool registered = false;
static bool initialized = false;

/*
  ______                              _       _     _           
 |  ____|                            (_)     | |   | |          
 | |__   _ ____   __ __   ____ _ _ __ _  __ _| |__ | | ___  ___ 
 |  __| | '_ \ \ / / \ \ / / _` | '__| |/ _` | '_ \| |/ _ \/ __|
 | |____| | | \ V /   \ V / (_| | |  | | (_| | |_) | |  __/\__ \
 |______|_| |_|\_/     \_/ \__,_|_|  |_|\__,_|_.__/|_|\___||___/
*/
typedef struct EnvVarialbe {
    FuriString* name;
    FuriString* value;
} EnvVarialbe;
static EnvVarialbe* env_var_alloc() {
    EnvVarialbe* var = (EnvVarialbe*)malloc(sizeof(EnvVarialbe));
    var->name = furi_string_alloc_set_str("");
    var->value = furi_string_alloc_set_str("");
    return var;
}
static void env_var_free(EnvVarialbe* var) {
    furi_string_free(var->name);
    furi_string_free(var->value);
    free(var);
}
static EnvVarialbe* vars[ENV_VAR_COUNT];
static FuriString* env_var_get(FuriString* name) {
    for(int i = 0; i < ENV_VAR_COUNT; i++) {
        if(furi_string_cmp(vars[i]->name, name) != 0) continue;
        return vars[i]->value;
    }
    return furi_string_alloc_set_str("");
}
static void env_var_set(FuriString* name, FuriString* value) {
    for(int i = 0; i < ENV_VAR_COUNT; i++) {
        if(furi_string_cmp(vars[i]->name, name) == 0) {
            furi_string_set(vars[i]->value, value);
            return;
        }
        if(furi_string_cmp_str(vars[i]->name, "") == 0) {
            furi_string_set(vars[i]->name, name);
            furi_string_set(vars[i]->value, value);
            return;
        }
    }
}

/*
  ______          _  _____ _        _             
 |  ____|        (_)/ ____| |      (_)            
 | |__ _   _ _ __ _| (___ | |_ _ __ _ _ __   __ _ 
 |  __| | | | '__| |\___ \| __| '__| | '_ \ / _` |
 | |  | |_| | |  | |____) | |_| |  | | | | | (_| |
 |_|   \__,_|_|  |_|_____/ \__|_|  |_|_| |_|\__, |
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
static bool furi_string_u_empty(FuriString* str) {
    for(size_t i = 0; i < furi_string_size(str); i++) {
        char c = furi_string_get_char(str, i);
        if(c > 32 && c != 127) return false;
    }
    return true;
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
    FuriString* parts[PATH_SPLIT_SIZE]; // Max safe depth
    for(int i = 0; i < PATH_SPLIT_SIZE; i++) {
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
    for(int i = 0; i < PATH_SPLIT_SIZE; i++) {
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
    for(int i = 0; i < PATH_SPLIT_SIZE; i++) {
        if(furi_string_cmp_str(parts[i], "") == 0) continue;
        flag = true;
        furi_string_cat(res, parts[i]);
        furi_string_push_back(res, '/');
    }
    for(int i = 0; i < PATH_SPLIT_SIZE; i++) furi_string_free(parts[i]);
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
  ______                     _       
 |  ____|                   | |      
 | |__  __  _____  ___ _   _| |_ ___ 
 |  __| \ \/ / _ \/ __| | | | __/ _ \
 | |____ >  <  __/ (__| |_| | ||  __/
 |______/_/\_\___|\___|\__,_|\__\___|
*/
static void execute_command(FuriString* scommand, Cli* cli, bool replace_vars) {
    size_t cmti = furi_string_search_char(scommand, '#');
    FuriString* cline = furi_string_alloc_set(scommand);
    furi_string_u_substr(cline, 0, cmti);
    furi_string_trim(cline);
    if(replace_vars)
        while(true) {
            for(int i = 0; i < ENV_VAR_COUNT; i++) {
                if(furi_string_cmp_str(vars[i]->name, "") == 0) continue;
                FuriString* needle = furi_string_alloc_set_str("%");
                furi_string_cat(needle, vars[i]->name);
                furi_string_push_back(needle, '%');
                furi_string_replace_all(cline, needle, vars[i]->value);
                furi_string_free(needle);
            }
            bool flag = true;
            for(int i = 0; i < ENV_VAR_COUNT; i++) {
                if(furi_string_cmp_str(vars[i]->name, "") == 0) continue;
                FuriString* needle = furi_string_alloc_set_str("%");
                furi_string_cat(needle, vars[i]->name);
                furi_string_push_back(needle, '%');
                if(furi_string_search(cline, needle) != 4294967295) {
                    flag = false;
                    break;
                }
            }
            if(flag) break;
        }
    if(furi_string_cmp_str(cline, "") == 0) return;
    size_t index = furi_string_search_char(cline, ' ');
    FuriString* command = furi_string_alloc_set(cline);
    if(index != 4294967295) furi_string_u_substr(command, 0, index);
    FuriString* arguments = furi_string_alloc_set(cline);
    if(index == 4294967295)
        furi_string_set_str(arguments, "");
    else
        furi_string_u_substr(arguments, index + 1, furi_string_size(cline));

    /* Uncomment for debugging
	printf("L %d\r\n", i);
	printf("I %d\r\n", cmti);
	printf("J %d\r\n", index);
	printf("X %s\r\n", furi_string_get_cstr(cline));
	printf("C %s\r\n", furi_string_get_cstr(command));
	printf("A %s\r\n", furi_string_get_cstr(arguments));
	*/

    CliCommand* cli_command = CliCommandTree_get(cli->commands, command);
    cli_command->callback(cli, arguments, cli_command->context);

    furi_string_free(cline);
    furi_string_free(command);
    furi_string_free(arguments);
}
static FuriString* execute_command_listen(FuriString* command, Cli* cli) {
    FuriString* output = furi_string_alloc_set_str("");
    void cb(const char* data, size_t size) {
        UNUSED(size);
        furi_string_cat_str(output, data);
    }
    // FuriThread* thread = furi_thread_get_current();
    FuriThreadStdoutWriteCallback old_cb = furi_thread_get_stdout_callback();
    furi_thread_set_stdout_callback(cb);
    execute_command(command, cli, false);
    // furi_thread_set_stdout_callback(cli->session->tx_stdout);
    furi_thread_set_stdout_callback(old_cb);
    return output;
}

/*
  _____                _   _ _            
 |  __ \              | | | (_)           
 | |__) |___  __ _  __| | | |_ _ __   ___ 
 |  _  // _ \/ _` |/ _` | | | | '_ \ / _ \
 | | \ \  __/ (_| | (_| | | | | | | |  __/
 |_|  \_\___|\__,_|\__,_| |_|_|_| |_|\___|
*/
static FuriString* read_line(Cli* cli) {
    FuriString* input = furi_string_alloc_set_str("");
    while(true) {
        char c;
        cli_read(cli, (uint8_t*)&c, 1);
        if(c == CliSymbolAsciiBackspace) {
            furi_string_u_substr(input, 0, furi_string_size(input) - 1);
            printf("\b \b");
            fflush(stdout);
            continue;
        }
        if(c == CliSymbolAsciiLF) continue;
        if(c == CliSymbolAsciiCR) {
            return input;
        } else {
            furi_string_push_back(input, c);
            printf("%c", c);
            fflush(stdout);
        }
    }
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
    if(empty || furi_string_cmp_str(args, "false") == 0) printf("false\r\n");
    if(empty || furi_string_cmp_str(args, "false") == 0) printf("Always outputs nothing.\r\n");
    if(empty || furi_string_cmp_str(args, "get") == 0) printf("get <name>\r\n");
    if(empty || furi_string_cmp_str(args, "get") == 0)
        printf("Prints the value of variable <name>\r\n");
    if(empty || furi_string_cmp_str(args, "if") == 0) printf("if <command1>;<command2>\r\n");
    if(empty || furi_string_cmp_str(args, "if") == 0)
        printf(
            "If <command1> output is non-empty, executes command 2. Note that commands are separated by a semicolon, not a space.\r\n");
    if(empty || furi_string_cmp_str(args, "input") == 0) printf("input <name>\r\n");
    if(empty || furi_string_cmp_str(args, "input") == 0)
        printf("Gets an input from the user and saves it to the variable <name>\r\n");
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
    if(empty || furi_string_cmp_str(args, "not") == 0) printf("not <command>\r\n");
    if(empty || furi_string_cmp_str(args, "not") == 0)
        printf(
            "If <command> output is non-empty, then output nothing. Otherwise, output \"true\".\r\n");
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
    if(empty || furi_string_cmp_str(args, "set") == 0) printf("set <name> <value>\r\n");
    if(empty || furi_string_cmp_str(args, "set") == 0)
        printf("Sets the variable <name> to <value>\r\n");
    if(empty || furi_string_cmp_str(args, "sleep") == 0) printf("sleep <time>\r\n");
    if(empty || furi_string_cmp_str(args, "sleep") == 0)
        printf("Freezes for <time> milliseconds.\r\n");
    if(empty || furi_string_cmp_str(args, "start") == 0) printf("sh\r\n");
    if(empty || furi_string_cmp_str(args, "start") == 0) printf("Starts the EXTRA shell.\r\n");
    if(empty || furi_string_cmp_str(args, "start") == 0) printf("start <path>\r\n");
    if(empty || furi_string_cmp_str(args, "start") == 0)
        printf("Execute the script at the specified path.\r\n");
    if(empty || furi_string_cmp_str(args, "start") == 0)
        printf(
            "Executes commands from a file line by line, igores lines starting with '#' as comments.\r\n");
    if(empty || furi_string_cmp_str(args, "start") == 0)
        printf(
            "For more info refer to https://github.com/Milk-Cool/EXTRA/blob/main/README.md.\r\n");
    if(empty || furi_string_cmp_str(args, "touch") == 0) printf("touch <path>\r\n");
    if(empty || furi_string_cmp_str(args, "touch") == 0)
        printf("Creates a file at the specified path.\r\n");
    if(empty || furi_string_cmp_str(args, "true") == 0) printf("true\r\n");
    if(empty || furi_string_cmp_str(args, "true") == 0) printf("Always outputs \"true\".\r\n");
    if(empty || furi_string_cmp_str(args, "write") == 0) printf("while <command1>;<command2>\r\n");
    if(empty || furi_string_cmp_str(args, "write") == 0)
        printf("While <command1> output is non-empty, repeatedly execute <command2>.\r\n");
    if(empty || furi_string_cmp_str(args, "write") == 0) printf("write <path>\r\n");
    if(empty || furi_string_cmp_str(args, "write") == 0)
        printf("Writes input text to the specified file.\r\n");
}
static void echo_handler(Cli* cli, FuriString* args, void* context) {
    UNUSED(cli);
    UNUSED(context);

    printf("%s\r\n", furi_string_get_cstr(args));
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
    char name[LS_NAME_SIZE];
    while(storage_dir_read(dir, NULL, name, LS_NAME_SIZE)) {
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
        else {
            cwd = new_path;
        }
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
    uint8_t* content = malloc(READ_CHUNK_SIZE);
    uint16_t read_size = 0;
    do {
        read_size = storage_file_read(file, content, READ_CHUNK_SIZE);
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
    if(err == FSE_OK) {
        printf("Moved file successfully!");
    } else if(err == FSE_EXIST)
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
    if(err == FSE_OK) {
        printf("Copied file successfully!");
    } else if(err == FSE_EXIST)
        printf("File already exists!");
    else if(err == FSE_NOT_EXIST)
        printf("File doesn't exist!");
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
    FuriString* lines[MAX_SCRIPT_LINES];
    for(int i = 0; i < MAX_SCRIPT_LINES; i++) lines[i] = furi_string_alloc_set_str("");
    int cur_line = 0;
    uint8_t* content = malloc(READ_CHUNK_SIZE);
    FuriString* line = furi_string_alloc_set_str("");
    uint16_t read_size = 0;
    do {
        read_size = storage_file_read(file, content, READ_CHUNK_SIZE);
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
    for(int i = 0; i < MAX_SCRIPT_LINES; i++) execute_command(lines[i], cli, true);
    for(int i = 0; i < MAX_SCRIPT_LINES; i++) furi_string_free(lines[i]);

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
    else {
        printf("Usage: sequence <sequence>\r\nSee `extra_help` for more info\r\n");
    }
    furi_record_close(RECORD_NOTIFICATION);
}
static void set_handler(Cli* cli, FuriString* args, void* context) {
    UNUSED(cli);
    UNUSED(context);

    furi_string_trim(args);
    size_t index = furi_string_search_char(args, ' ');
    FuriString* name = furi_string_alloc_set(args);
    if(index != 4294967295) furi_string_u_substr(name, 0, index);
    FuriString* value = furi_string_alloc_set(args);
    if(index == 4294967295)
        furi_string_set_str(value, "");
    else
        furi_string_u_substr(value, index + 1, furi_string_size(args));

    env_var_set(name, value);
    furi_string_free(name);
    furi_string_free(value);
}
static void get_handler(Cli* cli, FuriString* args, void* context) {
    UNUSED(cli);
    UNUSED(context);

    furi_string_trim(args);

    printf(furi_string_get_cstr(env_var_get(args)));
}
static void input_handler(Cli* cli, FuriString* args, void* context) {
    UNUSED(context);

    furi_string_trim(args);

    FuriString* input = read_line(cli);
    env_var_set(args, input);
    printf("\r\n");
    furi_string_free(input);
}
static void true_handler(Cli* cli, FuriString* args, void* context) {
    UNUSED(context);
    UNUSED(cli);
    UNUSED(args);

    printf("true");
}
static void false_handler(Cli* cli, FuriString* args, void* context) {
    UNUSED(context);
    UNUSED(cli);
    UNUSED(args);
}
static void not_handler(Cli* cli, FuriString* args, void* context) {
    UNUSED(context);

    FuriString* out = execute_command_listen(args, cli);
    if(furi_string_u_empty(out)) printf("true");
    furi_string_free(out);
}
static void if_handler(Cli* cli, FuriString* args, void* context) {
    UNUSED(context);

    size_t index = furi_string_search_char(args, ';');
    FuriString* cmd1 = furi_string_alloc_set(args);
    if(index != 4294967295) furi_string_u_substr(cmd1, 0, index);
    FuriString* cmd2 = furi_string_alloc_set(args);
    if(index == 4294967295)
        return;
    else
        furi_string_u_substr(cmd2, index + 1, furi_string_size(args));

    FuriString* out = execute_command_listen(cmd1, cli);
    if(!furi_string_u_empty(out)) execute_command(cmd2, cli, false);
    furi_string_free(out);
}
static void while_handler(Cli* cli, FuriString* args, void* context) {
    UNUSED(context);

    size_t index = furi_string_search_char(args, ';');
    FuriString* cmd1 = furi_string_alloc_set(args);
    if(index != 4294967295) furi_string_u_substr(cmd1, 0, index);
    FuriString* cmd2 = furi_string_alloc_set(args);
    if(index == 4294967295)
        return;
    else
        furi_string_u_substr(cmd2, index + 1, furi_string_size(args));

    while(true) {
        FuriString* out = execute_command_listen(cmd1, cli);
        bool brk = furi_string_u_empty(out);
        furi_string_free(out);
        if(brk) break;
        execute_command(cmd2, cli, false);
    }
}
static void sh_handler(Cli* cli, FuriString* args, void* context) {
    UNUSED(args);
    UNUSED(context);
    while(true) {
        printf("%s > ", furi_string_get_cstr(cwd));
        fflush(stdout);
        FuriString* input = read_line(cli);
        if(furi_string_cmp_str(input, "exit") == 0) break;
        execute_command(input, cli, true);
        printf("\r\n");
        furi_string_free(input);
    }
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
    for(int i = 0; i < ENV_VAR_COUNT; i++) vars[i] = env_var_alloc();
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
    cli_add_command(cli_, "set", CliCommandFlagParallelSafe, set_handler, NULL);
    cli_add_command(cli_, "get", CliCommandFlagParallelSafe, get_handler, NULL);
    cli_add_command(cli_, "input", CliCommandFlagParallelSafe, input_handler, NULL);
    cli_add_command(cli_, "not", CliCommandFlagParallelSafe, not_handler, NULL);
    cli_add_command(cli_, "false", CliCommandFlagParallelSafe, false_handler, NULL);
    cli_add_command(cli_, "true", CliCommandFlagParallelSafe, true_handler, NULL);
    cli_add_command(cli_, "if", CliCommandFlagParallelSafe, if_handler, NULL);
    cli_add_command(cli_, "while", CliCommandFlagParallelSafe, while_handler, NULL);
    cli_add_command(cli_, "sh", CliCommandFlagParallelSafe, sh_handler, NULL);
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
    for(int i = 0; i < ENV_VAR_COUNT; i++) env_var_free(vars[i]);
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
    cli_delete_command(cli_, "set");
    cli_delete_command(cli_, "get");
    cli_delete_command(cli_, "input");
    cli_delete_command(cli_, "not");
    cli_delete_command(cli_, "false");
    cli_delete_command(cli_, "true");
    cli_delete_command(cli_, "if");
    cli_delete_command(cli_, "while");
    cli_delete_command(cli_, "sh");
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
