#include "DialogueBox.h"
#include <iostream>

DialogueBox::DialogueBox(sf::RenderWindow& win, sf::Font& fnt)
    : window(win), font(fnt), currentCharIndex(0), visible(false),
    finishedTyping(false), typeSpeed(50.0f), hasBackgroundImage(false)
{
    // Get window size
    sf::Vector2u windowSize = window.getSize();

    // slightly larger
    float boxWidth = 900.0f;
    float boxHeight = 150.0f;
    background.setSize(sf::Vector2f(boxWidth, boxHeight));
    background.setFillColor(sf::Color(0, 0, 0, 180));
    background.setOutlineColor(sf::Color::White);
    background.setOutlineThickness(2);

    // Position at bottom center
    float boxX = (windowSize.x - boxWidth) / 2.0f;
    float boxY = windowSize.y - boxHeight - 30.0f; // 30px from bottom
    background.setPosition(boxX, boxY);

    // Text inside box, with margin
    textDisplay.setFont(font);
    textDisplay.setCharacterSize(22);
    textDisplay.setFillColor(sf::Color::White);
    textDisplay.setPosition(boxX + 20.0f, boxY + 20.0f);

    // Speaker box above main dialogue box
    float speakerBoxWidth = 220.0f;
    float speakerBoxHeight = 45.0f;
    speakerBackground.setSize(sf::Vector2f(speakerBoxWidth, speakerBoxHeight));
    speakerBackground.setFillColor(sf::Color(0, 0, 0, 200));
    speakerBackground.setOutlineColor(sf::Color::White);
    speakerBackground.setOutlineThickness(2);
    speakerBackground.setPosition(boxX, boxY - speakerBoxHeight - 10.0f);

    // Speaker name text
    speakerText.setFont(font);
    speakerText.setCharacterSize(20);
    speakerText.setFillColor(sf::Color::White);
    speakerText.setPosition(speakerBackground.getPosition().x + 10.0f, speakerBackground.getPosition().y + 8.0f);

    // Right-side panel for options just shape
    float panelWidth = 80.0f;
    float panelHeight = boxHeight;
    rightPanel.setSize(sf::Vector2f(panelWidth, panelHeight));
    rightPanel.setFillColor(sf::Color(0, 0, 0, 150));
    rightPanel.setOutlineColor(sf::Color::White);
    rightPanel.setOutlineThickness(2);
    rightPanel.setPosition(boxX + boxWidth + 10.0f, boxY); // 10px to the right of the dialogue box
}

void DialogueBox::startDialogue(const std::vector<DialogueEntry>& entries) {
    dialogues = std::queue<DialogueEntry>();
    for (const auto& entry : entries)
        dialogues.push(entry);
    nextDialogue();
    visible = true;
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
    finishedTyping = false;
}

void DialogueBox::update() {
    if (!visible || finishedTyping)
        return;

    float elapsed = typeClock.getElapsedTime().asSeconds();
    std::size_t targetCharIndex = static_cast<std::size_t>(elapsed * typeSpeed);

    if (targetCharIndex > currentCharIndex) {
        currentCharIndex = std::min(targetCharIndex, fullText.size());
        textDisplay.setString(fullText.substr(0, currentCharIndex));

        if (currentCharIndex >= fullText.size()) {
            finishedTyping = true;
        }
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

    // Draw right-side panel
    window.draw(rightPanel);
}

void DialogueBox::handleInput(const sf::Event& event) {
    if (!visible) return;

    if (event.type == sf::Event::MouseButtonPressed ||
        (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)) {
        if (!finishedTyping) {
            textDisplay.setString(fullText);
            finishedTyping = true;
            currentCharIndex = fullText.size();
        }
        else {
            nextDialogue();
        }
    }
}

bool DialogueBox::isVisible() const {
    return visible;
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

std::string DialogueBox::getCurrentDialogue() const {
    return fullText;
}

std::string DialogueBox::getCurrentSpeaker() const {
    return speakerName;
}