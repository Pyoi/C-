#pragma once

#include "Board.hpp"
#include "Piece.hpp"
#include "ResourceManager.hpp"
#include "Types.hpp"
#include <SFML/Graphics.hpp>
#include <random>
#include <string>
#include <vector>

class Game {
public:
    Game();
    void run();

private:
    void processEvents();
    void update(float dt);
    void render();

    void updateTitle();
    void updatePlaying(float dt);

    void drawTitle();
    void drawGame();
    void drawBoard();
    void drawPiece(const Piece& piece, bool preview = false);
    void drawText(const std::string& text, unsigned int size, float x, float y,
                  const sf::Color& color = sf::Color::White);

    void startGame(Difficulty difficulty);
    void spawnNextPiece();
    Piece makeRandomPiece() const;
    bool canPlace(const Piece& piece, int newX, int newY) const;
    bool tryMove(int dx, int dy);
    void tryRotate();
    void lockPiece();

    int colorCountForDifficulty() const;
    float rainbowChanceForDifficulty() const;

    sf::RenderWindow window_;
    ResourceManager resources_;
    Board board_;

    SceneState scene_{SceneState::Title};
    Difficulty difficulty_{Difficulty::Normal};

    std::vector<std::string> menuItems_{"簡単", "普通", "難しい", "ゲーム終了"};
    int menuIndex_{0};

    Piece currentPiece_;
    Piece nextPiece_;
    bool hasCurrentPiece_{false};

    int score_{0};
    float fallTimer_{0.0f};
    float lockTimer_{0.0f};
    bool touchingGround_{false};
    bool downHeld_{false};

    mutable std::mt19937 rng_;
};
