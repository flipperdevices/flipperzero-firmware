#include "lfrfid-app.h"
#include "scene/lfrfid-app-scene-start.h"
#include "scene/lfrfid-app-scene-read.h"
#include "scene/lfrfid-app-scene-read-success.h"
#include "scene/lfrfid-app-scene-readed-menu.h"
#include "scene/lfrfid-app-scene-write.h"
#include "scene/lfrfid-app-scene-write-success.h"
#include "scene/lfrfid-app-scene-emulate.h"
#include "scene/lfrfid-app-scene-save-name.h"

#include <file-worker.h>
#include <path.h>

const char* LfRfidApp::app_folder = "lfrfid";
const char* LfRfidApp::app_extension = ".rfid";

LfRfidApp::LfRfidApp()
    : scene_controller{this}
    , fs_api{"sdcard"}
    , sd_ex_api{"sdcard-ex"}
    , notification{"notification"}
    , text_store(40) {
    api_hal_power_insomnia_enter();

    // we need random
    srand(DWT->CYCCNT);
}

LfRfidApp::~LfRfidApp() {
    api_hal_power_insomnia_exit();
}

void LfRfidApp::run(void* args) {
    scene_controller.add_scene(SceneType::Start, new LfRfidAppSceneStart());
    scene_controller.add_scene(SceneType::Read, new LfRfidAppSceneRead());
    scene_controller.add_scene(SceneType::ReadSuccess, new LfRfidAppSceneReadSuccess());
    scene_controller.add_scene(SceneType::ReadedMenu, new LfRfidAppSceneReadedMenu());
    scene_controller.add_scene(SceneType::Write, new LfRfidAppSceneWrite());
    scene_controller.add_scene(SceneType::WriteSuccess, new LfRfidAppSceneWriteSuccess());
    scene_controller.add_scene(SceneType::Emulate, new LfRfidAppSceneEmulate());
    scene_controller.add_scene(SceneType::SaveName, new LfRfidAppSceneSaveName());
    scene_controller.process(100);
}

bool LfRfidApp::load_key_data(const char* path, RfidKey* key) {
    FileWorker file_worker;
    bool result = false;

    bool res = file_worker.open(path, FSAM_READ, FSOM_OPEN_EXISTING);

    if(res) {
        string_t str_result;
        string_init(str_result);

        do {
            RfidKey loaded_key;
            LfrfidKeyType loaded_type;

            // load type
            if(!file_worker.read_until(str_result, ' ')) break;
            if(!lfrfid_key_get_string_type(string_get_cstr(str_result), &loaded_type)) {
                file_worker.show_error("Cannot parse\nfile");
                break;
            }
            loaded_key.set_type(loaded_type);

            // load data
            if(!file_worker.read_hex(key->get_data(), key->get_type_data_count())) break;

            *key = loaded_key;
            result = true;
        } while(0);

        // load name
        path_extract_filename_no_ext(path, str_result);
        key->set_name(string_get_cstr(str_result));

        string_clear(str_result);
    }

    file_worker.close();

    return result;
}

bool LfRfidApp::save_key_data(const char* path, RfidKey* key) {
    FileWorker file_worker;
    bool result = false;

    bool res = file_worker.open(path, FSAM_WRITE, FSOM_CREATE_ALWAYS);

    if(res) {
        do {
            // type header
            const char* key_type = lfrfid_key_get_type_string(key->get_type());
            char delimeter = ' ';

            if(!file_worker.write(key_type, strlen(key_type))) break;
            if(!file_worker.write(&delimeter)) break;
            if(!file_worker.write_hex(key->get_data(), key->get_type_data_count())) break;

            result = true;
        } while(0);
    }

    file_worker.close();

    return result;
}