#include "QSoundsConfig.hpp"
#include "AsyncAudiocliploader.hpp"
#include "main.hpp"
#include "ObjectInstances.hpp"

#include "System/Collections/IEnumerator.hpp"
#include "custom-types/shared/coroutine.hpp"

#include <dirent.h>
#include <regex>

#include <chrono>

//using namespace audioClipLoader;

#include "UnityEngine\Networking\DownloadHandlerAudioClip.hpp"
#include "UnityEngine\Networking\UnityWebRequestMultimedia.hpp"
using namespace UnityEngine::Networking;

#include "System/Threading/Tasks/Task.hpp"
#include "System/Threading/Tasks/Task_1.hpp"
#include "System/Action_1.hpp"
using namespace System;

#include "UnityEngine/AsyncOperation.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/MonoBehaviour.hpp" // Added incase it's somehow needed
using namespace UnityEngine;

#include "GlobalNamespace/SharedCoroutineStarter.hpp"

custom_types::Helpers::Coroutine AsyncAudioClipLoader::loader::GetAudioClip(System::Action_1<UnityEngine::AsyncOperation*>* actionUWRM, AsyncAudioClipLoader::loader* self, int audioType, Il2CppString* filePathStr) {
    //getLogger().debug("Coroutine start");
    self->audioClipRequest = UnityEngine::Networking::UnityWebRequestMultimedia::GetAudioClip(filePathStr, audioType);
    {
        //getLogger().debug("audioClipRequest set");

        co_yield reinterpret_cast<System::Collections::IEnumerator*>(self->audioClipAsync = self->audioClipRequest->SendWebRequest());

        if (self->audioClipRequest->get_isHttpError() || self->audioClipRequest->get_isNetworkError()) {

            //getLogger().error("Web Request Error: %s", to_utf8(csstrtostr(self->audioClipRequest->get_error())).c_str());
            
            co_return;
        }
        else {
            //Stage 2
            //getLogger().info("Coroutine add_completed method");
            self->audioClipAsync->add_completed(actionUWRM);
            co_return;
        }
        //co_return;

    }
}


    //int getAudioType(std::string path) {
    //if (path.ends_with(".ogg")) {
    //    getLogger().debug("File is ogg");
    //    return 0xE;
    //} else if (path.ends_with(".wav")) {
    //    getLogger().debug("File is wav");
    //    return 0x14;
    //} else if (path.ends_with(".mp3")) {
    //    getLogger().debug("File is mp3");
    //    return 0xD;
    //}
    //return 0;
    //}

int getAudioType(std::string path) {
    //if (path.ends_with(".ogg")) {
    //    getLogger().debug("File is ogg");
    //    return 0xE;
    //}
    /*else*/ if (path.ends_with(".wav")) {
        getLogger().debug("File is wav");
        return 0x14;
    }
    else if (path.ends_with(".mp3")) {
        getLogger().debug("File is mp3");
        return 0xD;
    }
    else if (path.ends_with(".aiff") || path.ends_with(".aif")) {
        getLogger().debug("File is aiff");
        return 2;
    }
    return 0;
}

System::Threading::Tasks::Task_1<UnityEngine::AudioClip*>* audioClipTask;
//bool NeedReload = false;

bool AsyncAudioClipLoader::loader::load()
{
    // C++ Shell example of the below filePath Checks https://onlinegdb.com/BJ5RfolI_
    //Stage 0 
    static const char* FileExtensions[] =
    { "mp3","wav","aiff"};
    bool fileError;
    getLogger().info("Starting Stage 0");
    getLogger().info("FilePath to check try 1 is %s", filePath.c_str());
    Il2CppString* filePathStr;
    std::string CheckPath = filePath;
    if (filePath.starts_with("https://") || filePath.starts_with("http://")) {
        getLogger().info("Filepath is URL, skipping file checks and try loading it");
        filePathStr = il2cpp_utils::newcsstr(filePath);
        fileError = true;
    }
    else {
        // Checks if the given File in the config exists and
        // if that file doesn't exist check if the same file with one of the supported extensions exists file extension exists
        if (!fileexists(filePath)) {
            //if (std::regex_search(filePath, std::regex(".mp3|.wav|.aiff")))
            //{
                for (int i = 0; i < sizeof(FileExtensions)/sizeof(char*); ++i) {
                    fileError = fileexists(CheckPath.replace(CheckPath.length() - 3, 3, FileExtensions[i]));
                    getLogger().info("FilePath to check try %d is %s", i+2, CheckPath.c_str());
                    if (fileError) {
                        filePath = CheckPath;
                        break;
                    }
                }
            //}
            //fileError = fileexists(CheckPath.replace(CheckPath.length() - 3, 3, "mp3"));
            //getLogger().info("FilePath to check try 2 is %s", CheckPath.c_str());

            //if (fileError) {
            //    filePath = CheckPath;
            //    getLogger().warning("Could not find file set in Config, but found file with mp3 extension: %s", filePath.c_str());
            //}
            //else {
            //    getLogger().error("Could not find file with mp3 extension, checking for (1) in filename");
            //    fileError = fileexists(CheckPath.insert(CheckPath.length() - 4, " (1)"));
            //    getLogger().info("FilePath to check try 3 is %s", CheckPath.c_str());
            //    if (fileError) {
            //        filePath = CheckPath;
            //        getLogger().warning("Found mp3 file with (1) in name: %s", filePath.c_str());
            //    }
            //    else {
            //        fileError = fileexists(CheckPath.replace(CheckPath.length() - 3, 3, "ogg"));
            //        getLogger().info("FilePath to check try 4 is %s", CheckPath.c_str());
            //        if (fileError) {
            //            filePath = CheckPath;
            //            getLogger().warning("Found file with (1) in name: %s", filePath.c_str());
            //        }
            //        else { getLogger().error("All checks failed, no soundfile found"); }
            //    }
            //}
        }
        else {
            fileError = true;
        }
        getLogger().info("File error is %d", fileError);
        //bool fileError = true;
        //if(audioSource != nullptr && ReloadMenuMusic) bool NeedReload = true;
        //if (audioSource) UnityEngine::Object::Destroy(audioSource);
        //bool error = (/*audioClipAsync != nullptr ||*/ /*audioSource != nullptr ||*/ !fileError);
        //getLogger().info("error is %d", error);
        if (!fileError)
        {
            if (!fileError) // If fileError false, file not found
            {
                getLogger().error("File %s not found", filePath.c_str());
            }
            else
            {
                getLogger().error("Something went wrong when trying to load", filePath.c_str());
            }
            getLogger().error("Stage 0 Failed");
            return false;
        }
        else { getLogger().info("Stage 0 Done with %s", filePath.c_str()); }
    }

    if (filePath.ends_with(".ogg")) {
        filePathStr = il2cpp_utils::newcsstr(filePath);
        isOGG = true;
    }
    else {
        if (filePath.starts_with("https://") || filePath.starts_with("http://")) {
            filePathStr = il2cpp_utils::newcsstr(filePath);
        }
        else {
            filePathStr = il2cpp_utils::newcsstr("file:///" + filePath);
        }
        isOGG = false;
    }

    ////Stage 1
    if (!isOGG) {
        getLogger().debug("Running UnityWebRequestMultimedia");
        //audioClipRequest = UnityEngine::Networking::UnityWebRequestMultimedia::GetAudioClip(filePathStr, audioType);
        //audioClipAsync = audioClipRequest->SendWebRequest();
        //                                 ^ 
        // TODO: Figure out why it crashes here sometimes: 161:44
        audioType = getAudioType(filePath);

        auto actionUWRM = il2cpp_utils::MakeDelegate<System::Action_1<UnityEngine::AsyncOperation*>*>(classof(System::Action_1<UnityEngine::AsyncOperation*>*), this, audioClipCompleted);

        GlobalNamespace::SharedCoroutineStarter::get_instance()->StartCoroutine(reinterpret_cast<System::Collections::IEnumerator*>(custom_types::Helpers::CoroutineHelper::New(AsyncAudioClipLoader::loader::GetAudioClip(actionUWRM, this, audioType, filePathStr))));
    }
    else {
        getLogger().debug("Running MediaAsyncLoader");
        audioClipTask = GlobalNamespace::MediaAsyncLoader::LoadAudioClipAsync(filePathStr, System::Threading::CancellationToken::get_None());
        
        ////Stage 2
        auto actionMAL = il2cpp_utils::MakeDelegate<System::Action_1<System::Threading::Tasks::Task*>*>(classof(System::Action_1<System::Threading::Tasks::Task*>*), this, audioClipCompleted);
        reinterpret_cast<System::Threading::Tasks::Task*>(audioClipTask)->ContinueWith(actionMAL);
    }
    getLogger().debug("Stage 2 done with filepath %s", filePath.c_str());
    return true;
}

void AsyncAudioClipLoader::loader::audioClipCompleted(loader* obj, Il2CppObject* asyncOp)
{
    auto start = std::chrono::high_resolution_clock::now();

    // Stage 1
    getLogger().debug("isOGG %d", obj->isOGG);
    UnityEngine::AudioClip* temporaryClip = nullptr;
    if (obj->isOGG) temporaryClip = obj->audioClipTask->get_ResultOnSuccess();
    else temporaryClip = UnityEngine::Networking::DownloadHandlerAudioClip::GetContent(obj->audioClipRequest); // TODO: This method takes too long

    if(temporaryClip != nullptr)
    {  
        static auto goName = il2cpp_utils::createcsstr("AudioClipGO", il2cpp_utils::StringType::Manual);
        GameObject* audioClipGO = GameObject::New_ctor(goName);
        obj->audioSource = audioClipGO->AddComponent<AudioSource*>();
        obj->audioSource->set_playOnAwake(false);
        obj->audioSource->set_clip(temporaryClip);
        UnityEngine::Object::DontDestroyOnLoad(audioClipGO);
        obj->loaded = true;
        getLogger().debug("Stage 1 done with temporaryClip");
    }
    auto end_final = std::chrono::high_resolution_clock::now();
    double time_taken_Total = std::chrono::duration_cast<std::chrono::nanoseconds>(end_final - start).count();
    time_taken_Total *= 1e-9;
    getLogger().debug("Time taken for audioClipCompleted %lf", time_taken_Total);


    // Finished
}

UnityEngine::AudioClip* AsyncAudioClipLoader::loader::getClip()
{
    if(audioSource != nullptr && loaded)
    {
        getLogger().debug("AudioClip returned");
        return audioSource->get_clip();
    } else
    {
        getLogger().debug("nullptr returned: is AudioSource null: %d, is loaded: %d", audioSource, loaded);
        return nullptr;
    }
}


