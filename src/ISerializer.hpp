#pragma once
#include <filesystem>

namespace ImStyleEd {

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