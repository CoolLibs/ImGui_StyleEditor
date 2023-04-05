#pragma once
#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>
#include <string>
#include <vector>
#include "Group.hpp"

namespace ImStyleEd {

struct Category {
    std::string        name{"Unnamed Category"};
    std::vector<Group> groups{{Group{}}};

private:
    // Serialization
    friend class cereal::access;
    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(
            cereal::make_nvp("Name", name),
            cereal::make_nvp("Groups", groups)
        );
    }
};

} // namespace ImStyleEd