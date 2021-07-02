#if defined(BS__1_16)
#elif defined(BS_1_13_2)
#else
#error Define BS Version
#endif

#include "main.hpp"
//#include "audiocliploader.hpp"
#include "AsyncAudiocliploader.hpp"
#include "QSoundsConfig.hpp"
using namespace QSoundsConfig;
//using namespace audioClipLoader;
#include "AudioClips.hpp"
using namespace QuestSounds::AudioClips;
//#include "ObjectInstances.hpp"

#include "GlobalNamespace/ResultsViewController.hpp"
#include "GlobalNamespace/SongPreviewPlayer.hpp"
#include "GlobalNamespace/NoteCutSoundEffectManager.hpp"
#include "GlobalNamespace/NoteCutSoundEffect.hpp"
#include "GlobalNamespace/BasicUIAudioManager.hpp"
#include "GlobalNamespace/FireworkItemController.hpp"
#include "GlobalNamespace/GameServerLobbyFlowCoordinator.hpp"
#include "GlobalNamespace/MultiplayerModeSelectionFlowCoordinator.hpp"

#include "GlobalNamespace/HMTask.hpp"

using namespace GlobalNamespace;

#include "System/Action.hpp"

#include "UnityEngine/SceneManagement/Scene.hpp"
//using namespace UnityEngine::SceneManagement;

#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/QuestUI.hpp"
using namespace QuestUI;

#include "ViewControllers/MenuSdListViewController.hpp"
#include "ViewControllers/HitSdListViewController.hpp"
#include "ViewControllers/MenuClickSdListViewController.hpp"
#include "ViewControllers/BadHitSdListViewController.hpp"
#include "ViewControllers/FireworkSdListViewController.hpp"
#include "ViewControllers/LevelClearedSdListViewController.hpp"
#include "ViewControllers/LobbyMusicSdListViewController.hpp"
#include "ViewControllers/ConfigViewController.hpp"
#include "QSoundsFlowCoordinator.hpp"
using namespace QuestSounds;

#include "custom-types/shared/register.hpp"
using namespace custom_types;

#define RAPIDJSON_HAS_STDSTRING 1

ModInfo modInfo;

Configuration& getConfig() {
    static Configuration config(modInfo);
    //config.Load();
    return config;
}


Logger& getLogger() {
    static auto logger = new Logger(modInfo, LoggerOptions(false, true));
    return *logger;
}

void makeFolder() 
{
    //if(!direxists(soundPath.c_str()))
    //{
    //    int makePath = mkpath(soundPath.data());
    //    if(makePath == -1)
    //    {
    //        getLogger().debug("Failed to make path!");
    //    }
    //}
    
    if (!direxists(MenuMusicPath.data()))
    {
        int makePath = mkpath(MenuMusicPath.data());
        if (makePath == -1)
        {
            getLogger().debug("Failed to make MenuMusic Folder path!");
        }
    }

    if (!direxists(HitSoundPath.data()))
    {
        int makePath = mkpath(HitSoundPath.data());
        if (makePath == -1)
        {
            getLogger().debug("Failed to make HitSound Folder path!");
        }
    }

    if (!direxists(BadHitSoundPath.data()))
    {
        int makePath = mkpath(BadHitSoundPath.data());
            if (makePath == -1)
            {
                getLogger().debug("Failed to make BadHitSound Folder path!");
            }
    }    

    if (!direxists(MenuClickPath.data()))
    {
        int makePath = mkpath(MenuClickPath.data());
        if (makePath == -1)
        {
            getLogger().debug("Failed to make MenuClick Folder path!");
        }
    }

    if (!direxists(FireworkSoundPath.data()))
    {
        int makePath = mkpath(FireworkSoundPath.data());
        if (makePath == -1)
        {
            getLogger().debug("Failed to make FireworkSound Folder path!");
        }
    }

    if (!direxists(LevelClearedPath.data()))
    {
        int makePath = mkpath(LevelClearedPath.data());
        if (makePath == -1)
        {
            getLogger().debug("Failed to make LevelCleared Folder path!");
        }
    }
#ifndef BS__1_13_2
    if (!direxists(LobbyMusicPath.data()))
    {
        int makePath = mkpath(LobbyMusicPath.data());
        if (makePath == -1)
        {
            getLogger().debug("Failed to make LevelCleared Folder path!");
        }
    }
#endif
}



namespace QuestSounds::AudioClips {
/*audioClipLoader::loader*/
AsyncAudioClipLoader::loader    hitSoundLoader,     // hitSound
                                badHitSoundLoader,  // badHitSound
                                menuMusicLoader,    // menuMusic
                                menuClickLoader,
                                fireworkSoundLoader,
                                levelClearedLoader,
#ifndef BS__1_13_2
                                lobbyAmbienceLoader;    // Added for LobbyMusic
#endif
Array<UnityEngine::AudioClip*> *hitSoundArr,    // hitSoundArray
                               *badHitSoundArr, // badHitSoundArray
                               *menuClickArr,
                               *fireworkSoundArr;


Array<UnityEngine::AudioClip*> *origBadHitSoundArr, // badHitSoundArray
                               *origMenuClickArr,
                               *origFireworkSoundArr;


    void loadAudioClips()
    {
        hitSoundLoader.filePath = Config.hitSound_filepath;
        badHitSoundLoader.filePath = Config.badHitSound_filepath;
        menuMusicLoader.filePath = Config.menuMusic_filepath;
        menuClickLoader.filePath = Config.menuClick_filepath;
        fireworkSoundLoader.filePath = Config.firework_filepath;
        levelClearedLoader.filePath = Config.levelCleared_filepath;
#ifndef BS__1_13_2
        lobbyAmbienceLoader.filePath = Config.lobbyAmbience_filepath; // Added for LobbyMusic
#endif

        if (Config.hitSound_Active) GlobalNamespace::HMTask::New_ctor(il2cpp_utils::MakeDelegate<System::Action*>(classof(System::Action*), (std::function<void()>)[=] { hitSoundLoader.load(); }), nullptr)->Run();
        if (Config.badHitSound_Active) GlobalNamespace::HMTask::New_ctor(il2cpp_utils::MakeDelegate<System::Action*>(classof(System::Action*), (std::function<void()>)[=] { badHitSoundLoader.load(); }), nullptr)->Run();
        if (Config.menuMusic_Active) GlobalNamespace::HMTask::New_ctor(il2cpp_utils::MakeDelegate<System::Action*>(classof(System::Action*), (std::function<void()>)[=] { menuMusicLoader.load(); }), nullptr)->Run();
        if (Config.menuClick_Active) GlobalNamespace::HMTask::New_ctor(il2cpp_utils::MakeDelegate<System::Action*>(classof(System::Action*), (std::function<void()>)[=] { menuClickLoader.load(); }), nullptr)->Run();
        if (Config.firework_Active) GlobalNamespace::HMTask::New_ctor(il2cpp_utils::MakeDelegate<System::Action*>(classof(System::Action*), (std::function<void()>)[=] { fireworkSoundLoader.load(); }), nullptr)->Run();
        if (Config.levelCleared_Active) GlobalNamespace::HMTask::New_ctor(il2cpp_utils::MakeDelegate<System::Action*>(classof(System::Action*), (std::function<void()>)[=] { levelClearedLoader.load(); }), nullptr)->Run();
#ifndef BS__1_13_2
        if (Config.lobbyAmbience_Active) GlobalNamespace::HMTask::New_ctor(il2cpp_utils::MakeDelegate<System::Action*>(classof(System::Action*), (std::function<void()>)[=] { lobbyAmbienceLoader.load(); }), nullptr)->Run();    // Added for LobbyMusic
#endif
        //if (Config.hitSound_Active) {
        //    std::thread hitSoundThread(hitSoundLoader.load());
        //    hitSoundThread.detach();
        //}
        //if (Config.badHitSound_Active) std::thread(badHitSoundLoader.load()).detach();
        //if (Config.menuMusic_Active) std::thread(menuMusicLoader.load()).detach();
        //if (Config.menuClick_Active) std::thread(menuClickLoader.load()).detach();
        //if (Config.firework_Active) std::thread(fireworkSoundLoader.load()).detach();
        //if (Config.levelCleared_Active) std::thread(levelClearedLoader.load()).detach();
        //if (Config.lobbyAmbience_Active) std::thread(lobbyAmbienceLoader.load()).detach();    // Added for LobbyMusic


        //if (Config.hitSound_Active) hitSoundLoader.load();
        //if (Config.badHitSound_Active) badHitSoundLoader.load();
        //if (Config.menuMusic_Active) menuMusicLoader.load();
        //if (Config.menuClick_Active) menuClickLoader.load();
        //if (Config.firework_Active) fireworkSoundLoader.load();
        //if (Config.levelCleared_Active) levelClearedLoader.load();
        //if (Config.lobbyAmbience_Active) lobbyAmbienceLoader.load();    // Added for LobbyMusic
    }
    // Creates an Array, of AudioClips
    Array<UnityEngine::AudioClip*>* createAudioClipArray(AsyncAudioClipLoader::loader clipLoader)
    {
        UnityEngine::AudioClip* tempClip = clipLoader.getClip();
        auto* temporaryArray = Array<UnityEngine::AudioClip*>::New(tempClip);
        //temporaryArray->values[0] = tempClip;
        return temporaryArray;
    }
}

// TODO: Add LevelFailed sound as option
MAKE_HOOK_OFFSETLESS(ResultsViewController_DidActivate, void, ResultsViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    if (firstActivation && addedToHierarchy) {
        levelClearedLoader.set_OriginalClip(self->levelClearedAudioClip);
    }

    if (levelClearedLoader.loaded && addedToHierarchy && QSoundsConfig::Config.levelCleared_Active)
    {   
        UnityEngine::AudioClip* audioClip = levelClearedLoader.getClip();
        self->levelClearedAudioClip = audioClip;
    }
    else {
        self->levelClearedAudioClip = levelClearedLoader.get_OriginalClip();
    }
    ResultsViewController_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);
}

MAKE_HOOK_OFFSETLESS(SongPreviewPlayer_OnEnable, void, SongPreviewPlayer* self) {
    getLogger().info("is it true: %i", menuMusicLoader.loaded);

    if (menuMusicLoader.OriginalAudioSource == nullptr) {
        menuMusicLoader.set_OriginalClip(self->defaultAudioClip);
    }

    if (menuMusicLoader.loaded && QSoundsConfig::Config.menuMusic_Active)
    {
        UnityEngine::AudioClip* audioClip = menuMusicLoader.getClip();
        if (audioClip != nullptr)
            self->defaultAudioClip = audioClip;
    }
    else {
        self->defaultAudioClip = menuMusicLoader.get_OriginalClip();
    }
    SongPreviewPlayer_OnEnable(self);
}

#ifndef BS__1_13_2
MAKE_HOOK_OFFSETLESS(GameServerLobbyFlowCoordinator_DidActivate, void, GameServerLobbyFlowCoordinator* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
{
    getLogger().info("LobbyMusic is it true: %i", lobbyAmbienceLoader.loaded);
    if (lobbyAmbienceLoader.loaded && QSoundsConfig::Config.lobbyAmbience_Active && addedToHierarchy)
    {
        getLogger().debug("Overwriting LobbyAmbience Audio");
        UnityEngine::AudioClip* audioClip = lobbyAmbienceLoader.getClip();
        self->ambienceAudioClip = audioClip;
    }
    GameServerLobbyFlowCoordinator_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);
}

MAKE_HOOK_OFFSETLESS(GameServerLobbyFlowCoordinator_DidDeactivate, void, GameServerLobbyFlowCoordinator* self, bool removedFromHierarchy, bool screenSystemDisabling)
{
    getLogger().info("LobbyMusic is it true: %i", lobbyAmbienceLoader.loaded);
    if (menuMusicLoader.loaded && QSoundsConfig::Config.menuMusic_Active && removedFromHierarchy)
    {
        getLogger().debug("Switching LobbyMusic to MenuMusic Audio");
        UnityEngine::AudioClip* audioClip = menuMusicLoader.getClip();
        self->ambienceAudioClip = audioClip;
    }
    GameServerLobbyFlowCoordinator_DidDeactivate(self, removedFromHierarchy, screenSystemDisabling);
}
// */

// /*
MAKE_HOOK_OFFSETLESS(MultiplayerModeSelectionFlowCoordinator_DidActivate, void, MultiplayerModeSelectionFlowCoordinator* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
{
    getLogger().debug("MultiplayerModeSelectionFlowCoordinator_DidActivate");

    getLogger().info("LobbyMusic is it true: %i", menuMusicLoader.loaded);
    if (menuMusicLoader.loaded && QSoundsConfig::Config.menuMusic_Active && addedToHierarchy)
    {
        getLogger().debug("Switching LobbyMusic to MenuMusic Audio");
        UnityEngine::AudioClip* audioClip = menuMusicLoader.getClip();
        self->ambienceAudioClip = audioClip;
    }
    else {
        self->ambienceAudioClip = menuMusicLoader.get_OriginalClip();
    }
    MultiplayerModeSelectionFlowCoordinator_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling); // This has to be ran last, otherwise it will not work correctly
}
// /*
MAKE_HOOK_OFFSETLESS(MultiplayerModeSelectionFlowCoordinator_DidDeactivate, void, MultiplayerModeSelectionFlowCoordinator* self, bool removedFromHierarchy, bool screenSystemDisabling)
{
    getLogger().debug("MultiplayerModeSelectionFlowCoordinator_DidDeactivate");
    if (menuMusicLoader.loaded && QSoundsConfig::Config.menuMusic_Active && removedFromHierarchy)
    {
        UnityEngine::AudioClip* audioClip = menuMusicLoader.getClip();
        self->ambienceAudioClip = audioClip;
    }
    MultiplayerModeSelectionFlowCoordinator_DidDeactivate(self, removedFromHierarchy, screenSystemDisabling);
}
#endif
//*/

// TODO: Figure out why HitSounds here broke
//#ifdef HAS_CODEGEN
//#undef HAS_CODEGEN
//#define NO_CODEGEN_USE
//#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
//#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"

Il2CppArray* hitSoundArrIl2Cpp;

Il2CppArray* createAudioClipIl2CppArray(AsyncAudioClipLoader::loader clipLoader)
{
    Il2CppObject* tempClip = (Il2CppObject*)clipLoader.getClip();
    Il2CppArray* temporaryArray = (il2cpp_functions::array_new(il2cpp_utils::GetClassFromName("UnityEngine", "AudioClip"), 1));
    il2cpp_array_set(temporaryArray, Il2CppObject*, 0, tempClip);
    return temporaryArray;
}

#include "GlobalNamespace/RandomObjectPicker_1.hpp"

MAKE_HOOK_OFFSETLESS(NoteCutSoundEffectManager_Start, void, NoteCutSoundEffectManager* self) {
    if(hitSoundLoader.loaded && QSoundsConfig::Config.hitSound_Active)
    {

        //SafePtr<Array<UnityEngine::AudioClip*>*> hitSoundPtr = hitSoundPtr.emplace(createAudioClipArray(hitSoundLoader));
        SafePtr<Array<UnityEngine::AudioClip*>*> HSPtr(hitSoundArr);
        hitSoundArr = createAudioClipArray(hitSoundLoader);
        self->longCutEffectsAudioClips = hitSoundArr;
        self->shortCutEffectsAudioClips = hitSoundArr;
        getLogger().debug("NoteCutSoundEffectManager_Start: Loaded hitSoundArray");

        for (int i = 0; i < self->longCutEffectsAudioClips->Length(); i++) {
            getLogger().debug("Pre-Fix Checking Ptr longCutEffectsAudioClips->values[%d]: %p", i, self->longCutEffectsAudioClips->values[i]);
        }
        for (int i = 0; i < self->shortCutEffectsAudioClips->Length(); i++) {
            getLogger().debug("Pre-Fix Checking Ptr shortCutEffectsAudioClips->values[%d]: %p", i, self->shortCutEffectsAudioClips->values[i]);
        }



        //Array<UnityEngine::AudioClip*>* tempArraylongCut;
        //Array<UnityEngine::AudioClip*>* tempArrayshortCut;

        //tempArraylongCut = self->longCutEffectsAudioClips;
        //getLogger().debug("longCutEffectsAudioClips Length: %d", self->longCutEffectsAudioClips->Length());
        //for (int i = 0; i < self->longCutEffectsAudioClips->Length(); i++) {
        //    self->longCutEffectsAudioClips->values[i] = hitSoundLoader.getClip();
        //    getLogger().debug("Replacing longCutEffectsAudioClips->values[%d]", i);
        //}
        //self->longCutEffectsAudioClips = tempArraylongCut;

        //tempArrayshortCut = self->shortCutEffectsAudioClips;
        //getLogger().debug("shortCutEffectsAudioClips Length: %d", self->shortCutEffectsAudioClips->Length());
        //for (int i = 0; i < self->shortCutEffectsAudioClips->Length(); i++) {
        //    self->shortCutEffectsAudioClips->values[i] = hitSoundLoader.getClip();
        //    getLogger().debug("Replacing shortCutEffectsAudioClips->values[%d]", i);
        //}
        //self->shortCutEffectsAudioClips = tempArrayshortCut;
        //hitSoundArrIl2Cpp = createAudioClipIl2CppArray(hitSoundLoader);
        //CRASH_UNLESS(il2cpp_utils::SetFieldValue(self, "_shortCutEffectsAudioClips", hitSoundArrIl2Cpp));
        //CRASH_UNLESS(il2cpp_utils::SetFieldValue(self, "_longCutEffectsAudioClips", hitSoundArrIl2Cpp));
    }
    else {
        getLogger().debug("NoteCutSoundEffectManager_Start: Loading normally");
    }
    getLogger().debug("NoteCutSoundEffectManager_Start Pre-Fix end");
    NoteCutSoundEffectManager_Start(self);
    getLogger().debug("NoteCutSoundEffectManager_Start Post-Fix start");
    //getLogger().debug("randomLongCutSoundPicker minimumPickInterval: %lf", self->randomLongCutSoundPicker->minimumPickInterval);
    //getLogger().debug("randomLongCutSoundPicker objects->get_Length(): %d", self->randomLongCutSoundPicker->objects->get_Length());

    //getLogger().debug("randomShortCutSoundPicker minimumPickInterval: %lf", self->randomShortCutSoundPicker->minimumPickInterval);
    //getLogger().debug("randomShortCutSoundPicker objects->get_Length(): %d", self->randomShortCutSoundPicker->objects->get_Length());
    
    //hitSoundArr = createAudioClipArray(hitSoundLoader);

    //self->randomShortCutSoundPicker->objects = hitSoundArr;
    //self->randomLongCutSoundPicker->objects = hitSoundArr;
        //for (int i = 0; i < self->randomShortCutSoundPicker->objects->get_Length(); i++) {
        //    self->randomShortCutSoundPicker->objects = hitSoundLoader.getClip();
        //    getLogger().debug("Replacing longCutEffectsAudioClips->values[%d]", i);
        //}
    for (int i = 0; i < self->longCutEffectsAudioClips->Length(); i++) {
        getLogger().debug("Post-Fix Checking Ptr longCutEffectsAudioClips->values[%d]: %p", i, self->longCutEffectsAudioClips->values[i]);
    }
    for (int i = 0; i < self->shortCutEffectsAudioClips->Length(); i++) {
        getLogger().debug("Post-Fix Checking Ptr shortCutEffectsAudioClips->values[%d]: %p", i, self->shortCutEffectsAudioClips->values[i]);
    }
    getLogger().debug("NoteCutSoundEffectManager_Start Finished");

}
//#undef NO_CODEGEN_USE
//#define HAS_CODEGEN
//#endif

MAKE_HOOK_OFFSETLESS(NoteCutSoundEffectManager_HandleNoteWasCut, void, NoteCutSoundEffectManager* self, GlobalNamespace::NoteController* noteController, GlobalNamespace::NoteCutInfo& noteCutInfo) {
    
    NoteCutSoundEffectManager_HandleNoteWasCut(self, noteController, noteCutInfo);
    getLogger().debug("NoteCutSoundEffectManager_HandleNoteWasCut");
    for (int i = 0; i < self->longCutEffectsAudioClips->Length(); i++) {
        getLogger().debug("Post-Fix Checking Ptr longCutEffectsAudioClips->values[%d]: %p", i, self->longCutEffectsAudioClips->values[i]);
    }
    for (int i = 0; i < self->shortCutEffectsAudioClips->Length(); i++) {
        getLogger().debug("Post-Fix Checking Ptr shortCutEffectsAudioClips->values[%d]: %p", i, self->shortCutEffectsAudioClips->values[i]);
    }

}

MAKE_HOOK_OFFSETLESS(NoteCutSoundEffect_Awake, void, NoteCutSoundEffect* self) {
    //if (!origBadHitSoundArr) origBadHitSoundArr = self->badCutSoundEffectAudioClips;
    //
    if(badHitSoundLoader.loaded && QSoundsConfig::Config.badHitSound_Active)
    {
        badHitSoundArr = createAudioClipArray(badHitSoundLoader);
        self->badCutSoundEffectAudioClips = badHitSoundArr;
    }
    //else {
    //    self->badCutSoundEffectAudioClips = origBadHitSoundArr;
    //}
    NoteCutSoundEffect_Awake(self);
}

MAKE_HOOK_OFFSETLESS(BasicUIAudioManager_Start, void, BasicUIAudioManager* self) {
    if(menuClickLoader.loaded && QSoundsConfig::Config.menuClick_Active)
    {
        menuClickArr = createAudioClipArray(menuClickLoader);
        self->clickSounds = menuClickArr;
    }
    BasicUIAudioManager_Start(self);
}

MAKE_HOOK_OFFSETLESS(FireworkItemController_Awake, void, FireworkItemController* self) {
    if(fireworkSoundLoader.loaded && QSoundsConfig::Config.firework_Active)
    {
        fireworkSoundArr = createAudioClipArray(fireworkSoundLoader);
        self->explosionClips = fireworkSoundArr;
    }
    FireworkItemController_Awake(self);
}

MAKE_HOOK_OFFSETLESS(SceneManager_Internal_ActiveSceneChanged, void, UnityEngine::SceneManagement::Scene prevScene, UnityEngine::SceneManagement::Scene nextScene) {
    if (nextScene.IsValid()) {
        std::string sceneName = to_utf8(csstrtostr(nextScene.get_name()));
        getLogger().info("Scene found: %s", sceneName.data());

        std::string shaderWarmup = "ShaderWarmup";
            if(sceneName == shaderWarmup) QuestSounds::AudioClips::loadAudioClips();
    }
    SceneManager_Internal_ActiveSceneChanged(prevScene, nextScene);
}

extern "C" void setup(ModInfo &info)
{
    info.id = ID;
    info.version = VERSION;
    modInfo = info;

    getLogger().info("Modloader name: %s", Modloader::getInfo().name.c_str());
    getLogger().debug("Config Path is: %s", getConfig().getConfigFilePath(modInfo).c_str());
    getLogger().info("Loading Config");
    getConfig();
    getLogger().info("Completed setup!");
}  

extern "C" void load()
{
    il2cpp_functions::Init();
    QuestUI::Init();

    Logger& hkLog = getLogger();

    //custom_types::Register::RegisterType<QuestSounds::QSoundsFlowCoordinator>();
#ifndef REGISTER_FUNCTION
    custom_types::Register::AutoRegister();
#else
    custom_types::Register::RegisterTypes<
        QuestSounds::QSoundsFlowCoordinator,
        QuestSounds::ConfigViewController,
        QuestSounds::MenuSdListViewController,
        QuestSounds::HitSdListViewController,
        QuestSounds::MenuClickSdListViewController,
        QuestSounds::BadHitSdListViewController,
        QuestSounds::FireworkSdListViewController,
        QuestSounds::LevelClearedSdListViewController>();
#ifndef BS__1_13_2
    custom_types::Register::RegisterType<QuestSounds::LobbyMusicSdListViewController>();
#endif
#endif
    QuestUI::Register::RegisterModSettingsFlowCoordinator<QuestSounds::QSoundsFlowCoordinator*>(modInfo);
    //custom_types::Register::RegisterType<QuestSounds::QSoundsViewController>();
    //QuestUI::Register::RegisterModSettingsViewController<QuestSounds::QSoundsViewController*>(modInfo);

    if(!LoadConfig()) SaveConfig();
    makeFolder();

    getLogger().debug("Installing QuestSounds!");
    INSTALL_HOOK_OFFSETLESS(hkLog, SceneManager_Internal_ActiveSceneChanged, il2cpp_utils::FindMethodUnsafe("UnityEngine.SceneManagement", "SceneManager", "Internal_ActiveSceneChanged", 2));
    INSTALL_HOOK_OFFSETLESS(hkLog, SongPreviewPlayer_OnEnable, il2cpp_utils::FindMethodUnsafe("", "SongPreviewPlayer", "OnEnable", 0));
//#ifdef HAS_CODEGEN
//#undef HAS_CODEGEN
//#define NO_CODEGEN_USE
    INSTALL_HOOK_OFFSETLESS(hkLog, NoteCutSoundEffectManager_Start, il2cpp_utils::FindMethodUnsafe("", "NoteCutSoundEffectManager", "Start", 0));
//#define HAS_CODEGEN
//#undef NO_CODEGEN_USE
//#endif
    INSTALL_HOOK_OFFSETLESS(hkLog, NoteCutSoundEffectManager_HandleNoteWasCut, il2cpp_utils::FindMethodUnsafe("", "NoteCutSoundEffectManager", "HandleNoteWasCut", 2));

    INSTALL_HOOK_OFFSETLESS(hkLog, NoteCutSoundEffect_Awake, il2cpp_utils::FindMethodUnsafe("", "NoteCutSoundEffect", "Awake", 0));
    INSTALL_HOOK_OFFSETLESS(hkLog, FireworkItemController_Awake, il2cpp_utils::FindMethodUnsafe("", "FireworkItemController", "Awake", 0));
    INSTALL_HOOK_OFFSETLESS(hkLog, BasicUIAudioManager_Start, il2cpp_utils::FindMethodUnsafe("", "BasicUIAudioManager", "Start", 0));
    INSTALL_HOOK_OFFSETLESS(hkLog, ResultsViewController_DidActivate, il2cpp_utils::FindMethodUnsafe("", "ResultsViewController", "DidActivate", 3));
#ifndef BS__1_13_2
    INSTALL_HOOK_OFFSETLESS(hkLog, MultiplayerModeSelectionFlowCoordinator_DidActivate, il2cpp_utils::FindMethodUnsafe("", "MultiplayerModeSelectionFlowCoordinator", "DidActivate", 3));     // Added for switching out MP Lobby Music
    INSTALL_HOOK_OFFSETLESS(hkLog, MultiplayerModeSelectionFlowCoordinator_DidDeactivate, il2cpp_utils::FindMethodUnsafe("", "MultiplayerModeSelectionFlowCoordinator", "DidDeactivate", 2));  // Added for switching out MP Lobby Music
    INSTALL_HOOK_OFFSETLESS(hkLog, GameServerLobbyFlowCoordinator_DidActivate, il2cpp_utils::FindMethodUnsafe("", "GameServerLobbyFlowCoordinator", "DidActivate", 3));              // Added for switching out MP Lobby Music
    INSTALL_HOOK_OFFSETLESS(hkLog, GameServerLobbyFlowCoordinator_DidDeactivate, il2cpp_utils::FindMethodUnsafe("", "GameServerLobbyFlowCoordinator", "DidDeactivate", 2));          // Added for switching out MP Lobby Music
#endif
    getLogger().debug("Installed QuestSounds!");
}
