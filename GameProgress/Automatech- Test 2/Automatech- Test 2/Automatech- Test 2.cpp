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

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Escape from Biringan");

    // Load title font (used for titles, headings)
    sf::Font titleFont;
    if (!titleFont.loadFromFile("BlackDahlia.ttf")) {
        std::cerr << "Failed to load BlackDahlia.ttf\n";
        return -1;
    }

    // Load body font (used for body text, dialogue)
    sf::Font bodyFont;
    if (!bodyFont.loadFromFile("Railway.ttf")) {
        std::cerr << "Failed to load Raleway.ttf\n";
        return -1;
    }

    // Load music
    sf::Music titleMusic;
    if (!titleMusic.openFromFile("BeginMusic.ogg")) {
        std::cerr << "Failed to load music.\n";
        return -1;
    }

    // Start in the TITLE state
    TitleScreen::GameState state = TitleScreen::GameState::TITLE;

    // Main game loop
    while (window.isOpen()) {
        // TITLE SCREEN
        if (state == TitleScreen::GameState::TITLE) {
            TitleScreen titleScreen(window, titleFont, bodyFont, titleMusic);
            state = titleScreen.run();
        }
        // ABOUT SCREEN
        else if (state == TitleScreen::GameState::ABOUT) {
            AboutScreen aboutScreen(window, titleFont, bodyFont);
            AboutScreen::AboutState aboutState = aboutScreen.run();
            if (aboutState == AboutScreen::AboutState::BACK) {
                state = TitleScreen::GameState::TITLE;
            }
        }
        // GAMEPLAY STATE
        else if (state == TitleScreen::GameState::GAMEPLAY) {
            PlayIntroScreen intro(window, titleFont, bodyFont);
            PlayIntroScreen::IntroState introResult = intro.run();

            if (introResult == PlayIntroScreen::IntroState::QUIT) {
                state = TitleScreen::GameState::TITLE;
                continue;
            }

            std::cout << "Starting gameplay...\n";

            // Create dialogue box and attach to window
            DialogueBox dialogueBox(window, bodyFont);

            // Set background image for the current scene
            dialogueBox.setBackground("bgs.png");  

            // Dialogue script: list of entries with speaker and text
            std::vector<DialogueEntry> dialogues = {
                {"Dito ba talaga ang daan?", "???"},
                {"Dapat talaga nagpasama ko. Masyadong madilim ang kalsada.", "???"},
                {"Kaliwa ba o Kanan yung pupuntahan?", "???"},
                {"“Apo, puntahan mo nga muna bahay nila Auntie Amelita mo. Bigyan mo sya nang \nnatirang maja.”", "Flashback(Mother)"},
                {"Saan bahay ni Auntie?", "???"},
                {"Tapat bahay nya sa municipal hall. Pangatlong bahay na may third floor.\nNaintindihan mo ? ", "Flashback(Mother)"},
                {"Opo!", "Flashback(???)"},
                {"Sa tapat daw ng municipal hall....", "???"},
                {"Malapit ka nang kumanan nang napansin mo na maraming nalabas na \nusok sa part na yun.", "???"},
                {"Di mo ito ganong inisip at marami pa namang nagbebenta sa gabi.", "???"}
            };

            // Begin dialogue sequence
            dialogueBox.startDialogue(dialogues);
            dialogueBox.loadGameState();


            // Flag to show/hide dialogue box
            bool showDialogue = true;

            // Gameplay loop for this section
            while (window.isOpen()) {
                sf::Event event;
                while (window.pollEvent(event)) {
                    if (event.type == sf::Event::Closed)
                        window.close();

                    // Return to title with Escape
                    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                        state = TitleScreen::GameState::TITLE;
                        break;
                    }

                    // Handle dialogue click/spacebar input
                    dialogueBox.handleInput(event);
                }

                window.clear();

                if (showDialogue) {
                    // Draw background image first (behind UI elements)
                    dialogueBox.drawBackground();

                    // Animate typewriter text
                    dialogueBox.update();

                    // Draw dialogue UI (text box + speaker label)
                    dialogueBox.draw();

                    // If dialogue finishes, switch to gameplay mode
                    if (!dialogueBox.isVisible()) {
                        showDialogue = false;
                    }
                }
                else {
                    // Basic fallback/gameplay screen after dialogue
                    sf::Text gameplayText;
                    gameplayText.setFont(bodyFont);
                    gameplayText.setString("Gameplay is running...\nPress ESC to return.");
                    gameplayText.setCharacterSize(30);
                    gameplayText.setFillColor(sf::Color::White);
                    gameplayText.setPosition(50, 200);
                    window.draw(gameplayText);
                }

                window.display();

                // If returned to title, break this inner loop
                if (state == TitleScreen::GameState::TITLE) break;
            }
        }

        // QUIT STATE
        else if (state == TitleScreen::GameState::QUIT) {
            std::cout << "Quitting the game...\n";
            window.close();
        }
    }

    return 0;
}
