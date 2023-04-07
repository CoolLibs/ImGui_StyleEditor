#pragma once
#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>
#include <string>
#include <vector>
#include "Group.hpp"

namespace ImStyleEd {

struct Category {
    explicit Category(std::string name = "")
        : name{std::move(name)}
    {}
    std::string        name{};
    bool               behaves_differently_in_light_mode{true};
    std::vector<Group> groups{{Group{}}};

private:
    // Serialization
    friend class cereal::access;
    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(
            cereal::make_nvp("Name", name),
            cereal::make_nvp("Behaves differently in light mode", behaves_differently_in_light_mode),
            cereal::make_nvp("Groups", groups)
        );
    }
};

} // namespace ImStyleEd