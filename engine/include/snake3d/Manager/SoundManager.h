#ifndef SNAKE3_SOUNDMANAGER_H
#define SNAKE3_SOUNDMANAGER_H

#include <AL/al.h>
#include <initializer_list>
#include <string>
#include <unordered_map>
#include <utility>

namespace Manager {
    class SoundManager {
    public:
        SoundManager();
        ~SoundManager();

        SoundManager(const SoundManager &) = delete;
        SoundManager &operator=(const SoundManager &) = delete;
        SoundManager(SoundManager &&) = delete;
        SoundManager &operator=(SoundManager &&) = delete;

        bool initialize(int *argc = nullptr, char **argv = nullptr);
        bool isInitialized() const;

        bool addSound(const std::string &id, ALuint buffer, bool ownsBuffer = false, bool looping = false);
        bool addSoundFromFile(const std::string &id, const std::string &path, bool looping = false);
        bool removeSound(const std::string &id);
        void clear();

        bool play(const std::string &id,
                  std::initializer_list<std::pair<ALenum, ALint> > properties = {});
        bool stop(const std::string &id);
        bool pause(const std::string &id);
        bool rewind(const std::string &id);

        bool setProperty(const std::string &id, ALenum property, ALint value);
        bool setPropertyf(const std::string &id, ALenum property, ALfloat value);
        [[nodiscard]] bool isPlaying(const std::string &id) const;
        bool hasSound(const std::string &id) const;

    private:
        struct SoundResource {
            ALuint source{0};
            ALuint buffer{0};
            bool ownsBuffer{false};
        };

        std::unordered_map<std::string, SoundResource> sounds;
        static int instanceCount;
        static bool alutInitialized;

        static bool checkError(const char *operation);
        static bool canUseOpenAL();
        SoundResource *findSound(const std::string &id);
        const SoundResource *findSound(const std::string &id) const;
    };
} // Manager

#endif //SNAKE3_SOUNDMANAGER_H
