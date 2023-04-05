#pragma once
#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>
#include <string>

namespace ImStyleEd {

struct Group {
    std::string name{"Unnamed group"};
    float       brightness_delta{0.f}; // Between -1 and 1. 0 means using the same color as the category, > 0 means using a brighter one, and < 0 a darker one.
    float       opacity{1.f};

private:
    // Serialization
    friend class cereal::access;
    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(
            cereal::make_nvp("Name", name),
            cereal::make_nvp("Brightness delta", brightness_delta),
            cereal::make_nvp("Opacity", opacity)
        );
    }
};

} // namespace ImStyleEd