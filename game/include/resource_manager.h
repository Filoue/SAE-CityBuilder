#ifndef CITYBUILDER_RESOURCE_MANAGER_H
#define CITYBUILDER_RESOURCE_MANAGER_H

class ResourceManager {
public:
    ResourceManager();

    void AddWood(int amount);
    void AddStone(int amount);

    bool HasEnoughResources(int wood_cost, int stone_cost) const;
    void SpendResources(int wood_cost, int stone_cost);

    int GetWood() const;
    int GetStone() const;

private:
    int wood_;
    int stone_;
};

#endif //CITYBUILDER_RESOURCE_MANAGER_H
