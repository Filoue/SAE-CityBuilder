#include "resource_manager.h"

ResourceManager::ResourceManager() : wood_(0), stone_(0) {}

void ResourceManager::AddWood(int amount) {
    wood_ += amount;
}

void ResourceManager::AddStone(int amount) {
    stone_ += amount;
}

bool ResourceManager::HasEnoughResources(int wood_cost, int stone_cost) const {
    return wood_ >= wood_cost && stone_ >= stone_cost;
}

void ResourceManager::SpendResources(int wood_cost, int stone_cost) {
    wood_ -= wood_cost;
    stone_ -= stone_cost;
}

int ResourceManager::GetWood() const {
    return wood_;
}

int ResourceManager::GetStone() const {
    return stone_;
}
