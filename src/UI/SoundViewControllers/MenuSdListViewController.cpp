#include "main.hpp"
#include "QSoundsConfig.hpp"
#include "ViewControllers/MenuSdListViewController.hpp"
#include "AudioClips.hpp"
//#include "ObjectInstances.hpp"

#include <dirent.h>
#include <regex>
#include <list>

#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"
#include "questui/shared/CustomTypes/Components/ExternalComponents.hpp"

#include "UnityEngine/Object.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "UnityEngine/UI/Button.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"
#include "UnityEngine/UI/VerticalLayoutGroup.hpp"
#include "UnityEngine/UI/HorizontalLayoutGroup.hpp"
//#include "UnityEngine/UI/Button.hpp"
#include "UnityEngine/Events/UnityAction.hpp"
#include "TMPro/TextMeshProUGUI.hpp"

#include "GlobalNamespace/SongPreviewPlayer.hpp"

#include "System/Action.hpp"
#include "System/Threading/Tasks/Task_1.hpp"
#include "System/Threading/ThreadStart.hpp"


#include "GlobalNamespace/HMTask.hpp"

using namespace QuestSounds;
DEFINE_TYPE(MenuSdListViewController);

GlobalNamespace::SongPreviewPlayer* SPP;

MenuSdListViewController* MenuListView;
std::list<UnityEngine::UI::Button*> MenuQSlist = {};

//void OnChangeEnabled(bool newval)
//{
//    auto& modcfg = getConfig().config;
//    MenuListView->SDlistscroll->SetActive(newval);
//}

void MenuSelectSound()
{
    for (UnityEngine::UI::Button* button : MenuQSlist)
    {
        if (button->hasSelection)
        {
            std::string filename = to_utf8(csstrtostr(button->GetComponentInChildren<TMPro::TextMeshProUGUI*>()->get_text()));
            QSoundsConfig::Config.menuMusic_filepath = QSoundsConfig::soundPath + filename;
            QuestSounds::AudioClips::menuMusicLoader.filePath = QSoundsConfig::Config.menuMusic_filepath;
            //if (AudioClips::menuMusicLoader.load() && AudioClips::menuMusicLoader.loaded) ObjectInstances::SPP->CrossfadeToNewDefault(AudioClips::menuMusicLoader.getClip());
            //GlobalNamespace::HMTask::New_ctor(il2cpp_utils::MakeDelegate<System::Action*>(classof(System::Action*),
            //    (std::function<void()>)[=] {
            AudioClips::menuMusicLoader.loaded = false;
            AudioClips::menuMusicLoader.load();
                    //if (AudioClips::menuMusicLoader.load(true) && AudioClips::menuMusicLoader.loaded) {
                        //UnityEngine::AudioClip* tempClicp = AudioClips::menuMusicLoader.getClip();
                        //ObjectInstances::SPP->CrossfadeToNewDefault(tempClicp);
                    //    //SPP->defaultAudioClip = tempClicp;
                    //    //SPP->defaultAudioClip = tempClicp;
                    //    //SPP->CrossFadeToDefault();
                    //}
                    //else getLogger().error("Sound with Path %s not loaded", QSoundsConfig::Config.menuMusic_filepath.c_str());
            //    }
            //), nullptr)->Run();
            getLogger().debug("Selected Sound Path %s", QSoundsConfig::Config.menuMusic_filepath.c_str());

        }
    }
}

void MenuRefreshList()
{
    if (MenuListView->listtxtgroup && MenuListView->listtxtgroup->m_CachedPtr.m_value) UnityEngine::GameObject::Destroy(MenuListView->listtxtgroup->get_gameObject());
    for (UnityEngine::UI::Button* button : MenuQSlist) UnityEngine::Object::Destroy(button->get_transform()->get_parent()->get_gameObject());
    MenuQSlist = {};
    DIR* sounddir = opendir(QSoundsConfig::MenuMusicPath.c_str());
    dirent* fileent;
    while ((fileent = readdir(sounddir)) != NULL)
    {
        std::string filename = fileent->d_name;
        for (char& ch : filename) ch = tolower(ch);

        if (std::regex_search(filename, std::regex(".ogg|.mp3|.wav|.aiff|.aif")))
        {
            UnityEngine::UI::HorizontalLayoutGroup* rowgroup = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(MenuListView->SDlistscroll->get_transform());
            UnityEngine::UI::Button* button = QuestUI::BeatSaberUI::CreateUIButton(rowgroup->get_rectTransform(), fileent->d_name, MenuSelectSound);
            button->get_gameObject()->GetComponentInChildren<TMPro::TextMeshProUGUI*>()->set_fontStyle(2);
            MenuQSlist.push_back(button);
        }
    }
    if (MenuQSlist.size() < 1)
    {
        MenuListView->listtxtgroup = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(MenuListView->SDlistscroll->get_transform());
        QuestUI::BeatSaberUI::CreateText(MenuListView->listtxtgroup->get_rectTransform(), "No sound files were found!\nPlease add a sound into\n"+ QSoundsConfig::MenuMusicPath +"\nfile to continue.", false);
    }
    (void)closedir(sounddir);
}

void MenuSdListViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
{
    MenuListView = this;
    if (firstActivation && addedToHierarchy)
    {
        SPP = UnityEngine::GameObject::FindObjectOfType<GlobalNamespace::SongPreviewPlayer*>();
        UnityEngine::UI::VerticalLayoutGroup* container = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(get_rectTransform());
        container->set_spacing(0.4f);
        container->GetComponent<UnityEngine::UI::LayoutElement*>()->set_minWidth(125.0);


         //Bool settings
        this->QSconfigcontainer = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(container->get_rectTransform());
        QSconfigcontainer->set_childAlignment(UnityEngine::TextAnchor::UpperCenter);
        QSconfigcontainer->set_childForceExpandHeight(false);
        QSconfigcontainer->set_childControlHeight(true);

        // Enable or Disable BadHitSounds
        QSoundsConfig::QSAddConfigValueToggle(QSconfigcontainer->get_rectTransform(), "Custom Menu Music", QSoundsConfig::Config.menuMusic_Active, SDlistscroll, "Activates or deactivates Custom Menu Music");


        //QSconfigcontainer->set_childForceExpandHeight(false);
        //QSconfigcontainer->set_childControlHeight(true);

        //UnityEngine::UI::Button::Button();

        //bool enabled_initval = getConfig().config["enabled"].GetBool();
        //this->masterEnabled = QuestUI::BeatSaberUI::CreateToggle(QSconfigcontainer->get_rectTransform(), "Enable Quest Sounds", enabled_initval, UnityEngine::Vector2(0, 0), OnChangeEnabled);

        // Sound List (recursively adds buttons as MenuListView isn't an easy type to deal with)
        this->SDlistscroll = QuestUI::BeatSaberUI::CreateScrollView(container->get_rectTransform());
        SDlistscroll->GetComponent<QuestUI::ExternalComponents*>()->Get<UnityEngine::UI::LayoutElement*>()->set_minHeight(56.0);
        auto* SDlistcontainer = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(SDlistscroll->get_transform());
        SDlistcontainer->set_childAlignment(UnityEngine::TextAnchor::UpperCenter);
        SDlistcontainer->set_childForceExpandHeight(false);
        SDlistcontainer->set_childControlHeight(true);

        //this->SDlistscroll->get_gameObject()->SetActive(enabled_initval);
        this->SDlistscroll->get_gameObject()->SetActive(QSoundsConfig::Config.menuMusic_Active);
    }
    MenuRefreshList();
}

void MenuSdListViewController::DidDeactivate(bool removedFromHierarchy, bool systemScreenDisabling)
{
    for (UnityEngine::UI::Button* button : MenuQSlist) UnityEngine::Object::Destroy(button->get_transform()->get_parent()->get_gameObject());
    MenuQSlist = {};
    SPP->CrossfadeToNewDefault(AudioClips::menuMusicLoader.getClip());
}