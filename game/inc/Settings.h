#pragma once

#include <filesystem>
#include <fstream>
#include <functional>
#include <exception>
#include <iostream>
#include <string>
#include <vector>
#include <variant>
#include <map>
#include <unordered_map>

const std::string g_ConfigFilePath = "config.config";

class FormatObject;

template <typename T>
class Token
{
    using Data = std::variant<T, std::vector<T>>;

    Data m_data;
public:
    Data& GetData()
    {
        return m_data;
    }

    template <typename D>
    void SetData(D&& data)
    {
        m_data = std::forward<D> (data);
    }
};

using SimpleData = std::variant<int, float, std::string, FormatObject>;
using ObjectData = std::unordered_map<std::string, Token<SimpleData>>;

class FormatObject
{
    ObjectData m_Data;

    template <typename S>
    decltype(auto) GetData(S&& name)
    {
        return m_Data.at(std::forward<S> (name)).GetData();
    }
public:
    template <typename S, typename D>
    void AddData(S&& name, D&& data)
    {
        m_Data[std::forward<S> (name)] = std::forward<D> (data);
    }

    template <typename S>
    int GetInt(S&& name)
    {
        return std::get<int> (std::get<SimpleData> (GetData(std::forward<S> (name))));
    }

    template <typename S>
    std::string& GetString(S&& name)
    {
        std::string str = std::get<std::string> (std::get<SimpleData> (GetData(std::forward<S> (name))));
        
        return std::get<std::string> (std::get<SimpleData> (GetData(std::forward<S> (name))));;
    }

    template <typename S>
    float GetFloat(S&& name)
    {
        return std::get<float> (std::get<SimpleData> (GetData(std::forward<S> (name))));
    }

    template <typename S>
    FormatObject& GetObject(S&& name)
    {   
        auto object = std::get<FormatObject> (std::get<SimpleData> (GetData(std::forward<S> (name))));
        
        return std::get<FormatObject> (std::get<SimpleData> (GetData(std::forward<S> (name))));;
    }
    

    template <typename S>
    decltype(auto) GetVector(S&& name)
    {
        return std::get<std::vector<SimpleData>> (GetData(std::forward<S> (name)));
    }
};

class Settings
{
    FormatObject m_Data;
    std::filesystem::file_time_type m_Time;
public:
    Settings();
    ~Settings();

    void ParseFile();
    void ReadObject(std::ifstream& file, FormatObject& formatObject);
    bool ReadBool(std::ifstream& file);
    std::string ReadName(std::ifstream& file);
    std::string ReadString(std::ifstream& file, const std::function<bool(char)>& predicate);
    
    std::variant<SimpleData, std::vector<SimpleData>> ReadData(std::ifstream& file);
    std::vector<SimpleData> ReadVector(std::ifstream& file);

    FormatObject& GetData()
    {
        return m_Data;
    }

    void Update();
    bool IsOld() noexcept;
};
