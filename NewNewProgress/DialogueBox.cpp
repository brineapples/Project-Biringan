#include "DialogueBox.h"
#include <iostream>

DialogueBox::DialogueBox(sf::RenderWindow& win, sf::Font& fnt)
    : window(win), font(fnt), currentCharIndex(0), visible(false),
    finishedTyping(false), typeSpeed(50.0f), hasBackgroundImage(false),
    autoForwardEnabled(false), backPressed(false)
{
    sf::Vector2u windowSize = window.getSize();

    float boxWidth = 900.0f;
    float boxHeight = 150.0f;
    background.setSize(sf::Vector2f(boxWidth, boxHeight));
    background.setFillColor(sf::Color(0, 0, 0, 180));
    background.setOutlineColor(sf::Color::White);
    background.setOutlineThickness(2);

    float boxX = (windowSize.x - boxWidth) / 2.0f;
    float boxY = windowSize.y - boxHeight - 30.0f;
    background.setPosition(boxX, boxY);

    textDisplay.setFont(font);
    textDisplay.setCharacterSize(22);
    textDisplay.setFillColor(sf::Color::White);
    textDisplay.setPosition(boxX + 20.0f, boxY + 20.0f);

    // Speaker box
    float speakerBoxWidth = 220.0f;
    float speakerBoxHeight = 45.0f;
    speakerBackground.setSize(sf::Vector2f(speakerBoxWidth, speakerBoxHeight));
    speakerBackground.setFillColor(sf::Color(0, 0, 0, 200));
    speakerBackground.setOutlineColor(sf::Color::White);
    speakerBackground.setOutlineThickness(2);
    speakerBackground.setPosition(boxX, boxY - speakerBoxHeight - 10.0f);

    speakerText.setFont(font);
    speakerText.setCharacterSize(20);
    speakerText.setFillColor(sf::Color::White);
    speakerText.setPosition(speakerBackground.getPosition().x + 10.0f, speakerBackground.getPosition().y + 8.0f);

    // Right panel
    float panelWidth = 80.0f;
    float panelHeight = boxHeight;
    rightPanel.setSize(sf::Vector2f(panelWidth, panelHeight));
    rightPanel.setFillColor(sf::Color(0, 0, 0, 150));
    rightPanel.setOutlineColor(sf::Color::White);
    rightPanel.setOutlineThickness(2);
    rightPanel.setPosition(boxX + boxWidth + 10.0f, boxY);

    // Forward button
    forwardButton.setSize(sf::Vector2f(60.0f, 30.0f));
    forwardButton.setOutlineColor(sf::Color::White);
    forwardButton.setOutlineThickness(1);
    forwardButton.setPosition(rightPanel.getPosition().x + 10.0f, rightPanel.getPosition().y + 10.0f);

    forwardText.setFont(font);
    forwardText.setCharacterSize(14);
    forwardText.setString("Auto");
    forwardText.setFillColor(sf::Color::White);
    forwardText.setPosition(forwardButton.getPosition().x + 10.0f, forwardButton.getPosition().y + 5.0f);

    // Back button
    backButton.setSize(sf::Vector2f(60.0f, 30.0f));
    backButton.setOutlineColor(sf::Color::White);
    backButton.setOutlineThickness(1);
    backButton.setPosition(rightPanel.getPosition().x + 10.0f, forwardButton.getPosition().y + 40.0f);

    backText.setFont(font);
    backText.setCharacterSize(14);
    backText.setString("Back");
    backText.setFillColor(sf::Color::White);
    sf::FloatRect backBounds = backText.getLocalBounds();
    backText.setOrigin(backBounds.left + backBounds.width / 2.0f, backBounds.top + backBounds.height / 2.0f);
    backText.setPosition(
        backButton.getPosition().x + backButton.getSize().x / 2.0f,
        backButton.getPosition().y + backButton.getSize().y / 2.0f
    );
}

void DialogueBox::startDialogue(const std::vector<DialogueEntry>& entries) {
    dialogues = std::queue<DialogueEntry>();
    for (const auto& entry : entries)
        dialogues.push(entry);
    nextDialogue();
    visible = true;
    backPressed = false;
}

void DialogueBox::nextDialogue() {
    if (dialogues.empty()) {
        visible = false;
        return;
    }

    DialogueEntry entry = dialogues.front();
    dialogues.pop();
    fullText = entry.text;
    speakerName = entry.speaker;
    currentCharIndex = 0;
    textDisplay.setString("");
    speakerText.setString(speakerName);
    typeClock.restart();
    advanceClock.restart();
    finishedTyping = false;
}

void DialogueBox::update() {
    if (!visible) return;

    float elapsed = typeClock.getElapsedTime().asSeconds();
    std::size_t targetCharIndex = static_cast<std::size_t>(elapsed * typeSpeed);

    if (!finishedTyping && targetCharIndex > currentCharIndex) {
        currentCharIndex = std::min(targetCharIndex, fullText.size());
        textDisplay.setString(fullText.substr(0, currentCharIndex));

        if (currentCharIndex >= fullText.size()) {
            finishedTyping = true;
            advanceClock.restart();  // Delay before auto-forward
        }
    }

    if (autoForwardEnabled && finishedTyping && advanceClock.getElapsedTime().asSeconds() >= 1.0f) {
        nextDialogue();
    }

    // Hover highlight + persistent state
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    // Forward button
    if (forwardButton.getGlobalBounds().contains(mousePos)) {
        forwardButton.setFillColor(sf::Color(130, 130, 255)); // Hover blue
    }
    else {
        forwardButton.setFillColor(autoForwardEnabled ? sf::Color(100, 100, 255) : sf::Color(50, 50, 50));
    }

    // Back button
    if (backButton.getGlobalBounds().contains(mousePos)) {
        backButton.setFillColor(sf::Color(255, 130, 130)); // Hover red
    }
    else {
        backButton.setFillColor(backPressed ? sf::Color(200, 50, 50) : sf::Color(50, 50, 50));
    }
}

void DialogueBox::draw() {
    if (!visible) return;

    window.draw(background);
    window.draw(textDisplay);

    if (!speakerName.empty()) {
        window.draw(speakerBackground);
        window.draw(speakerText);
    }

    window.draw(rightPanel);
    window.draw(forwardButton);
    window.draw(forwardText);
    window.draw(backButton);
    window.draw(backText);
}

void DialogueBox::handleInput(const sf::Event& event) {
    if (!visible) return;

    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        if (forwardButton.getGlobalBounds().contains(mousePos)) {
            autoForwardEnabled = !autoForwardEnabled;
            return;
        }

        if (backButton.getGlobalBounds().contains(mousePos)) {
            backPressed = true;
            visible = false;
            return;
        }

        if (!finishedTyping) {
            textDisplay.setString(fullText);
            currentCharIndex = fullText.size();
            finishedTyping = true;
        }
        else {
            nextDialogue();
        }
    }

    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
        if (!finishedTyping) {
            textDisplay.setString(fullText);
            currentCharIndex = fullText.size();
            finishedTyping = true;
        }
        else {
            nextDialogue();
        }
    }
}

void DialogueBox::setBackground(const std::string& imagePath) {
    if (backgroundTexture.loadFromFile(imagePath)) {
        backgroundSprite.setTexture(backgroundTexture);
        float scaleX = static_cast<float>(window.getSize().x) / backgroundTexture.getSize().x;
        float scaleY = static_cast<float>(window.getSize().y) / backgroundTexture.getSize().y;
        backgroundSprite.setScale(scaleX, scaleY);
        hasBackgroundImage = true;
    }
    else {
        std::cerr << "Failed to load image: " << imagePath << std::endl;
        hasBackgroundImage = false;
    }
}

void DialogueBox::drawBackground() {
    if (hasBackgroundImage) {
        window.draw(backgroundSprite);
    }
}

bool DialogueBox::isVisible() const {
    return visible;
}

bool DialogueBox::wasBackPressed() const {
    return backPressed;
}

void DialogueBox::resetBackPressed() {
    backPressed = false;
}

std::string DialogueBox::getCurrentDialogue() const {
    return fullText;
}

std::string DialogueBox::getCurrentSpeaker() const {
    return speakerName;
}