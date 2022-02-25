/**
  * @file irda_app.h
  * Infrared: Main infrared application class
  */
#pragma once
#include <map>
#include <irda.h>
#include <furi.h>
#include <forward_list>
#include <stdint.h>
#include <notification/notification_messages.h>
#include <irda_worker.h>

#include "scene/irda_app_scene.h"
#include "scene/irda_app_scene.h"
#include "irda_app_view_manager.h"
#include "irda_app_remote_manager.h"
#include "irda_app_view_manager.h"

/** Main Irda application class */
class IrdaApp {
public:
    /** Enum to save scene state: edit element */
    enum class EditElement : uint8_t {
        Button,
        Remote,
    };
    /** Enum to save scene state: edit action */
    enum class EditAction : uint8_t {
        Rename,
        Delete,
    };
    /** List of scenes for Irda application */
    enum class Scene : uint8_t {
        Exit,
        Start,
        Universal,
        UniversalTV,
        UniversalAudio,
        UniversalAirConditioner,
        Learn,
        LearnSuccess,
        LearnEnterName,
        LearnDone,
        AskBack,
        Remote,
        RemoteList,
        Edit,
        EditKeySelect,
        EditRename,
        EditDelete,
        EditRenameDone,
        EditDeleteDone,
    };

/** Start application
 *
 * @param args - application arguments.
 *      Allowed argument is path to remote file.
 * @retval 0 on success, error code otherwise
 */
    int32_t run(void* args);

/** Switch to next scene. Put current scene number on stack.
 * Doesn't save scene state.
 *
 * @param index - next scene index
 */
    void switch_to_next_scene(Scene index);

/** Switch to next scene, but don't put current scene on
 * stack. Thus calling switch_to_previous_scene() doesn't return
 * to current scene.
 *
 * @param index - next scene index
 */
    void switch_to_next_scene_without_saving(Scene index);

/** Switch to previous scene. Pop scenes from stack and switch to last one.
 *
 * @param count - how many scenes should be popped
 * @retval false on failed, true on success
 */
    bool switch_to_previous_scene(uint8_t count = 1);

/** Get previous scene in scene stack
 *
 * @retval previous scene
 */
    Scene get_previous_scene();

/** Get view manager instance
 *
 * @retval view manager instance
 */
    IrdaAppViewManager* get_view_manager();

/** Set one of text stores
 *
 * @param index - index of text store
 * @param text - text to set
 */
    void set_text_store(uint8_t index, const char* text...);

/** Get value in text store
 *
 * @param index - index of text store
 * @retval value in text_store
 */
    char* get_text_store(uint8_t index);

/** Get text store size
 *
 * @retval size of text store
 */
    uint8_t get_text_store_size();

/** Get remote manager instance
 *
 * @retval remote manager instance
 */
    IrdaAppRemoteManager* get_remote_manager();

/** Get irda worker instance
 *
 * @retval irda worker instance
 */
    IrdaWorker* get_irda_worker();

/** Get signal, previously got on Learn scene
 *
 * @retval received signal
 */
    const IrdaAppSignal& get_received_signal() const;

/** Set received signal
 *
 * @param signal - signal
 */
    void set_received_signal(const IrdaAppSignal& signal);

/** Switch to previous scene in one of provided in list.
 * Pop scene stack, and find first scene from list.
 *
 * @param scenes_list - list of scenes
 */
    void search_and_switch_to_previous_scene(const std::initializer_list<Scene>& scenes_list);

/** Set edit element value. It is used on edit scene to determine
 * what should be deleted - remote or button.
 *
 * @param value - value to set
 */
    void set_edit_element(EditElement value);

/** Get edit element
 *
 * @retval edit element value
 */
    EditElement get_edit_element(void);

/** Set edit action value. It is used on edit scene to determine
 * what action to perform - deletion or renaming.
 *
 * @param value - value to set
 */
    void set_edit_action(EditAction value);

/** Get edit action
 *
 * @retval edit action value
 */
    EditAction get_edit_action(void);

/** Get state of learning new signal.
 * Adding new remote with 1 button from start scene and
 * learning 1 additional button to remote have very similar
 * flow, so they are joined. Difference in flow is handled
 * by this boolean flag.
 *
 * @retval false if flow is in learning new remote, true if
 *      adding signal to created remote
 *
 */
    bool get_learn_new_remote();

/** Set state of learning new signal.
 * Adding new remote with 1 button from start scene and
 * learning 1 additional button to remote have very similar
 * flow, so they are joined. Difference in flow is handled
 * by this boolean flag.
 *
 * @param value - false if flow is in learning new remote, true if
 *      adding signal to created remote
 */
    void set_learn_new_remote(bool value);

/** Button is not assigned value
 */
    enum : int {
        ButtonNA = -1,
    };

/** Get current button index
 *
 * @retval current button index
 */
    int get_current_button();

/** Set current button index
 *
 * @param current button index
 */
    void set_current_button(int value);

/** Play success notification */
    void notify_success();
/** Play red blink notification */
    void notify_red_blink();
/** Light green */
    void notify_green_on();
/** Disable green light */
    void notify_green_off();
/** Play click sound */
    void notify_click();
/** Play click and green notification */
    void notify_click_and_green_blink();
/** Blink green light */
    void notify_blink_green();

/** Text input callback
 *
 * @param context - context to pass to callback
 */
    static void text_input_callback(void* context);

/** Popup callback
 *
 * @param context - context to pass to callback
 */
    static void popup_callback(void* context);

/** Signal sent callback
 *
 * @param context - context to pass to callback
 */
    static void signal_sent_callback(void* context);

/** Main class constructor, initializes all critical objects */
    IrdaApp();
/** Main class destructor, deinitializes all critical objects */
    ~IrdaApp();

/** Path to Irda directory */
    static constexpr const char* irda_directory = "/any/irda";
/** Irda files extension (remote files and universal databases) */
    static constexpr const char* irda_extension = ".ir";
/** Max Raw timings in signal */
    static constexpr const uint32_t max_raw_timings_in_signal = 512;
/** Max line length in Infrared file */
    static constexpr const uint32_t max_line_length =
        (9 + 1) * IrdaApp::max_raw_timings_in_signal + 100;

private:
/** Text store size */
    static constexpr const uint8_t text_store_size = 128;
/** Amount of text stores */
    static constexpr const uint8_t text_store_max = 2;
/** Store text here, for some views, because they doesn't
 * hold ownership of text */
    char text_store[text_store_max][text_store_size + 1];
/**
 * Flag to control adding new signal flow.
 * Adding new remote with 1 button from start scene and
 * learning 1 additional button to remote have very similar
 * flow, so they are joined. Difference in flow is handled
 * by this boolean flag.
 */
    bool learn_new_remote;
/** Value to control edit scene */
    EditElement element;
/** Value to control edit scene */
    EditAction action;
/** Selected button index */
    uint32_t current_button;

/** Notification instance */
    NotificationApp* notification;
/** View manager instance */
    IrdaAppViewManager view_manager;
/** Remote manager instance */
    IrdaAppRemoteManager remote_manager;
/** Irda worker instance */
    IrdaWorker* irda_worker;
/** Signal received on Learn scene */
    IrdaAppSignal received_signal;

/** Stack of previous scenes */
    std::forward_list<Scene> previous_scenes_list;
/** Now acting scene */
    Scene current_scene = Scene::Start;

/** Map of index/scene objects */
    std::map<Scene, IrdaAppScene*> scenes = {
        {Scene::Start, new IrdaAppSceneStart()},
        {Scene::Universal, new IrdaAppSceneUniversal()},
        {Scene::UniversalTV, new IrdaAppSceneUniversalTV()},
        {Scene::Learn, new IrdaAppSceneLearn()},
        {Scene::LearnSuccess, new IrdaAppSceneLearnSuccess()},
        {Scene::LearnEnterName, new IrdaAppSceneLearnEnterName()},
        {Scene::LearnDone, new IrdaAppSceneLearnDone()},
        {Scene::AskBack, new IrdaAppSceneAskBack()},
        {Scene::Remote, new IrdaAppSceneRemote()},
        {Scene::RemoteList, new IrdaAppSceneRemoteList()},
        {Scene::Edit, new IrdaAppSceneEdit()},
        {Scene::EditKeySelect, new IrdaAppSceneEditKeySelect()},
        {Scene::EditRename, new IrdaAppSceneEditRename()},
        {Scene::EditDelete, new IrdaAppSceneEditDelete()},
        {Scene::EditRenameDone, new IrdaAppSceneEditRenameDone()},
        {Scene::EditDeleteDone, new IrdaAppSceneEditDeleteDone()},
    };
};
