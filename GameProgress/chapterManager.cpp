#include "chapterManager.h"

void ChapterManager::transitionToChapter(sf::RenderWindow& window, sf::Font& bodyFont, const std::string& titleText) {
    sf::Text text;
    text.setFont(bodyFont);
    text.setString(titleText);
    text.setCharacterSize(70);
    text.setFillColor(sf::Color::White);
    text.setStyle(sf::Text::Bold);

    // Center the text using origin
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    text.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f);

    // Instruction text
    /*sf::Text continueText;
    continueText.setFont(bodyFont);
    //continueText.setString("Click or press any key to continue...");
    continueText.setCharacterSize(30);
    continueText.setFillColor(sf::Color(200, 200, 200));
    sf::FloatRect instrRect = continueText.getLocalBounds();
    continueText.setOrigin(instrRect.left + instrRect.width / 2.0f, instrRect.top + instrRect.height / 2.0f);
    continueText.setPosition(window.getSize().x / 2.0f, window.getSize().y * 0.75f);*/

    bool waiting = true;
    while (waiting && window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return;
            }
            if (event.type == sf::Event::KeyPressed || event.type == sf::Event::MouseButtonPressed) {
                waiting = false;
            }
        }

        window.clear(sf::Color::Black);
        window.draw(text);
        //window.draw(continueText);
        window.display();
    }
}