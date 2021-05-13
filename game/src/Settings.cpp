#include <Settings.h>

Settings::Settings()
{

}

Settings::~Settings()
{
   
}

void Settings::ParseFile()
{
    std::ifstream file(g_ConfigFilePath);

    if (!file.is_open()) {
        throw std::invalid_argument(g_ConfigFilePath);
    }

    char c;
    while ((c = file.get()) != -1) {
    
        if (c == '{') {
            ReadObject(file, m_Data);
        }
        else if (std::isspace(c)) {
            continue;
        }
        else {
            throw std::invalid_argument("Cant read config file");
        }
    }

    file.close();
}

void Settings::ReadObject(std::ifstream& file, FormatObject& object)
{
    while (true) {
        char c = file.get();

        if (std::isspace(c)) {
            continue;
        }
        else if (c == '}') {
            if (true && file.peek() == ',')
                file.get();
            return;
        }
        else if (c == '\"') {
            std::string name = ReadName(file);
            Token<SimpleData> t;

            t.SetData(ReadData(file));
            object.AddData(name, t);

            while (true) {
                c = file.get();
                if (std::isspace(c)) {
                    continue;
                }
                else if (c == ',') {
                    break;
                }
                else if (c == '}') {
                    if (true && file.peek() == ',')
                        file.get();
                    return;
                }
                else {
                    std::cout << c << std::endl;
                    throw std::invalid_argument("Invalid charachters in config file (ParseObejct).");
                }
            }
        }
    }
}

std::string Settings::ReadName(std::ifstream& file)
{
    char c;
    std::string name = ReadString(file, [&file](char c) {
            if (c == ',' || c == ']' || c == '}') file.unget();
            return c == '\"';
        });

    if (name.empty())
        throw std::invalid_argument("Name in file file can not be empty.");

    while (true) {
        c = file.get();

        if (c == ':') {
            break;
        }
        else if (std::isspace(c)) {
            continue;
        }
        else {
            // throw std::invalid_argument("Invalid charachters in file (Parse name).");
        }
    }

    return name;
}

std::string Settings::ReadString(std::ifstream& file, const std::function<bool(char)>& predicate)
{
    char c;
    std::string str;

    while (true) {
        c = file.get();

        if (predicate(c)) {
            break;
        }
        else {
            str += c;
        }
    }

    return str;
}

bool Settings::ReadBool(std::ifstream& file)
{
    auto lambda = [&file](char c) {
        if (c == ',' || c == ']' || c == '}' || c == ' ') {
            file.unget();
            return true;
        }
        return false;
    };
    std::string boolean = ReadString(file, lambda);

    if (boolean == "true") {
        return true;
    }
    else if (boolean == "false") {
        return false;
    }
    else {
        std::cerr << boolean << std::endl;
        throw std::invalid_argument("Invalid charachters in file.");
    }
}

std::variant<SimpleData, std::vector<SimpleData>> Settings::ReadData(std::ifstream& file)
{
    char c;

    while (true) {
        c = file.get();

        if (std::isspace(c)) {
            continue;
        }
        else if (c == '\"') {
            return ReadString(file, [&file](char c) {
                    if (c == ',' || c == ']' || c == '}') file.unget();
                    return c == '\"';
                });
        }
        else if (c == '{') {
            FormatObject object;

            ReadObject(file, object);
            return object;
        }
        else if (std::isdigit(c)) {
            file.unget();
            auto predicate = [&file](char c) {
                if (c == ',' || c == ']' || c == '}') file.unget();
                return !std::isdigit(c) && c != '.';
            };
            std::string number = ReadString(file, predicate);

            if (number.find('.') == std::string::npos) {
                return stoi(number);
            }
            else {
                return std::stof(number);;
            }
        }
        else if (c == '[') {
            return ReadVector(file);
        }
        else {
            std::cout << c << std::endl;
            throw std::invalid_argument("Invalid charachters in config file.");
        }
    }
}

std::vector<SimpleData> Settings::ReadVector(std::ifstream& file)
{
    std::vector<SimpleData> result;
    char c;

    while (true) {
        c = file.get();

        if (c == ']') {
            break;
        }
        else if (c == ',' || std::isspace(c)) {
            continue;
        }
        else {
            file.unget();
            result.push_back(std::get<SimpleData> (ReadData(file)));
        }
    }

    return result;
}