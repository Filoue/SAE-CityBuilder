#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

// ============================================================
// SaveManager : systeme de sauvegarde generique en JSON
// ============================================================
// Usage :
//   SaveManager save("save.json");
//   save.Set("playerHP", 80);
//   save.Set("playerName", std::string("Petitfiloue"));
//   save.Set("unlockedLevels", std::vector<int>{1, 2, 3});
//   save.SaveToFile();
//
//   save.LoadFromFile();
//   int hp = save.Get<int>("playerHP", 100); // 100 = valeur par defaut
//
// Marche aussi avec des structs/classes perso, voir plus bas
// la macro NLOHMANN_DEFINE_TYPE_INTRUSIVE.
// ============================================================

class SaveManager {
public:
    using json = nlohmann::json;

    SaveManager() = default;
    explicit SaveManager(std::string filePath) : m_filePath(std::move(filePath)) {}

    // Stocke n'importe quelle valeur : int, float, string, bool,
    // vector<T>, map<string,T>, ou un objet perso qui a to_json/from_json.
    template<typename T>
    void Set(const std::string& key, const T& value) {
        m_data[key] = value;
    }

    // Recupere une valeur. Si la cle n'existe pas ou que le type
    // ne correspond pas, renvoie defaultValue au lieu de crasher.
    template<typename T>
    T Get(const std::string& key, const T& defaultValue = T{}) const {
        if (!m_data.contains(key)) {
            return defaultValue;
        }
        try {
            return m_data.at(key).get<T>();
        } catch (const json::exception& e) {
            std::cerr << "[SaveManager] Erreur lecture cle '" << key << "': " << e.what() << "\n";
            return defaultValue;
        }
    }

    bool Has(const std::string& key) const {
        return m_data.contains(key);
    }

    void Remove(const std::string& key) {
        m_data.erase(key);
    }

    void Clear() {
        m_data.clear();
    }

    // Sauvegarde tout le contenu dans un fichier .json (joli et indente)
    bool SaveToFile(const std::string& filePath = "") const {
        const std::string path = filePath.empty() ? m_filePath : filePath;
        std::ofstream file(path);
        if (!file.is_open()) {
            std::cerr << "[SaveManager] Impossible d'ecrire dans: " << path << "\n";
            return false;
        }
        file << m_data.dump(4);
        return true;
    }

    // Charge un fichier .json existant
    bool LoadFromFile(const std::string& filePath = "") {
        const std::string path = filePath.empty() ? m_filePath : filePath;
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "[SaveManager] Fichier introuvable: " << path << "\n";
            return false;
        }
        try {
            file >> m_data;
        } catch (const json::parse_error& e) {
            std::cerr << "[SaveManager] JSON invalide dans " << path << ": " << e.what() << "\n";
            return false;
        }
        return true;
    }

    // Acces direct au json brut si besoin de trucs plus avances
    const json& Raw() const { return m_data; }
    json& Raw() { return m_data; }

private:
    json m_data;
    std::string m_filePath = "save.json";
};
