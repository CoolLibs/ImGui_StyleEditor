#pragma once
#include <imgui/imgui.h>
#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>
#include <functional>
#include <string>

namespace ImStyleEd {

struct Element {
    std::string                        name{};
    std::function<ImVec4()>            get_color{};   // If you use a lambda, please don't capture any references as this function will last over time.
    std::function<void(ImVec4 const&)> set_color{};   // If you use a lambda, please don't capture any references as this function will last over time.
    std::string                        description{}; // Optional description that will be displayed in the big config UI where you assign each element to a category / group.

private:
    // Serialization
    friend class cereal::access;
    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(
            cereal::make_nvp("Name", name)
            // The rest doesn't need to be serialized. It is set when registering elements in the Config.
        );
    }
};

} // namespace ImStyleEd