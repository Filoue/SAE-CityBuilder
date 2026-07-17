#pragma once
#include "JsonValue.h"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

// ============================================================
// SaveManager : systeme de sauvegarde generique, ZERO dependance
// externe. Juste JsonValue.h + SaveManager.h a copier dans ton
// projet.
//
// Usage :
//   SaveManager save("save.json");
//   save.Set("playerHP", 80);
//   save.Set("playerName", std::string("Petitfiloue"));
//   save.SaveToFile();
//   save.LoadFromFile();
//   int hp = save.Get<int>("playerHP", 100);
//
// Pour tes propres structs (ex: PlayerData, BossSaveState),
// ecris juste une fonction ToJson() et FromJson() pour ce type
// (voir main_example.cpp). Pas de macro, pas de magie.
// ============================================================

// --- Conversions pour les types de base ---
inline JsonValue ToJson(int v) { return JsonValue(v); }
inline JsonValue ToJson(float v) { return JsonValue(v); }
inline JsonValue ToJson(double v) { return JsonValue(v); }
inline JsonValue ToJson(bool v) { return JsonValue(v); }
inline JsonValue ToJson(const std::string& v) { return JsonValue(v); }

template<typename T>
JsonValue ToJson(const std::vector<T>& v) {
    JsonValue::Array arr;
    arr.reserve(v.size());
    for (const auto& item : v) arr.push_back(ToJson(item));
    return JsonValue(arr);
}

inline void FromJson(const JsonValue& j, int& v) { v = j.AsInt(); }
inline void FromJson(const JsonValue& j, float& v) { v = static_cast<float>(j.AsDouble()); }
inline void FromJson(const JsonValue& j, double& v) { v = j.AsDouble(); }
inline void FromJson(const JsonValue& j, bool& v) { v = j.AsBool(); }
inline void FromJson(const JsonValue& j, std::string& v) { v = j.AsString(); }

template<typename T>
void FromJson(const JsonValue& j, std::vector<T>& v) {
    v.clear();
    for (const auto& item : j.AsArray()) {
        T elem{};
        FromJson(item, elem);
        v.push_back(std::move(elem));
    }
}

class SaveManager {
public:
    SaveManager() : m_data(JsonValue::Object{}) {}
    explicit SaveManager(std::string filePath)
        : m_data(JsonValue::Object{}), m_filePath(std::move(filePath)) {}

    template<typename T>
    void Set(const std::string& key, const T& value) {
        m_data[key] = ToJson(value);
    }

    template<typename T>
    T Get(const std::string& key, const T& defaultValue = T{}) const {
        const JsonValue* found = m_data.Find(key);
        if (!found) return defaultValue;
        try {
            T result{};
            FromJson(*found, result);
            return result;
        } catch (const std::exception& e) {
            std::cerr << "[SaveManager] Erreur lecture cle '" << key << "': " << e.what() << "\n";
            return defaultValue;
        }
    }

    bool Has(const std::string& key) const { return m_data.Contains(key); }
    void Remove(const std::string& key) { m_data.Erase(key); }
    void Clear() { m_data = JsonValue(JsonValue::Object{}); }

    bool SaveToFile(const std::string& filePath = "") const {
        const std::string path = filePath.empty() ? m_filePath : filePath;
        std::ofstream file(path);
        if (!file.is_open()) {
            std::cerr << "[SaveManager] Impossible d'ecrire dans: " << path << "\n";
            return false;
        }
        file << m_data.Dump();
        return true;
    }

    bool LoadFromFile(const std::string& filePath = "") {
        const std::string path = filePath.empty() ? m_filePath : filePath;
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "[SaveManager] Fichier introuvable: " << path << "\n";
            return false;
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        try {
            m_data = JsonValue::Parse(buffer.str());
        } catch (const std::exception& e) {
            std::cerr << "[SaveManager] JSON invalide dans " << path << ": " << e.what() << "\n";
            return false;
        }
        return true;
    }

    const JsonValue& Raw() const { return m_data; }
    JsonValue& Raw() { return m_data; }

private:
    JsonValue m_data;
    std::string m_filePath = "save.json";
};