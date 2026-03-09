#include "SoundManager.h"

#include <AL/alc.h>
#include <AL/alut.h>
#include <iostream>

namespace Manager {
    int SoundManager::instanceCount = 0;
    bool SoundManager::alutInitialized = false;

    SoundManager::SoundManager() {
        ++instanceCount;
    }

    SoundManager::~SoundManager() {
        clear();
        --instanceCount;

        if (instanceCount == 0 && alutInitialized) {
            alutExit();
            alutInitialized = false;
        }
    }

    bool SoundManager::initialize(int *argc, char **argv) {
        if (alutInitialized) {
            return true;
        }

        if (alutInit(argc, argv) == AL_FALSE) {
            std::cerr << "ALUT init failed: " << alutGetErrorString(alutGetError()) << std::endl;
            return false;
        }

        alutInitialized = true;
        return true;
    }

    bool SoundManager::isInitialized() const {
        return alutInitialized;
    }

    bool SoundManager::addSound(const std::string &id, const ALuint buffer, const bool ownsBuffer, const bool looping) {
        if (!canUseOpenAL()) {
            return false;
        }
        if (id.empty() || buffer == 0) {
            return false;
        }
        if (hasSound(id)) {
            return false;
        }

        ALuint source = 0;
        alGenSources(1, &source);
        if (!checkError("alGenSources")) {
            return false;
        }

        alSourcei(source, AL_BUFFER, static_cast<ALint>(buffer));
        if (!checkError("alSourcei(AL_BUFFER)")) {
            alDeleteSources(1, &source);
            checkError("alDeleteSources");
            return false;
        }

        alSourcei(source, AL_LOOPING, looping ? AL_TRUE : AL_FALSE);
        if (!checkError("alSourcei(AL_LOOPING)")) {
            alDeleteSources(1, &source);
            checkError("alDeleteSources");
            return false;
        }

        sounds.emplace(id, SoundResource{source, buffer, ownsBuffer});
        return true;
    }

    bool SoundManager::addSoundFromFile(const std::string &id, const std::string &path, const bool looping) {
        if (!canUseOpenAL()) {
            return false;
        }
        if (id.empty() || path.empty()) {
            return false;
        }
        if (hasSound(id)) {
            return false;
        }

        const ALuint buffer = alutCreateBufferFromFile(path.c_str());
        if (buffer == AL_NONE) {
            checkError("alutCreateBufferFromFile");
            return false;
        }

        if (!addSound(id, buffer, true, looping)) {
            alDeleteBuffers(1, &buffer);
            checkError("alDeleteBuffers");
            return false;
        }
        return true;
    }

    bool SoundManager::removeSound(const std::string &id) {
        const auto it = sounds.find(id);
        if (it == sounds.end()) {
            return false;
        }

        if (!canUseOpenAL()) {
            sounds.erase(it);
            return true;
        }

        stop(id);

        bool ok = true;
        if (it->second.source != 0) {
            alDeleteSources(1, &it->second.source);
            ok = checkError("alDeleteSources");
        }
        if (it->second.ownsBuffer) {
            alDeleteBuffers(1, &it->second.buffer);
            ok = checkError("alDeleteBuffers") && ok;
        }
        sounds.erase(it);
        return ok;
    }

    void SoundManager::clear() {
        if (!canUseOpenAL()) {
            sounds.clear();
            return;
        }

        for (auto &[id, sound] : sounds) {
            stop(id);
            if (sound.source != 0) {
                alDeleteSources(1, &sound.source);
                checkError("alDeleteSources");
            }
            if (sound.ownsBuffer) {
                alDeleteBuffers(1, &sound.buffer);
                checkError("alDeleteBuffers");
            }
        }
        sounds.clear();
    }

    bool SoundManager::play(const std::string &id, const std::initializer_list<std::pair<ALenum, ALint> > properties) {
        if (!canUseOpenAL()) {
            return false;
        }
        const SoundResource *sound = findSound(id);
        if (sound == nullptr) {
            return false;
        }

        for (const auto &[property, value] : properties) {
            alSourcei(sound->source, property, value);
            if (!checkError("alSourcei(play properties)")) {
                return false;
            }
        }

        alSourcePlay(sound->source);
        return checkError("alSourcePlay");
    }

    bool SoundManager::stop(const std::string &id) {
        if (!canUseOpenAL()) {
            return false;
        }
        const SoundResource *sound = findSound(id);
        if (sound == nullptr) {
            return false;
        }
        alSourceStop(sound->source);
        return checkError("alSourceStop");
    }

    bool SoundManager::pause(const std::string &id) {
        if (!canUseOpenAL()) {
            return false;
        }
        const SoundResource *sound = findSound(id);
        if (sound == nullptr) {
            return false;
        }
        alSourcePause(sound->source);
        return checkError("alSourcePause");
    }

    bool SoundManager::rewind(const std::string &id) {
        if (!canUseOpenAL()) {
            return false;
        }
        const SoundResource *sound = findSound(id);
        if (sound == nullptr) {
            return false;
        }
        alSourceRewind(sound->source);
        return checkError("alSourceRewind");
    }

    bool SoundManager::setProperty(const std::string &id, const ALenum property, const ALint value) {
        if (!canUseOpenAL()) {
            return false;
        }
        const SoundResource *sound = findSound(id);
        if (sound == nullptr) {
            return false;
        }
        alSourcei(sound->source, property, value);
        return checkError("alSourcei");
    }

    bool SoundManager::setPropertyf(const std::string &id, const ALenum property, const ALfloat value) {
        if (!canUseOpenAL()) {
            return false;
        }
        const SoundResource *sound = findSound(id);
        if (sound == nullptr) {
            return false;
        }
        alSourcef(sound->source, property, value);
        return checkError("alSourcef");
    }

    bool SoundManager::isPlaying(const std::string &id) const {
        if (!canUseOpenAL()) {
            return false;
        }
        const SoundResource *sound = findSound(id);
        if (sound == nullptr) {
            return false;
        }

        ALint state = AL_STOPPED;
        alGetSourcei(sound->source, AL_SOURCE_STATE, &state);
        if (!checkError("alGetSourcei(AL_SOURCE_STATE)")) {
            return false;
        }
        return state == AL_PLAYING;
    }

    bool SoundManager::hasSound(const std::string &id) const {
        return sounds.contains(id);
    }

    bool SoundManager::checkError(const char *operation) {
        const ALenum error = alGetError();
        if (error == AL_NO_ERROR) {
            return true;
        }
        std::cerr << "OpenAL error after " << operation << ": " << error << std::endl;
        return false;
    }

    bool SoundManager::canUseOpenAL() {
        return alutInitialized && alcGetCurrentContext() != nullptr;
    }

    SoundManager::SoundResource *SoundManager::findSound(const std::string &id) {
        const auto it = sounds.find(id);
        if (it == sounds.end()) {
            return nullptr;
        }
        return &it->second;
    }

    const SoundManager::SoundResource *SoundManager::findSound(const std::string &id) const {
        const auto it = sounds.find(id);
        if (it == sounds.end()) {
            return nullptr;
        }
        return &it->second;
    }
} // Manager
