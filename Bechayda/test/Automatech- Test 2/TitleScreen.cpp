#include "TitleScreen.h"
#include <cmath>

// Constructor to initialize assets and layout
TitleScreen::TitleScreen(sf::RenderWindow& win, sf::Font& titleFont, sf::Font& bodyFont, SoundManager& soundManager)
    : window(win), titleFont(titleFont), bodyFont(bodyFont), soundManager(soundManager)
{
    backgroundTexture.loadFromFile("simba.png");
    background.setTexture(backgroundTexture);

    float scaleX = (float)window.getSize().x / backgroundTexture.getSize().x;
    float scaleY = (float)window.getSize().y / backgroundTexture.getSize().y;
    background.setScale(scaleX, scaleY);

    titleLine1.setFont(titleFont);
    titleLine1.setString("Escape from");
    titleLine1.setCharacterSize(48);
    titleLine1.setFillColor(sf::Color::Red);
    titleLine1.setPosition(50, 30);

    titleLine2.setFont(titleFont);
    titleLine2.setString("Biringan City");
    titleLine2.setCharacterSize(60);
    titleLine2.setFillColor(sf::Color::Red);
    titleLine2.setPosition(30, 100);

    playButton.setFont(bodyFont);
    aboutButton.setFont(bodyFont);
    quitButton.setFont(bodyFont);
    loadButton.setFont(bodyFont);

    playButton.setString("Play");
    aboutButton.setString("About");
    loadButton.setString("Load");
    quitButton.setString("Quit");

    playButton.setCharacterSize(34);
    aboutButton.setCharacterSize(34);
    loadButton.setCharacterSize(34);
    quitButton.setCharacterSize(34);

    playButton.setPosition(50, 250);
    aboutButton.setPosition(50, 320);
    loadButton.setPosition(50, 390);  // Adjust position to include Load button
    quitButton.setPosition(50, 460);
}

TitleScreen::GameState TitleScreen::run() {
    sf::Clock clock;
    soundManager.playMusic("BeginMusic.ogg", true);  // Background music

    while (window.isOpen()) {
        GameState state = handleEvents();
        if (state != GameState::TITLE) return state;

        float time = clock.getElapsedTime().asSeconds();
        animateTitles(time);

        draw();
    }
    return GameState::QUIT;
}

void TitleScreen::handleMouseHover(const sf::Vector2f& mousePos) {
    playButton.setFillColor(playButton.getGlobalBounds().contains(mousePos) ? sf::Color::Red : sf::Color::Black);
    aboutButton.setFillColor(aboutButton.getGlobalBounds().contains(mousePos) ? sf::Color::Red : sf::Color::Black);
    loadButton.setFillColor(loadButton.getGlobalBounds().contains(mousePos) ? sf::Color::Red : sf::Color::Black);
    quitButton.setFillColor(quitButton.getGlobalBounds().contains(mousePos) ? sf::Color::Red : sf::Color::Black);
}

TitleScreen::GameState TitleScreen::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

            if (playButton.getGlobalBounds().contains(mousePos)) {
                soundManager.stopMusic();
                return GameState::GAMEPLAY;
            }
            if (aboutButton.getGlobalBounds().contains(mousePos))
                return GameState::ABOUT;
            if (loadButton.getGlobalBounds().contains(mousePos))
                return GameState::LOAD;  // Fully functional Load button
            if (quitButton.getGlobalBounds().contains(mousePos)) {
                window.close();
                return GameState::QUIT;
            }
        }
    }

    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    handleMouseHover(mousePos);
    return GameState::TITLE;
}

void TitleScreen::animateTitles(float time) {
    float offsetY1 = std::sin(time * 3.0f) * 2.0f;
    float scale1 = 1.0f + std::sin(time * 2.0f) * 0.01f;
    float offsetY2 = std::sin(time * 2.5f + 1.0f) * 5.0f;
    float scale2 = 1.0f + std::sin(time * 3.5f + 0.5f) * 0.02f;

    titleLine1.setPosition(50, 30 + offsetY1);
    titleLine2.setPosition(30, 100 + offsetY2);
    titleLine1.setScale(scale1, scale1);
    titleLine2.setScale(scale2, scale2);
}

void TitleScreen::draw() {
    window.clear();
    window.draw(background);
    window.draw(titleLine1);
    window.draw(titleLine2);
    window.draw(playButton);
    window.draw(aboutButton);
    window.draw(loadButton);  // Draw Load button
    window.draw(quitButton);
    window.display();
}