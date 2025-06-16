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
#include "chapterManager.h"
#include "CharacterPortrait.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(1600, 900), "Escape from Biringan");

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
    if (!soundManager.loadSound("ECH1", "ECH1.ogg")) {
        std::cerr << "Failed to load ECH sound effect.\n";
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

            BackgroundManager bgManager(window);
            DialogueBox dialogueBox(window, bodyFont);
            CharacterPortrait middlePortrait;

            if (!middlePortrait.load("mac.png")) {
                std::cerr << "Failed to load mysterious_character.png\n";
            }
            middlePortrait.setScale(0.6f, 0.6f);
            middlePortrait.setVisible(false);

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
            bool chapter2Started = false;
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

                window.clear(sf::Color::Black);
                bgManager.draw();

                std::string currentDialogue = dialogueBox.getCurrentDialogue();

                if (showDialogue) {
                    if (currentDialogue == "You have entered the City of Biringan.") {
                        middlePortrait.setVisible(true);
                    }
                    else {
                        middlePortrait.setVisible(false);
                    }

                    // Center character portrait
                    if (middlePortrait.isVisible()) {
                        float centerX = window.getSize().x / 2.0f - middlePortrait.getGlobalBounds().width / 2.0f;
                        float posY = window.getSize().y * 0.2f;
                        middlePortrait.setPosition(centerX, posY);
                        middlePortrait.draw(window);
                    }

                    dialogueBox.draw();

                    if (currentDialogue == "Ang dilim." && !fadeTriggered) {
                        bgManager.setBackground("BlackBG.jpg");  
                        fadeTriggered = true;
                    }

                    if (currentDialogue == "Rub eyes." && !fadeCompleted) {
                        bgManager.setBackground("Birig.png");  
                        fadeCompleted = true;
                    }

                    if (currentDialogue == "Looks back") {
                        soundManager.playSound("ECH1");
                    }

                    if (currentDialogue == "Look Back.\nPan on the city of Biringan" && !chapter2Started) {
                        chapter2Started = true;

                        ChapterManager::transitionToChapter(window, bodyFont, "Chapter 2: Biringan");
                        ChapterManager::transitionToChapter(window, bodyFont, "TIME: 9:00 PM");
                        bgManager.setBackground("House.png");

                        std::vector<DialogueEntry> chapter2Dialogues = {
                            {"Mga naglalakihang mga gusali, maaayos ang mga daan at kay rami ang mga halaman\nang nakikita ko. Ibang-iba ito sa lugar na dapat na nakatayo duon. Parang isang\nbustling metropolitan kung sasabihin ng iba. Nararamdaman ko ang taas ng mga\nbuhok ko sa balahibo habang segu-segundo akong nakatayo sa gitna ng kalsadang. ", "???"},
                            {"nito. Tuloy tuloy ang galaw ng mga tao sa iba't ibang direksyon na parang normal lang\n ang nangyayari.Na parang walang kakaiba ang napapagmasdan ko. Nabaling\n ang tingin ko sa gitna ng daan. Marami ang nagtitipon at pinapaganda ang daanan.", "???"},
                            {"Naglalagay ng mga ilaw, bandana, balloons at mga kung ano-ano sa mga streetlights\nna katulad duon sa Jones Bridge. May mga tao na may dala-dalang mga basket na\npuno ng pagkain at mga halaman. Halatang may pagdiriwang ang nangyayari.\nMaraming naka-suot ng pormal at may mga iba na parang buwan ng wika ang atake " ,"???"},
                            {"Rinig na rinig ang mga paguusap, ang tawa ng paglalaro ng mga bata at ang mga\nkanta at ang kasiyahan.Pinagmamasdan ko sila, Mahahaba ang kanilang buhok\nat iba iba ang kanilang kulay, mga puti o kayumanggi ang balat, matatangkad\nat ang ga-ganda at gwapo na mapapabaling tingin ka ulit at iisipin kung ibang." , "???"},
                            {"mundo ba sila", "???"},
                            {"Ibang mundo?", "???"},
                            {"Magaganda", "???"},
                            {"Para akong nabuhusan ng tubig nang maintindihan ko ang lahat ng nangyayari.","???"},
                            {"Akala ko sabi-sabi lang ito. Mito kuno ng mga matatanda pang bigay takot.", "???"},
                            {"Napahakbang ako pabalik ng marinig ko ang tunog ng kampana. Nagsitahimikan\nang mga tao.", "???"},
                            //SOUND EFFECTS HERE  DUM DUM DUM DUM
                            {"Kada tunog ng kampana ay padagdag ng padagdag ang takot sa puso ko. Pinwersa\nko ang sarili ko na tignan ang nangyayari sa likod. Nakita kong binubuksan ang\npinto ng isang lumang gusali. Namangha ako sa Nakita kong display. Marangya lang\nang mad-describe ko sa nakita ko. Gintong mga karwahe, mga mamahaling kotse,", "???"},
                            {"mga karong ginto na may puting lace na punong puno ng pagkain, gamit at kayamanan.\nMayroon rin silang mga bantay nakasuot ng mga lumang kasuotan at--", "???"}
                          
                        };
                        
                        dialogueBox.startDialogue(chapter2Dialogues);
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
