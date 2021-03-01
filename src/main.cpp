#include "main.hpp"
#include "audiocliploader.hpp"
#include <dlfcn.h>
//using namespace audioClipLoader;

#include "GlobalNamespace/ResultsViewController.hpp"
#include "GlobalNamespace/SongPreviewPlayer.hpp"
#include "GlobalNamespace/NoteCutSoundEffectManager.hpp"
#include "GlobalNamespace/NoteCutSoundEffect.hpp"
#include "GlobalNamespace/BasicUIAudioManager.hpp"
#include "GlobalNamespace/FireworkItemController.hpp"
using namespace GlobalNamespace;


#include "UnityEngine/SceneManagement/Scene.hpp"
//using namespace UnityEngine::SceneManagement;

//#include "questui/shared/BeatSaberUI.hpp"
//#include "questui/shared/QuestUI.hpp"
//using namespace QuestUI;

//#include "NewAudioClipLoader.hpp"
//#include "QSoundsViewController.hpp"
//using namespace QuestSounds;

#include "custom-types/shared/register.hpp"
using namespace custom_types;

#define RAPIDJSON_HAS_STDSTRING 1
#define SOUND_PATH_FORMAT "/sdcard/ModData/%s/Mods/QuestSounds/"

ModInfo modInfo;

// Loads the config from disk using our modInfo, then returns it for use
//Configuration& getConfig() {
//    static Configuration config(modInfo);   // TODO: Why is this ambigous.
//    config.Load();
//    return config;
//}
Configuration& getConfig() {
    static Configuration config(modInfo);
    return config;
}

Logger& getLogger() {
    static auto logger = new Logger(modInfo, LoggerOptions(false, true));
    return *logger;
}

std::string soundPath = string_format(SOUND_PATH_FORMAT, Modloader::getApplicationId().c_str());
void makeFolder() 
{
    
    if(!direxists(soundPath.c_str()))
    {
        int makePath = mkpath(soundPath.data());
        if(makePath == -1)
        {
            getLogger().debug("Failed to make path!");
        }
    }
}

//Config stuff
static struct Config_t 
{
    bool hitSound_Active = true;
    bool badHitSound_Active = true;
    bool menuMusic_Active = true;
    bool menuClick_Active = true;
    bool firework_Active = true;
    bool levelCleared_Active = true;
    std::string hitSound_filepath = soundPath + "HitSound.ogg";
    std::string badHitSound_filepath = soundPath + "BadHitSound.ogg";
    std::string menuMusic_filepath = soundPath + "MenuMusic.ogg";
    std::string menuClick_filepath = soundPath + "MenuClick.ogg";
    std::string firework_filepath = soundPath + "Firework.ogg";
    std::string levelCleared_filepath = soundPath + "LevelCleared.ogg";
} Config;

void AddChildSound(ConfigValue& parent, std::string_view soundName, bool active, std::string filePath, ConfigDocument::AllocatorType& allocator)
{
    ConfigValue value(rapidjson::kObjectType);
    value.AddMember("activated", active, allocator);
    std::string data(filePath);
    value.AddMember("filepath", data, allocator);
    parent.AddMember((ConfigValue::StringRefType)soundName.data(), value, allocator);
}

// Edited ParseVector made by Darknight1050
bool ParseSound(bool& active, std::string& filepath, ConfigValue& parent, std::string_view soundName)
{
    if(!parent.HasMember(soundName.data()) || !parent[soundName.data()].IsObject()) return false;
    ConfigValue value = parent[soundName.data()].GetObject();
    if(!(value.HasMember("activated") && value["activated"].IsBool() &&
         value.HasMember("filepath") && value["filepath"].IsString() )) return false;
    active = value["activated"].GetBool();
    filepath = value["filepath"].GetString();
    return true;
}

void SaveConfig()
{
    getConfig().config.RemoveAllMembers();
    getConfig().config.SetObject();
    auto& allocator = getConfig().config.GetAllocator();

    ConfigValue soundsValue(rapidjson::kObjectType);
    AddChildSound(soundsValue, "HitSound", Config.hitSound_Active, Config.hitSound_filepath, allocator);
    AddChildSound(soundsValue, "BadHitSound", Config.badHitSound_Active, Config.badHitSound_filepath, allocator);  
    AddChildSound(soundsValue, "MenuMusic", Config.menuMusic_Active, Config.menuMusic_filepath, allocator);  
    AddChildSound(soundsValue, "MenuClick", Config.menuClick_Active, Config.menuClick_filepath, allocator);  
    AddChildSound(soundsValue, "Firework", Config.firework_Active, Config.firework_filepath, allocator);  
    AddChildSound(soundsValue, "LevelCleared", Config.levelCleared_Active, Config.levelCleared_filepath, allocator); 
    getConfig().config.AddMember("Sounds 1.0.0", soundsValue, allocator); 
    getConfig().Write();
}

bool LoadConfig()
{
    getConfig().Load();

    if(getConfig().config.HasMember("Sounds 1.0.0") && getConfig().config["Sounds 1.0.0"].IsObject())
    {
        ConfigValue soundsValue = getConfig().config["Sounds 1.0.0"].GetObject();
        if(!ParseSound(Config.hitSound_Active, Config.hitSound_filepath, soundsValue, "HitSound")) return false;
        if(!ParseSound(Config.badHitSound_Active, Config.badHitSound_filepath, soundsValue, "BadHitSound")) return false;
        if(!ParseSound(Config.menuMusic_Active, Config.menuMusic_filepath, soundsValue, "MenuMusic")) return false;
        if(!ParseSound(Config.menuClick_Active, Config.menuClick_filepath, soundsValue, "MenuClick")) return false;
        if(!ParseSound(Config.firework_Active, Config.firework_filepath, soundsValue, "Firework")) return false;
        if(!ParseSound(Config.levelCleared_Active, Config.levelCleared_filepath, soundsValue, "LevelCleared")) return false;
    } else return false;
    
    return true;
}

audioClipLoader::loader hitSoundLoader; // hitSound
audioClipLoader::loader badHitSoundLoader; // badHitSound
audioClipLoader::loader menuMusicLoader;    // menuMusic
audioClipLoader::loader menuClickLoader;
audioClipLoader::loader fireworkSoundLoader;
audioClipLoader::loader levelClearedLoader;
Array<UnityEngine::AudioClip*>* hitSoundArr; // hitSoundArray
Array<UnityEngine::AudioClip*>* badHitSoundArr; // badHitSoundArray
Array<UnityEngine::AudioClip*>* menuClickArr;
Array<UnityEngine::AudioClip*>* fireworkSoundArr;


/*
also lines 147 to 150 is not only bad practice
(because holding pointers to C# objects like that may not consider the possibilities if the object is GC'd)
but also bad because you set them to null
*/

//Il2CppArray* hitSoundArr; // hitSoundArray
//Il2CppArray* badHitSoundArr; // badHitSoundArray
//Il2CppArray* menuClickArr;
//Il2CppArray* fireworkSoundArr;


void loadAudioClips()
{
    hitSoundLoader.filePath = Config.hitSound_filepath;
    badHitSoundLoader.filePath = Config.badHitSound_filepath;
    menuMusicLoader.filePath = Config.menuMusic_filepath;
    menuClickLoader.filePath = Config.menuClick_filepath;
    fireworkSoundLoader.filePath = Config.firework_filepath;
    levelClearedLoader.filePath =  Config.levelCleared_filepath;
    if(Config.hitSound_Active) hitSoundLoader.load();
    if(Config.badHitSound_Active) badHitSoundLoader.load();
    if(Config.menuMusic_Active) menuMusicLoader.load();
    if(Config.menuClick_Active) menuClickLoader.load();
    if(Config.firework_Active) fireworkSoundLoader.load();
    if(Config.levelCleared_Active) levelClearedLoader.load();
}

// Creates an Array, of AudioClips
Array<UnityEngine::AudioClip*>* createAudioClipArray(audioClipLoader::loader clipLoader)
{
    ////Il2CppObject* tempClip = clipLoader.getClip();
    UnityEngine::AudioClip* tempClip = clipLoader.getClip();
    ////Il2CppArray* temporaryArray = (il2cpp_functions::array_new(il2cpp_utils::GetClassFromName("UnityEngine", "AudioClip"), 1));
    //::Array<UnityEngine::AudioClip*>* temporaryArray;
    ////il2cpp_array_set(temporaryArray, Il2CppObject*, 0, tempClip);
    //il2cpp_array_set(temporaryArray, Il2CppObject*, 0, tempClip);
    //return temporaryArray;
    //auto* temporaryArray = Array<UnityEngine::AudioClip*>::New(/*length you want of your array*/);
    auto* temporaryArray = Array<UnityEngine::AudioClip*>::New(tempClip);
    temporaryArray->values[0] = tempClip;
    return temporaryArray;
}

//MAKE_HOOK_OFFSETLESS(ResultsViewController_DidActivate, void, Il2CppObject* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
//{
//    if(levelClearedLoader.loaded && addedToHierarchy)
//    {
//        Il2CppObject* audioClip = levelClearedLoader.getClip();
//        CRASH_UNLESS(il2cpp_utils::SetFieldValue(self, "_levelClearedAudioClip", audioClip));
//    } 
//    ResultsViewController_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);
//}

MAKE_HOOK_OFFSETLESS(ResultsViewController_DidActivate, void, ResultsViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    if (levelClearedLoader.loaded && addedToHierarchy)
    {   // TODO: Write the audioClip stuff correctly.
        UnityEngine::AudioClip* audioClip = levelClearedLoader.getClip();
        self->levelClearedAudioClip = audioClip;
        //CRASH_UNLESS(il2cpp_utils::SetFieldValue(self, "_levelClearedAudioClip", audioClip));
    }
    ResultsViewController_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);
}

//MAKE_HOOK_OFFSETLESS(SongPreviewPlayer_OnEnable, void, Il2CppObject* self)
//{
//    
//    getLogger().info("is it true: %i", menuMusicLoader.loaded);
//    if(menuMusicLoader.loaded)
//    {
//        Il2CppObject* audioClip = menuMusicLoader.getClip();
//        if(audioClip != nullptr)
//            CRASH_UNLESS(il2cpp_utils::SetFieldValue(self, "_defaultAudioClip", audioClip));
//    }
//    SongPreviewPlayer_OnEnable(self);
//
//}

MAKE_HOOK_OFFSETLESS(SongPreviewPlayer_OnEnable, void, SongPreviewPlayer* self) {
    getLogger().info("is it true: %i", menuMusicLoader.loaded);

    if (menuMusicLoader.loaded)
    {   // TODO: Write the audioClip stuff correctly.
        UnityEngine::AudioClip* audioClip = menuMusicLoader.getClip();
        if (audioClip != nullptr)
            self->defaultAudioClip = audioClip;
            //CRASH_UNLESS(il2cpp_utils::SetFieldValue(self, "_defaultAudioClip", audioClip));
    }
    SongPreviewPlayer_OnEnable(self);
}

MAKE_HOOK_OFFSETLESS(NoteCutSoundEffectManager_Start, void, NoteCutSoundEffectManager* self) {
    if(hitSoundLoader.loaded)
    {
        hitSoundArr = createAudioClipArray(hitSoundLoader);
        self->longCutEffectsAudioClips = hitSoundArr;
        self->shortCutEffectsAudioClips = hitSoundArr;
        //CRASH_UNLESS(il2cpp_utils::SetFieldValue(self, "_longCutEffectsAudioClips", hitSoundArr));
        //CRASH_UNLESS(il2cpp_utils::SetFieldValue(self, "_shortCutEffectsAudioClips", hitSoundArr));
    }
    NoteCutSoundEffectManager_Start(self);
}

MAKE_HOOK_OFFSETLESS(NoteCutSoundEffect_Awake, void, NoteCutSoundEffect* self) {
    if(badHitSoundLoader.loaded)
    {
        badHitSoundArr = createAudioClipArray(badHitSoundLoader);
        //CRASH_UNLESS(il2cpp_utils::SetFieldValue(self, "_badCutSoundEffectAudioClips", badHitSoundArr));
        self->badCutSoundEffectAudioClips = badHitSoundArr;
    }
    NoteCutSoundEffect_Awake(self);
}

MAKE_HOOK_OFFSETLESS(BasicUIAudioManager_Start, void, BasicUIAudioManager* self) {
    if(menuClickLoader.loaded)
    {
        menuClickArr = createAudioClipArray(menuClickLoader);
        self->clickSounds = menuClickArr;
        //CRASH_UNLESS(il2cpp_utils::SetFieldValue(self, "_clickSounds", menuClickArr));
    }
    BasicUIAudioManager_Start(self);
}

MAKE_HOOK_OFFSETLESS(FireworkItemController_Awake, void, FireworkItemController* self) {
    if(fireworkSoundLoader.loaded)
    {
        fireworkSoundArr = createAudioClipArray(fireworkSoundLoader);
        self->explosionClips = fireworkSoundArr;
        //CRASH_UNLESS(il2cpp_utils::SetFieldValue(self, "_explosionClips", fireworkSoundArr));
    }
    FireworkItemController_Awake(self);
}

//MAKE_HOOK_OFFSETLESS(SceneManager_ActiveSceneChanged, void, Scene previousActiveScene, Scene nextActiveScene) {
//    SceneManager_ActiveSceneChanged(previousActiveScene, nextActiveScene);
//    Il2CppString* activeSceneName = CRASH_UNLESS(il2cpp_utils::RunMethod<Il2CppString*>(il2cpp_utils::GetClassFromName("UnityEngine.SceneManagement", "Scene"), "GetNameInternal", nextActiveScene.m_Handle));
//    std::string activeSceneStr  = to_utf8(csstrtostr(activeSceneName));
//    getLogger().info("Scene found: %s",  activeSceneStr.data());
//    
//    std::string shaderWarmup = "ShaderWarmup";
//    if(activeSceneStr == shaderWarmup) loadAudioClips();
//
//
//}

MAKE_HOOK_OFFSETLESS(SceneManager_Internal_ActiveSceneChanged, void, UnityEngine::SceneManagement::Scene prevScene, UnityEngine::SceneManagement::Scene nextScene) {
    if (nextScene.IsValid()) {
        std::string sceneName = to_utf8(csstrtostr(nextScene.get_name()));
        getLogger().info("Scene found: %s", sceneName.data());

        std::string shaderWarmup = "ShaderWarmup";
            if(sceneName == shaderWarmup) loadAudioClips();
        //if (sceneName.find("MenuViewControllers") != std::string::npos) { //Don't use find
        //}
    }
    SceneManager_Internal_ActiveSceneChanged(prevScene, nextScene);
}

/*
MAKE_HOOK_OFFSETLESS(SceneManager_Internal_ActiveSceneChanged, void, UnityEngine::SceneManagement::Scene prevScene, UnityEngine::SceneManagement::Scene nextScene) {
    SceneManager_Internal_ActiveSceneChanged(prevScene, nextScene);
    if(prevScene.IsValid() && nextScene.IsValid()) {
        std::string prevSceneName = to_utf8(csstrtostr(prevScene.get_name()));
        std::string nextSceneName = to_utf8(csstrtostr(nextScene.get_name()));
        getLogger().info("Scene change from %s to %s", prevSceneName.c_str(), nextSceneName.c_str());
*/

extern "C" void setup(ModInfo &info)
{
    info.id = "QuestSounds";
    info.version = VERSION;
    modInfo = info;
    getConfig();
    getLogger().info("Completed setup!");
    getLogger().info("Modloader name: %s", Modloader::getInfo().name.c_str());
}  

extern "C" void load()
{
    il2cpp_functions::Init();
    //QuestUI::Init();

    Logger& hkLog = getLogger();

    if(!LoadConfig()) SaveConfig();
    makeFolder();
    getLogger().debug("Installing QuestSounds!");
    //auto* SM_ActiveSceneChanged =   il2cpp_utils::FindMethodUnsafe("UnityEngine.SceneManagement", "SceneManager", "Internal_ActiveSceneChanged", 2);
    //auto* RVC_DidActivate =         il2cpp_utils::FindMethodUnsafe("", "ResultsViewController", "DidActivate", 3);
    //auto* SPP_OnEnable =            il2cpp_utils::FindMethodUnsafe("", "SongPreviewPlayer", "OnEnable", 0);
    //auto* NCSEM_Start =             il2cpp_utils::FindMethodUnsafe("", "NoteCutSoundEffectManager", "Start", 0);
    //auto* BUIAM_Start =             il2cpp_utils::FindMethodUnsafe("", "BasicUIAudioManager", "Start", 0);
    //auto* NCSE_Awake =              il2cpp_utils::FindMethodUnsafe("", "NoteCutSoundEffect", "Awake", 0);
    //auto* FIC_Awake =               il2cpp_utils::FindMethodUnsafe("", "FireworkItemController", "Awake", 0);
    //INSTALL_HOOK_OFFSETLESS(hkLog, SceneManager_ActiveSceneChanged, il2cpp_utils::FindMethodUnsafe("UnityEngine::SceneManagement", "SceneManager", "Internal_ActiveSceneChanged", 2));
    INSTALL_HOOK_OFFSETLESS(hkLog, SceneManager_Internal_ActiveSceneChanged, il2cpp_utils::FindMethodUnsafe("UnityEngine.SceneManagement", "SceneManager", "Internal_ActiveSceneChanged", 2));
    INSTALL_HOOK_OFFSETLESS(hkLog, SongPreviewPlayer_OnEnable, il2cpp_utils::FindMethodUnsafe("", "SongPreviewPlayer", "OnEnable", 0));
    INSTALL_HOOK_OFFSETLESS(hkLog, NoteCutSoundEffectManager_Start, il2cpp_utils::FindMethodUnsafe("", "NoteCutSoundEffectManager", "Start", 0));
    INSTALL_HOOK_OFFSETLESS(hkLog, NoteCutSoundEffect_Awake, il2cpp_utils::FindMethodUnsafe("", "NoteCutSoundEffect", "Awake", 0));
    INSTALL_HOOK_OFFSETLESS(hkLog, FireworkItemController_Awake, il2cpp_utils::FindMethodUnsafe("", "FireworkItemController", "Awake", 0));
    INSTALL_HOOK_OFFSETLESS(hkLog, BasicUIAudioManager_Start, il2cpp_utils::FindMethodUnsafe("", "BasicUIAudioManager", "Start", 0));
    INSTALL_HOOK_OFFSETLESS(hkLog, ResultsViewController_DidActivate, il2cpp_utils::FindMethodUnsafe("", "ResultsViewController", "DidActivate", 3));
    getLogger().debug("Installed QuestSounds!");
}
