#include "ResourceManager.hpp"
#include <stdexcept>

bool ResourceManager::load() {
    const std::map<BlockType, std::string> paths = {
        {BlockType::Red, "assets/images/red.png"},
        {BlockType::Blue, "assets/images/blue.png"},
        {BlockType::Green, "assets/images/green.png"},
        {BlockType::Yellow, "assets/images/yellow.png"},
        {BlockType::Purple, "assets/images/purple.png"},
        {BlockType::Orange, "assets/images/orange.png"},
        {BlockType::Cyan, "assets/images/cyan.png"},
        {BlockType::Rainbow, "assets/images/rainbow.png"},
    };

    for (const auto& [type, path] : paths) {
        sf::Texture tex;
        if (tex.loadFromFile(path)) {
            textures_.emplace(type, std::move(tex));
        }
    }

    fontLoaded_ = font_.loadFromFile("assets/fonts/NotoSansJP-Regular.ttf");
    return true;
}

bool ResourceManager::hasFont() const {
    return fontLoaded_;
}

bool ResourceManager::hasTexture(BlockType type) const {
    return textures_.find(type) != textures_.end();
}

const sf::Texture& ResourceManager::getTexture(BlockType type) const {
    return textures_.at(type);
}

const sf::Font& ResourceManager::getFont() const {
    return font_;
}
