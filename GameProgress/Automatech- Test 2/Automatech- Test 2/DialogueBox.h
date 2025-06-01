#ifndef DIALOGUEBOX_HPP
#define DIALOGUEBOX_HPP

#include <SFML/Graphics.hpp>
#include <string>
#include <queue>
#include <vector>

// Struct for holding speaker + dialogue
struct DialogueEntry {
    std::string text;
    std::string speaker;

    DialogueEntry(const std::string& t, const std::string& s) : text(t), speaker(s) {}
};

class DialogueBox {
public:
    DialogueBox(sf::RenderWindow& window, sf::Font& font);

    void startDialogue(const std::vector<DialogueEntry>& entries);
    void update();
    void draw();
    void handleInput(const sf::Event& event);
    bool isVisible() const;

    // Background image functions
    void setBackground(const std::string& imagePath);
    void drawBackground();

    void saveGameState(); //LOOK HERE
    void loadGameState(); // LOOK HERE

    void exitAndResetGame(); // Reset Load State

private:
    void nextDialogue();

    std::vector<DialogueEntry> allDialogues; //LOOK HERE
    size_t currentDialogueIndex = 0;         //LOOK HERE

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

    // Background image variables
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    bool hasBackgroundImage = false;
};

#endif // DIALOGUEBOX_HPP
