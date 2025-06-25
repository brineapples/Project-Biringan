#include "PlayIntro.h"

PlayIntroScreen::PlayIntroScreen(sf::RenderWindow& win, sf::Font& tFont, sf::Font& bFont)
    : window(win), titleFont(tFont), bodyFont(bFont), messageIndex(0)
{
    messages = {
        "Notice: This game is still under development.\nYou may experience bugs or incomplete features.",
        "Disclaimer: This game is for players 18 and older.\nIt may contain gore and disturbing imagery.",
        "Chapter 1: Biringan"
    };

    // Configure SFML text object to display the first message
    message.setFont(bodyFont);  // Start with body font
    message.setCharacterSize(24);
    message.setFillColor(sf::Color::White);
    message.setString(messages[messageIndex]);

    // Center the text in the window
    sf::FloatRect bounds = message.getLocalBounds();
    message.setOrigin(bounds.width / 2, bounds.height / 2);
    message.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f);
}

// Main loop to run the intro screen
PlayIntroScreen::IntroState PlayIntroScreen::run() {
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            // If the window is closed, exit to TITLE screen
            if (event.type == sf::Event::Closed)
                return IntroState::QUIT;

            // If left mouse button is clicked, advance message
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                messageIndex++;

                // If all messages have been shown, exit to gameplay
                if (messageIndex >= messages.size())
                    return IntroState::DONE;

                // If it's the final message (chapter title), style it differently
                if (messageIndex == messages.size() - 1) {
                    message.setFont(bodyFont);
                    message.setCharacterSize(48);
                }
                else {
                    message.setFont(bodyFont);
                    message.setCharacterSize(24);
                }
                // Update the message content
                message.setString(messages[messageIndex]);
                // Recenter the message after changing the text
                sf::FloatRect bounds = message.getLocalBounds();
                message.setOrigin(bounds.width / 2, bounds.height / 2);
                message.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f);
            }
        }

        window.clear();
        window.draw(message);
        window.display();
    }

    return IntroState::QUIT;
}