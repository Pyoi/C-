#include "Game.hpp"
#include <algorithm>
#include <iostream>

namespace {
sf::Color fallbackColor(BlockType type) {
    switch (type) {
        case BlockType::Red: return sf::Color(255, 90, 120);
        case BlockType::Blue: return sf::Color(90, 160, 255);
        case BlockType::Green: return sf::Color(90, 220, 120);
        case BlockType::Yellow: return sf::Color(255, 220, 90);
        case BlockType::Purple: return sf::Color(200, 120, 255);
        case BlockType::Orange: return sf::Color(255, 160, 80);
        case BlockType::Cyan: return sf::Color(90, 240, 240);
        case BlockType::Rainbow: return sf::Color(255, 255, 255);
        default: return sf::Color(120, 120, 120);
    }
}
}

Game::Game()
    : window_(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Poop Drop Game"),
      rng_(std::random_device{}()) {
    window_.setFramerateLimit(60);
    resources_.load();
}

void Game::run() {
    sf::Clock clock;
    while (window_.isOpen() && scene_ != SceneState::Exiting) {
        const float dt = clock.restart().asSeconds();
        processEvents();
        update(dt);
        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (window_.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window_.close();
            scene_ = SceneState::Exiting;
            return;
        }

        if (event.type == sf::Event::KeyPressed) {
            if (scene_ == SceneState::Title) {
                if (event.key.code == sf::Keyboard::Up) {
                    menuIndex_ = (menuIndex_ + static_cast<int>(menuItems_.size()) - 1) % static_cast<int>(menuItems_.size());
                } else if (event.key.code == sf::Keyboard::Down) {
                    menuIndex_ = (menuIndex_ + 1) % static_cast<int>(menuItems_.size());
                } else if (event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Return) {
                    if (menuIndex_ == 0) startGame(Difficulty::Easy);
                    if (menuIndex_ == 1) startGame(Difficulty::Normal);
                    if (menuIndex_ == 2) startGame(Difficulty::Hard);
                    if (menuIndex_ == 3) {
                        window_.close();
                        scene_ = SceneState::Exiting;
                    }
                }
            } else if (scene_ == SceneState::Playing && hasCurrentPiece_) {
                if (event.key.code == sf::Keyboard::Left) {
                    tryMove(-1, 0);
                } else if (event.key.code == sf::Keyboard::Right) {
                    tryMove(1, 0);
                } else if (event.key.code == sf::Keyboard::Up) {
                    tryRotate();
                }
            }
        }
    }

    downHeld_ = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
}

void Game::update(float dt) {
    if (scene_ == SceneState::Title) {
        updateTitle();
    } else if (scene_ == SceneState::Playing) {
        updatePlaying(dt);
    }
}

void Game::updateTitle() {
}

void Game::updatePlaying(float dt) {
    if (!hasCurrentPiece_) {
        currentPiece_ = nextPiece_;
        spawnNextPiece();
        hasCurrentPiece_ = true;
        if (!canPlace(currentPiece_, currentPiece_.x, currentPiece_.y)) {
            scene_ = SceneState::Title;
            return;
        }
    }

    const float normalFallInterval = 0.45f;
    const float softDropInterval = 0.05f;
    const float currentInterval = downHeld_ ? softDropInterval : normalFallInterval;
    const float lockDelay = 0.35f;

    fallTimer_ += dt;

    if (fallTimer_ >= currentInterval) {
        fallTimer_ = 0.0f;
        if (!tryMove(0, 1)) {
            touchingGround_ = true;
        }
    }

    if (touchingGround_) {
        lockTimer_ += dt;
        if (lockTimer_ >= lockDelay) {
            lockPiece();
        }
    }
}

void Game::render() {
    window_.clear(sf::Color(20, 20, 30));
    if (scene_ == SceneState::Title) {
        drawTitle();
    } else if (scene_ == SceneState::Playing) {
        drawGame();
    }
    window_.display();
}

void Game::drawTitle() {
    drawText("Poop Drop Game", 34, 120.0f, 90.0f, sf::Color::White);
    drawText("↑↓ で選択 / Enter で決定", 20, 110.0f, 150.0f, sf::Color(220, 220, 220));

    for (int i = 0; i < static_cast<int>(menuItems_.size()); ++i) {
        const bool selected = (i == menuIndex_);
        sf::Color color = selected ? sf::Color(255, 220, 120) : sf::Color::White;
        std::string label = selected ? "> " + menuItems_[i] : "  " + menuItems_[i];
        drawText(label, 28, 160.0f, 240.0f + i * 56.0f, color);
    }
}

void Game::drawGame() {
    drawBoard();
    if (hasCurrentPiece_) {
        drawPiece(currentPiece_, false);
    }

    sf::RectangleShape panel(sf::Vector2f(150, 230));
    panel.setPosition(330, 40);
    panel.setFillColor(sf::Color(32, 32, 45));
    panel.setOutlineThickness(2.0f);
    panel.setOutlineColor(sf::Color(90, 90, 120));
    window_.draw(panel);

    drawText("SCORE", 24, 360.0f, 60.0f);
    drawText(std::to_string(score_), 28, 360.0f, 95.0f, sf::Color(255, 230, 140));
    drawText("NEXT", 24, 365.0f, 160.0f);
    drawPiece(nextPiece_, true);

    drawText("←→ 移動", 18, 340.0f, 320.0f, sf::Color(210, 210, 210));
    drawText("↓  加速落下", 18, 340.0f, 350.0f, sf::Color(210, 210, 210));
    drawText("↑  回転", 18, 340.0f, 380.0f, sf::Color(210, 210, 210));
    drawText("Esc でタイトルへ", 18, 340.0f, 410.0f, sf::Color(210, 210, 210));

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
        scene_ = SceneState::Title;
    }
}

void Game::drawBoard() {
    sf::RectangleShape cellBg(sf::Vector2f(CELL_SIZE - 1.0f, CELL_SIZE - 1.0f));
    cellBg.setFillColor(sf::Color(40, 40, 56));

    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            const float px = static_cast<float>(BOARD_X + col * CELL_SIZE);
            const float py = static_cast<float>(BOARD_Y + row * CELL_SIZE);
            cellBg.setPosition(px, py);
            window_.draw(cellBg);

            const BlockType type = board_.getCell(row, col);
            if (type == BlockType::Empty) {
                continue;
            }

            if (resources_.hasTexture(type)) {
                sf::Sprite sprite(resources_.getTexture(type));
                const auto texSize = sprite.getTexture()->getSize();
                sprite.setScale(40.0f / texSize.x, 40.0f / texSize.y);
                sprite.setPosition(px + 4.0f, py + 4.0f);
                window_.draw(sprite);
            } else {
                sf::CircleShape shape(18.0f);
                shape.setPosition(px + 6.0f, py + 6.0f);
                shape.setFillColor(fallbackColor(type));
                shape.setOutlineThickness(2.0f);
                shape.setOutlineColor(sf::Color(255, 255, 255, 40));
                window_.draw(shape);
            }
        }
    }
}

void Game::drawPiece(const Piece& piece, bool preview) {
    for (const auto& cell : piece.cells) {
        float px;
        float py;

        if (preview) {
            px = 368.0f + cell.dx * 42.0f;
            py = 205.0f + cell.dy * 42.0f;
        } else {
            px = static_cast<float>(BOARD_X + (piece.x + cell.dx) * CELL_SIZE + 4);
            py = static_cast<float>(BOARD_Y + (piece.y + cell.dy) * CELL_SIZE + 4);
        }

        if (resources_.hasTexture(cell.type)) {
            sf::Sprite sprite(resources_.getTexture(cell.type));
            const auto texSize = sprite.getTexture()->getSize();
            const float size = preview ? 36.0f : 40.0f;
            sprite.setScale(size / texSize.x, size / texSize.y);
            sprite.setPosition(px, py);
            window_.draw(sprite);
        } else {
            sf::CircleShape shape(preview ? 16.0f : 18.0f);
            shape.setPosition(px + (preview ? 2.0f : 0.0f), py + (preview ? 2.0f : 0.0f));
            shape.setFillColor(fallbackColor(cell.type));
            shape.setOutlineThickness(2.0f);
            shape.setOutlineColor(sf::Color(255, 255, 255, 40));
            window_.draw(shape);
        }
    }
}

void Game::drawText(const std::string& text, unsigned int size, float x, float y, const sf::Color& color) {
    if (!resources_.hasFont()) {
        return;
    }
    sf::Text drawable;
    drawable.setFont(resources_.getFont());
    drawable.setString(text);
    drawable.setCharacterSize(size);
    drawable.setFillColor(color);
    drawable.setPosition(x, y);
    window_.draw(drawable);
}

void Game::startGame(Difficulty difficulty) {
    difficulty_ = difficulty;
    scene_ = SceneState::Playing;
    board_.clear();
    score_ = 0;
    fallTimer_ = 0.0f;
    lockTimer_ = 0.0f;
    touchingGround_ = false;
    hasCurrentPiece_ = false;
    nextPiece_ = makeRandomPiece();
}

void Game::spawnNextPiece() {
    nextPiece_ = makeRandomPiece();
}

Piece Game::makeRandomPiece() const {
    Piece piece;

    std::uniform_real_distribution<float> chanceDist(0.0f, 1.0f);
    const bool rainbow = chanceDist(rng_) < rainbowChanceForDifficulty();

    std::uniform_real_distribution<float> shapeDist(0.0f, 1.0f);
    const float roll = shapeDist(rng_);
    if (rainbow) {
        piece.shape = PieceShape::Single;
        piece.cells = {{0, 0, BlockType::Rainbow}};
    } else if (roll < 0.50f) {
        piece.shape = PieceShape::Single;
    } else if (roll < 0.75f) {
        piece.shape = PieceShape::Vertical2;
    } else {
        piece.shape = PieceShape::Horizontal2;
    }

    std::uniform_int_distribution<int> colorDist(1, colorCountForDifficulty());
    auto randomColor = [&]() {
        return static_cast<BlockType>(colorDist(rng_));
    };

    if (piece.shape == PieceShape::Single && piece.cells.empty()) {
        piece.cells = {{0, 0, randomColor()}};
    } else if (piece.shape == PieceShape::Vertical2) {
        piece.cells = {{0, 0, randomColor()}, {0, 1, randomColor()}};
    } else if (piece.shape == PieceShape::Horizontal2) {
        piece.cells = {{0, 0, randomColor()}, {1, 0, randomColor()}};
    }

    piece.x = (COLS / 2) - 1;
    piece.y = 0;
    return piece;
}

bool Game::canPlace(const Piece& piece, int newX, int newY) const {
    for (const auto& cell : piece.cells) {
        const int col = newX + cell.dx;
        const int row = newY + cell.dy;
        if (!board_.isInside(row, col) || !board_.isEmpty(row, col)) {
            return false;
        }
    }
    return true;
}

bool Game::tryMove(int dx, int dy) {
    if (!hasCurrentPiece_) return false;
    const int newX = currentPiece_.x + dx;
    const int newY = currentPiece_.y + dy;
    if (!canPlace(currentPiece_, newX, newY)) {
        return false;
    }
    currentPiece_.x = newX;
    currentPiece_.y = newY;
    touchingGround_ = false;
    lockTimer_ = 0.0f;
    return true;
}

void Game::tryRotate() {
    if (!hasCurrentPiece_) return;
    Piece rotated = currentPiece_;
    rotated.rotate();

    if (canPlace(rotated, rotated.x, rotated.y)) {
        currentPiece_ = rotated;
    } else if (canPlace(rotated, rotated.x - 1, rotated.y)) {
        rotated.x -= 1;
        currentPiece_ = rotated;
    } else if (canPlace(rotated, rotated.x + 1, rotated.y)) {
        rotated.x += 1;
        currentPiece_ = rotated;
    } else {
        return;
    }

    touchingGround_ = false;
    lockTimer_ = 0.0f;
}

void Game::lockPiece() {
    if (!hasCurrentPiece_) return;

    for (const auto& cell : currentPiece_.cells) {
        const int col = currentPiece_.x + cell.dx;
        const int row = currentPiece_.y + cell.dy;
        board_.setCell(row, col, cell.type);
    }

    score_ += static_cast<int>(currentPiece_.cells.size()) * 10;

    // TODO:
    // 1. 4個以上の連結判定
    // 2. 消去処理
    // 3. レインボー効果
    // 4. 重力と連鎖処理

    hasCurrentPiece_ = false;
    touchingGround_ = false;
    lockTimer_ = 0.0f;
}

int Game::colorCountForDifficulty() const {
    switch (difficulty_) {
        case Difficulty::Easy: return 5;
        case Difficulty::Normal: return 6;
        case Difficulty::Hard: return 7;
    }
    return 6;
}

float Game::rainbowChanceForDifficulty() const {
    switch (difficulty_) {
        case Difficulty::Easy: return 0.10f;
        case Difficulty::Normal: return 0.06f;
        case Difficulty::Hard: return 0.03f;
    }
    return 0.06f;
}
