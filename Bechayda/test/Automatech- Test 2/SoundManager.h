#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>
#include <memory>

class SoundManager {
private:
    std::unordered_map<std::string, std::unique_ptr<sf::SoundBuffer>> soundBuffers;
    std::unordered_map<std::string, sf::Sound> sounds;
    std::unique_ptr<sf::Music> currentMusic;
    float musicVolume = 100.0f;
    float soundVolume = 100.0f;

    SoundManager() = default;

public:
    static SoundManager& getInstance() {
        static SoundManager instance;
        return instance;
    }

    // Load sound effect
    bool loadSound(const std::string& name, const std::string& filename) {
        auto buffer = std::make_unique<sf::SoundBuffer>();
        if (!buffer->loadFromFile(filename)) return false;
        sounds[name] = sf::Sound();
        sounds[name].setBuffer(*buffer);
        soundBuffers[name] = std::move(buffer);
        return true;
    }

    void playSound(const std::string& name) {
        if (sounds.find(name) != sounds.end()) {
            sounds[name].setVolume(soundVolume);
            sounds[name].play();
        }
    }

    void setSoundVolume(float volume) {
        soundVolume = volume;
    }

    // Load and play background music
    bool playMusic(const std::string& filename, bool loop = true) {
        stopMusic();  // Stop any existing music
        currentMusic = std::make_unique<sf::Music>();
        if (!currentMusic->openFromFile(filename)) return false;
        currentMusic->setLoop(loop);
        currentMusic->setVolume(musicVolume);
        currentMusic->play();
        return true;
    }

    void stopMusic() {
        if (currentMusic) currentMusic->stop();
    }

    void pauseMusic() {
        if (currentMusic) currentMusic->pause();
    }

    void resumeMusic() {
        if (currentMusic) currentMusic->play();
    }

    void setMusicVolume(float volume) {
        musicVolume = volume;
        if (currentMusic) currentMusic->setVolume(musicVolume);
    }

    // Prevent copying
    SoundManager(const SoundManager&) = delete;
    SoundManager& operator=(const SoundManager&) = delete;
};

#endif