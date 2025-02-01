#pragma once
#include <filesystem>

namespace ImStyleEd {

// We go through this interface because we don't want users of our library to receive nlohmann::json as a public dependency
template<typename T>
class ISerializer {
public:
    ISerializer()                                  = default;
    ISerializer(ISerializer const&)                = delete;
    ISerializer& operator=(ISerializer const&)     = delete;
    ISerializer(ISerializer&&) noexcept            = delete;
    ISerializer& operator=(ISerializer&&) noexcept = delete;
    virtual ~ISerializer()                         = default;

    virtual void save(T const&, std::filesystem::path const& file_path) = 0;
    virtual void load(T&, std::filesystem::path const& file_path)       = 0;
};

} // namespace ImStyleEd