#ifndef DIALOGUEBOX_HPP
#define DIALOGUEBOX_HPP

#include <SFML/Graphics.hpp>
#include <string>
#include <queue>

// Struct for holding speaker + dialogue
struct DialogueEntry {
    std::string text;
    std::string speaker;

    DialogueEntry(const std::string& t, const std::string& s) : text(t), speaker(s) {}
};

class DialogueBox {
public:
    DialogueBox(sf::RenderWindow& window, sf::Font& font);

    // Start a sequence of dialogue lines
    void startDialogue(const std::vector<DialogueEntry>& entries);

    // Update the typewriter effect
    void update();

    // Render the dialogue box and speaker name
    void draw();

    // Input handler for advancing or skipping
    void handleInput(const sf::Event& event);

    // Is the dialogue box visible?
    bool isVisible() const;

private:
    void nextDialogue();

    sf::RenderWindow& window;
    sf::Font& font;

    sf::RectangleShape background;
    sf::Text textDisplay;

    sf::RectangleShape speakerBackground;
    sf::Text speakerText;

    std::string fullText;
    std::string speakerName;
    std::size_t currentCharIndex;
    sf::Clock typeClock;

    bool visible;
    bool finishedTyping;
    float typeSpeed;

    std::queue<DialogueEntry> dialogues;
};

#endif // DIALOGUEBOX_HPP