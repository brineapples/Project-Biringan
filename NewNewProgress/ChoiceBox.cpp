#include "ChoiceBox.h"


ChoiceBox::ChoiceBox(sf::RenderWindow& win, sf::Font& fnt)
    : window(win), font(fnt), active(false) {
}

void ChoiceBox::startChoices(const std::vector<Choice>& choices) {
    currentChoices = choices;
    boxes.clear();
    texts.clear();
    active = true;
    layoutChoices();
}

void ChoiceBox::layoutChoices() {
    float boxWidth = 700;
    float boxHeight = 60;
    float x = (window.getSize().x - boxWidth) / 2.0f;
    float yStart = window.getSize().y / 2.0f - (currentChoices.size() * (boxHeight + 15)) / 2.0f;

    for (std::size_t i = 0; i < currentChoices.size(); ++i) {
        sf::RectangleShape box(sf::Vector2f(boxWidth, boxHeight));
        box.setFillColor(sf::Color::Black);
        box.setOutlineColor(sf::Color::Green);
        box.setOutlineThickness(2);
        box.setPosition(x, yStart + i * (boxHeight + 15));
        boxes.push_back(box);

        sf::Text text;
        text.setFont(font);
        text.setString(currentChoices[i].text);
        text.setCharacterSize(22);
        text.setFillColor(sf::Color::Green);

        // Center the text inside the box
        sf::FloatRect textBounds = text.getLocalBounds();
        text.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
        text.setPosition(box.getPosition().x + boxWidth / 2.0f, box.getPosition().y + boxHeight / 2.0f);

        texts.push_back(text);
    }
}

void ChoiceBox::update() {
    if (!active) return;

    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    for (std::size_t i = 0; i < boxes.size(); ++i) {
        if (boxes[i].getGlobalBounds().contains(mousePos)) {
            boxes[i].setOutlineColor(sf::Color::Red);
            texts[i].setFillColor(sf::Color::Red);
        }
        else {
            boxes[i].setOutlineColor(sf::Color::Green);
            texts[i].setFillColor(sf::Color::Green);
        }
    }
}

void ChoiceBox::handleEvent(const sf::Event& event) {
    if (!active || event.type != sf::Event::MouseButtonPressed || event.mouseButton.button != sf::Mouse::Left) return;

    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    for (std::size_t i = 0; i < boxes.size(); ++i) {
        if (boxes[i].getGlobalBounds().contains(mousePos)) {
            currentChoices[i].action();  // Call the selected action
            active = false;              // Hide the choice box
            break;
        }
    }
}

void ChoiceBox::draw(sf::RenderWindow& targetWindow) {
    if (!active) return;
    for (const auto& box : boxes) targetWindow.draw(box);
    for (const auto& text : texts) targetWindow.draw(text);
}

bool ChoiceBox::isVisible() const {
    return active;
}