#pragma once
#include <fstream>
#include "ErrorHandlers.hpp"
#include "ISerializer.hpp"
#include "nlohmann/json.hpp"

namespace ImStyleEd {

template<typename T>
void json_set(nlohmann::json& json, std::string_view key, T const& val)
{
    to_json(json[key], val); // We don't write `json[key] = val` because we don't want to overwrite values in the json that aren't used anymore, because they might be used by another version of the software.
}

template<typename T>
auto json_get(nlohmann::json const& json, std::string_view key, T& val) -> bool
{
    auto const it = json.find(key);
    if (it == json.end())
        return false;

    try
    {
        from_json(*it, val);
        return true;
    }
    catch (...) // NOLINT(*empty-catch) Ignore the error, it is totally possible that the key is not of the right type
    {}

    return false;
}

template<typename T>
class JsonSerializer : public ISerializer<T> {
public:
    JsonSerializer(
        std::function<void(nlohmann::json&, T const&)> save,
        std::function<void(nlohmann::json const&, T&)> load
    )
        : _save{std::move(save)}, _load{std::move(load)}
    {}
    void save(T const& val, std::filesystem::path const& file_path) override
    {
        try
        {
            _save(_json, val);
            auto ofs = std::ofstream{file_path};
            if (!ofs.is_open())
            {
                error_handlers().on_mandatory_warning("Your changes will not be saved. If you want them to be saved, please restart the program as an administrator.");
                return;
            }
            ofs << _json.dump(1);
        }
        catch (std::exception const& e)
        {
            error_handlers().on_optional_warning("Failed to save file \"" + file_path.string() + "\":\n" + e.what());
        }
    }
    void load(T& val, std::filesystem::path const& file_path) override
    {
        if (!std::filesystem::exists(file_path))
            return; // Don't try to ifstream the file, it would log an error which we don't care about
        try
        {
            std::ifstream{file_path} >> _json; // NB: this clears the _json from its previous values. This means that when loading twice in a row (e.g. from UserDataDefault and then from UserData), only the last load will have its values stored in the _json. This is fine because we don't care about saving values from UserDataDefault into UserData if they are not used.
            _load(_json, val);
        }
        catch (std::exception const& e)
        {
            error_handlers().on_optional_warning("Corrupted file \"" + file_path.string() + "\":\n" + e.what());
        }
    }

private:
    nlohmann::json _json{}; // We need to store the json, because when we save it we want to keep all the unused fields that we found when we loaded the json. Otherwise we might remove information that is usefull in another version of the software.

    std::function<void(nlohmann::json&, T const&)> _save;
    std::function<void(nlohmann::json const&, T&)> _load;
};

} // namespace ImStyleEd