#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <functional>
#include <string>

struct Choice {
    std::string text;
    std::function<void()> action;
};

class ChoiceBox {
public:
    ChoiceBox(sf::RenderWindow& win, sf::Font& fnt);

    void startChoices(const std::vector<Choice>& choices);
    void handleEvent(const sf::Event& event);
    void update();  // <--- NEW: handles hover
    void draw(sf::RenderWindow& targetWindow);
    bool isVisible() const;

private:
    sf::RenderWindow& window;
    sf::Font& font;
    bool active;

    std::vector<Choice> currentChoices;
    std::vector<sf::RectangleShape> boxes;
    std::vector<sf::Text> texts;

    void layoutChoices();
};