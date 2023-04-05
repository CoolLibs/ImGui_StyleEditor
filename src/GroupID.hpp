#pragma once
#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>
#include <string>

namespace ImStyleEd {

struct GroupID {
    std::string category_name{};
    std::string group_name{};

private:
    // Serialization
    friend class cereal::access;
    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(
            cereal::make_nvp("Category name", category_name),
            cereal::make_nvp("Group name", group_name)
        );
    }
};

} // namespace ImStyleEd
