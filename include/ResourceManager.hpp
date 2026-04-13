#pragma once

#include "Types.hpp"
#include <SFML/Graphics.hpp>
#include <map>

class ResourceManager {
public:
    bool load();
    bool hasFont() const;
    bool hasTexture(BlockType type) const;
    const sf::Texture& getTexture(BlockType type) const;
    const sf::Font& getFont() const;

private:
    std::map<BlockType, sf::Texture> textures_;
    sf::Font font_;
    bool fontLoaded_{false};
};
