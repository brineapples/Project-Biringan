#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Network.hpp>
#include <cmath>
#include <string>

// Include custom headers for game screens and dialogue system
#include "TitleScreen.h"
#include "GameState.h"
#include "AboutScreen.h"
#include "PlayIntro.h"
#include "DialogueBox.h"
#include "BackgroundManager.h"
#include "SoundManager.h"
#include "LoadScreen.h"
#include "SaveManager.h"
#include "GameProgress.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(1700, 900), "Escape from Biringan");

    // Load fonts
    sf::Font titleFont;
    if (!titleFont.loadFromFile("BlackDahlia.ttf")) {
        std::cerr << "Failed to load BlackDahlia.ttf\n";
        return -1;
    }

    sf::Font bodyFont;
    if (!bodyFont.loadFromFile("Railway.ttf")) {
        std::cerr << "Failed to load Railway.ttf\n";
        return -1;
    }

    // Initialize SoundManager 
    SoundManager& soundManager = SoundManager::getInstance();

    // Load sound effects
    if (!soundManager.loadSound("Shock", "Shock.ogg")) {
        std::cerr << "Failed to load shock sound effect.\n";
    }

    // Play background music for the title screen
    if (!soundManager.playMusic("BeginMusic.ogg", true)) {
        std::cerr << "Failed to load background music.\n";
        return -1;
    }

    TitleScreen::GameState state = TitleScreen::GameState::TITLE;
    bool loadedFromSave = false;  // Track if "Load" was chosen

    while (window.isOpen()) {
        if (state == TitleScreen::GameState::TITLE) {
            TitleScreen titleScreen(window, titleFont, bodyFont, soundManager);
            state = titleScreen.run();
            loadedFromSave = false;  // Reset between transitions
        }
        else if (state == TitleScreen::GameState::ABOUT) {
            AboutScreen aboutScreen(window, titleFont, bodyFont);
            if (aboutScreen.run() == AboutScreen::AboutState::BACK) {
                state = TitleScreen::GameState::TITLE;
            }
        }
        else if (state == TitleScreen::GameState::LOAD) {
            LoadScreen loadScreen(window, titleFont, bodyFont);
            LoadScreen::LoadState loadState = loadScreen.run();
            if (loadState == LoadScreen::LoadState::BACK) {
                state = TitleScreen::GameState::TITLE;
            }
            else if (loadState == LoadScreen::LoadState::LOAD_SUCCESS) {
                GameProgress progress;
                if (SaveManager::loadProgress("savegame.dat", progress)) {
                    loadedFromSave = true;  // Set flag
                    state = TitleScreen::GameState::GAMEPLAY;
                }
                else {
                    std::cerr << "Failed to load saved game progress.\n";
                    state = TitleScreen::GameState::TITLE;
                }
            }
        }
        else if (state == TitleScreen::GameState::GAMEPLAY) {
            std::cout << "Starting gameplay...\n";
            //soundManager.playMusic("BeginMusic.ogg", true);

            BackgroundManager bgManager(window);
            DialogueBox dialogueBox(window, bodyFont);

            if (loadedFromSave) {
                GameProgress progress;
                if (SaveManager::loadProgress("savegame.dat", progress)) {
                    bgManager.setBackground(progress.backgroundImage);
                    std::vector<DialogueEntry> resume = {
                        {progress.dialogue, progress.speaker}
                    };
                    dialogueBox.startDialogue(resume);
                }
            }
            else {
                bgManager.setBackground("bgs.png");
                std::vector<DialogueEntry> dialogues = {
                    {"Dito ba talaga ang daan?", "???"},
                    {"Dapat talaga nagpasama ko. Masyadong madilim ang kalsada.", "???"},
                    {"Kaliwa ba o Kanan yung pupuntahan?", "???"},
                    {"“Apo, puntahan mo nga muna bahay nila Auntie Amelita mo. Bigyan mo sya nang \nnatirang maja.”", "Flashback(Mother)"},
                    {"Saan bahay ni Auntie?", "???"},
                    {"Tapat bahay nya sa municipal hall. Pangatlong bahay na may third floor.\nNaintindihan mo ?", "Flashback(Mother)"},
                    {"Opo!", "Flashback(???)"},
                    {"Sa tapat daw ng municipal hall....", "???"},
                    {"Malapit ka nang kumanan nang napansin mo na maraming nalabas na \nusok sa part na yun.", "???"},
                    {"Di mo ito ganong inisip at marami pa namang nagbebenta sa gabi.", "???"},
                    {"Try to make the smoke disperse using hand.", "???"},
                    {"Close your eyes.", "???"},
                    {"Ang dilim.", "???"},
                    {"Rub eyes.", "???"},
                    {"Dito ba talaga yun? Masyadong marangya parang lang kila tita.\nHindi kaya mali yung liko ko?", "???"},
                    {"Looks back", "???"},
                    {"TANGINA- nasan– may– nasaan na yung mga bahay dito?", "???"},
                    {"Step back", "???"},
                    {"Fuck", "???"},
                    {"Look Back.\nPan on the city of Biringan", "???"}
                };
                dialogueBox.startDialogue(dialogues);
            }

            bool showDialogue = true;
            bool fadeTriggered = false;
            bool fadeCompleted = false;
            sf::Clock clock;

            while (window.isOpen()) {
                sf::Event event;
                while (window.pollEvent(event)) {
                    if (event.type == sf::Event::Closed)
                        window.close();
                    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                        state = TitleScreen::GameState::TITLE;
                        break;
                    }
                    dialogueBox.handleInput(event);
                }

                float deltaTime = clock.restart().asSeconds();
                bgManager.update(deltaTime);
                dialogueBox.update();

                window.clear();
                bgManager.draw();

                if (showDialogue) {
                    dialogueBox.draw();

                    std::string currentDialogue = dialogueBox.getCurrentDialogue();

                    if (currentDialogue == "Ang dilim." && !fadeTriggered) {
                        bgManager.startFadeOut(5.0f);
                        fadeTriggered = true;
                    }

                    if (bgManager.isHidden() && !fadeCompleted) {
                        bgManager.setBackground("Birig.png");
                        bgManager.resetFade();
                        fadeCompleted = true;
                    }

                    if (currentDialogue == "Looks back") {
                        soundManager.playSound("Shock");
                    }

                    if (!dialogueBox.isVisible()) {
                        showDialogue = false;
                    }

                    // Save progress
                    GameProgress currentProgress;
                    currentProgress.backgroundImage = bgManager.getCurrentBackground();
                    currentProgress.speaker = dialogueBox.getCurrentSpeaker();
                    currentProgress.dialogue = dialogueBox.getCurrentDialogue();
                    SaveManager::saveProgress("savegame.dat", currentProgress);
                }
                else {
                    sf::Text gameplayText;
                    gameplayText.setFont(bodyFont);
                    gameplayText.setString("Gameplay is running...\nPress ESC to return.");
                    gameplayText.setCharacterSize(30);
                    gameplayText.setFillColor(sf::Color::White);
                    gameplayText.setPosition(50, 200);
                    window.draw(gameplayText);
                }

                window.display();

                if (state == TitleScreen::GameState::TITLE)
                    break;
            }

            soundManager.stopMusic();
        }
        else if (state == TitleScreen::GameState::QUIT) {
            std::cout << "Quitting the game...\n";
            window.close();
        }
    }

    return 0;
}