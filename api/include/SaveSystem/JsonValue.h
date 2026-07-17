GIT#pragma once
#include <string>
#include <vector>
#include <map>
#include <variant>
#include <sstream>
#include <stdexcept>
#include <cctype>
#include <cmath>

// ============================================================
// JsonValue : mini representation JSON maison (sans dependance)
// Supporte : null, bool, nombre, string, tableau, objet.
// ============================================================

class JsonValue {
public:
    using Array = std::vector<JsonValue>;
    using Object = std::map<std::string, JsonValue>;
    using ValueType = std::variant<std::nullptr_t, bool, double, std::string, Array, Object>;

    JsonValue() : m_value(nullptr) {}
    JsonValue(std::nullptr_t) : m_value(nullptr) {}
    JsonValue(bool b) : m_value(b) {}
    JsonValue(int i) : m_value(static_cast<double>(i)) {}
    JsonValue(long long i) : m_value(static_cast<double>(i)) {}
    JsonValue(size_t i) : m_value(static_cast<double>(i)) {}
    JsonValue(float f) : m_value(static_cast<double>(f)) {}
    JsonValue(double d) : m_value(d) {}
    JsonValue(const char* s) : m_value(std::string(s)) {}
    JsonValue(const std::string& s) : m_value(s) {}
    JsonValue(const Array& a) : m_value(a) {}
    JsonValue(const Object& o) : m_value(o) {}

    bool IsNull()   const { return std::holds_alternative<std::nullptr_t>(m_value); }
    bool IsBool()   const { return std::holds_alternative<bool>(m_value); }
    bool IsNumber() const { return std::holds_alternative<double>(m_value); }
    bool IsString() const { return std::holds_alternative<std::string>(m_value); }
    bool IsArray()  const { return std::holds_alternative<Array>(m_value); }
    bool IsObject() const { return std::holds_alternative<Object>(m_value); }

    bool AsBool()   const { return std::get<bool>(m_value); }
    double AsDouble() const { return std::get<double>(m_value); }
    int AsInt()     const { return static_cast<int>(std::get<double>(m_value)); }
    const std::string& AsString() const { return std::get<std::string>(m_value); }
    const Array& AsArray() const { return std::get<Array>(m_value); }
    Array& AsArray() { if (!IsArray()) m_value = Array{}; return std::get<Array>(m_value); }
    const Object& AsObject() const { return std::get<Object>(m_value); }
    Object& AsObject() { if (!IsObject()) m_value = Object{}; return std::get<Object>(m_value); }

    // Acces objet : cree la cle si absente (comme un std::map)
    JsonValue& operator[](const std::string& key) {
        return AsObject()[key];
    }

    const JsonValue* Find(const std::string& key) const {
        if (!IsObject()) return nullptr;
        auto& obj = std::get<Object>(m_value);
        auto it = obj.find(key);
        return it == obj.end() ? nullptr : &it->second;
    }

    bool Contains(const std::string& key) const { return Find(key) != nullptr; }
    void Erase(const std::string& key) { if (IsObject()) std::get<Object>(m_value).erase(key); }

    void PushBack(JsonValue v) { AsArray().push_back(std::move(v)); }

    // ---------------- Serialisation ----------------
    std::string Dump(int indent = 4) const {
        std::string out;
        DumpImpl(out, indent, 0);
        return out;
    }

    // ---------------- Parsing ----------------
    static JsonValue Parse(const std::string& text) {
        size_t i = 0;
        SkipWhitespace(text, i);
        JsonValue result = ParseValue(text, i);
        SkipWhitespace(text, i);
        if (i != text.size()) {
            throw std::runtime_error("Caracteres en trop apres le JSON");
        }
        return result;
    }

private:
    ValueType m_value;

    static void EscapeString(const std::string& s, std::string& out) {
        out += '"';
        for (char c : s) {
            switch (c) {
                case '"':  out += "\\\""; break;
                case '\\': out += "\\\\"; break;
                case '\n': out += "\\n"; break;
                case '\t': out += "\\t"; break;
                case '\r': out += "\\r"; break;
                default:
                    out += c;
            }
        }
        out += '"';
    }

    static void DumpNumber(double d, std::string& out) {
        if (d == static_cast<long long>(d)) {
            out += std::to_string(static_cast<long long>(d));
        } else {
            std::ostringstream oss;
            oss << d;
            out += oss.str();
        }
    }

    void DumpImpl(std::string& out, int indent, int depth) const {
        std::string pad(indent * (depth + 1), ' ');
        std::string padEnd(indent * depth, ' ');

        if (IsNull()) {
            out += "null";
        } else if (IsBool()) {
            out += AsBool() ? "true" : "false";
        } else if (IsNumber()) {
            DumpNumber(AsDouble(), out);
        } else if (IsString()) {
            EscapeString(AsString(), out);
        } else if (IsArray()) {
            const auto& arr = AsArray();
            if (arr.empty()) { out += "[]"; return; }
            out += "[\n";
            for (size_t idx = 0; idx < arr.size(); ++idx) {
                out += pad;
                arr[idx].DumpImpl(out, indent, depth + 1);
                if (idx + 1 < arr.size()) out += ",";
                out += "\n";
            }
            out += padEnd + "]";
        } else if (IsObject()) {
            const auto& obj = AsObject();
            if (obj.empty()) { out += "{}"; return; }
            out += "{\n";
            size_t idx = 0;
            for (const auto& [key, val] : obj) {
                out += pad;
                EscapeString(key, out);
                out += ": ";
                val.DumpImpl(out, indent, depth + 1);
                if (++idx < obj.size()) out += ",";
                out += "\n";
            }
            out += padEnd + "}";
        }
    }

    static void SkipWhitespace(const std::string& s, size_t& i) {
        while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i]))) ++i;
    }

    static std::string ParseString(const std::string& s, size_t& i) {
        if (s[i] != '"') throw std::runtime_error("Chaine attendue");
        ++i;
        std::string out;
        while (i < s.size() && s[i] != '"') {
            char c = s[i];
            if (c == '\\' && i + 1 < s.size()) {
                char next = s[i + 1];
                switch (next) {
                    case '"':  out += '"'; break;
                    case '\\': out += '\\'; break;
                    case '/':  out += '/'; break;
                    case 'n':  out += '\n'; break;
                    case 't':  out += '\t'; break;
                    case 'r':  out += '\r'; break;
                    default:   out += next; break;
                }
                i += 2;
            } else {
                out += c;
                ++i;
            }
        }
        if (i >= s.size()) throw std::runtime_error("Chaine JSON non terminee");
        ++i; // skip closing quote
        return out;
    }

    static JsonValue ParseValue(const std::string& s, size_t& i) {
        SkipWhitespace(s, i);
        if (i >= s.size()) throw std::runtime_error("JSON incomplet");

        char c = s[i];
        if (c == '"') {
            return JsonValue(ParseString(s, i));
        }
        if (c == '{') {
            ++i;
            Object obj;
            SkipWhitespace(s, i);
            if (i < s.size() && s[i] == '}') { ++i; return JsonValue(obj); }
            while (true) {
                SkipWhitespace(s, i);
                std::string key = ParseString(s, i);
                SkipWhitespace(s, i);
                if (i >= s.size() || s[i] != ':') throw std::runtime_error("':' attendu");
                ++i;
                JsonValue val = ParseValue(s, i);
                obj[key] = val;
                SkipWhitespace(s, i);
                if (i < s.size() && s[i] == ',') { ++i; continue; }
                if (i < s.size() && s[i] == '}') { ++i; break; }
                throw std::runtime_error("',' ou '}' attendu");
            }
            return JsonValue(obj);
        }
        if (c == '[') {
            ++i;
            Array arr;
            SkipWhitespace(s, i);
            if (i < s.size() && s[i] == ']') { ++i; return JsonValue(arr); }
            while (true) {
                JsonValue val = ParseValue(s, i);
                arr.push_back(val);
                SkipWhitespace(s, i);
                if (i < s.size() && s[i] == ',') { ++i; continue; }
                if (i < s.size() && s[i] == ']') { ++i; break; }
                throw std::runtime_error("',' ou ']' attendu");
            }
            return JsonValue(arr);
        }
        if (s.compare(i, 4, "true") == 0) { i += 4; return JsonValue(true); }
        if (s.compare(i, 5, "false") == 0) { i += 5; return JsonValue(false); }
        if (s.compare(i, 4, "null") == 0) { i += 4; return JsonValue(nullptr); }

        // Nombre
        size_t start = i;
        if (s[i] == '-') ++i;
        while (i < s.size() && (std::isdigit(static_cast<unsigned char>(s[i])) || s[i] == '.' ||
                                 s[i] == 'e' || s[i] == 'E' || s[i] == '+' || s[i] == '-')) {
            ++i;
        }
        if (i == start) throw std::runtime_error("Valeur JSON invalide");
        return JsonValue(std::stod(s.substr(start, i - start)));
    }
};
