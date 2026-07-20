#pragma once
#include "Vendor/json.hpp"
using json = nlohmann::json;

class ISerializable
{
    public:

    virtual ~ISerializable() = default;
    virtual json ToJson() const = 0;
    virtual void FromJson(const json& j) = 0;
};