#pragma once

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

template <typename T>
class CallStack {
   public:
    CallStack() { scopes.push_back({}); }

    using ScopeMap = std::unordered_map<std::string, T>;

    T* Declare(const std::string& name, const T& entry) {
        if (auto iter = scopes.back().find(name); iter != scopes.back().end()) {
            return &iter->second;
        }
        scopes.back()[name] = entry;
        return nullptr;
    }

    void PushScope() { scopes.push_back({}); }
    void PopScope() {
        if (scopes.empty()) throw std::runtime_error("empty scope");
        scopes.pop_back();
    }

    T* Find(const std::string& name) {
        for (auto iter = scopes.rbegin(); iter != scopes.rend(); ++iter) {
            auto found = iter->find(name);
            if (found != iter->end()) return &found->second;
        }
        return nullptr;
    }

   private:
    std::vector<ScopeMap> scopes;
};