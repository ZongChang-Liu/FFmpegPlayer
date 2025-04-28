//
// Created by liu_zongchang on 2025/1/11 20:01.
// Email 1439797751@qq.com
// 
//

#include "Microscope_Utils_Config.h"
#include "Microscope_Utils_Log.h"
#include <filesystem>
#include <fstream>
#include <queue>
#include <QDir>

Microscope_Utils_Config* Microscope_Utils_Config::m_instance = nullptr;
std::mutex Microscope_Utils_Config::m_mutex;

Microscope_Utils_Config* Microscope_Utils_Config::getInstance()
{
    if (m_instance == nullptr)
    {
        m_mutex.lock();
        if (m_instance == nullptr)
        {
            m_instance = new Microscope_Utils_Config();
        }
        m_mutex.unlock();
    }
    return m_instance;
}


Microscope_Utils_Config::Microscope_Utils_Config()
{
    m_doc = std::make_shared<tinyxml2::XMLDocument>();
}

Microscope_Utils_Config::~Microscope_Utils_Config() = default;

std::shared_ptr<tinyxml2::XMLDocument> Microscope_Utils_Config::getDoc() const
{
    return m_doc;
}

void Microscope_Utils_Config::init(const std::string& configPath)
{
    if (!std::filesystem::exists(configPath))
    {
        LOG_WARN("Config file {} not exists, create a new one ", configPath);
        if (createConfigFile(configPath) != 0)
        {
            return;
        }
    }

    if (const tinyxml2::XMLError ret = m_doc->LoadFile(configPath.c_str()); ret != tinyxml2::XML_SUCCESS)
    {
        LOG_ERROR("Failed to load config file {}", m_doc->ErrorName());
        if (createConfigFile(configPath) != 0)
        {
            return;
        }
    }

    m_root = m_doc->RootElement();
    if (m_root == nullptr)
    {
        LOG_ERROR("Failed to get root element");
        return;
    }

    m_inited = true;
    m_configPath = configPath;

    bool ret = isElementExist("FilePath", "SystemSetting") ;
    if(!ret) {
        createElement("FilePath", "SystemSetting");
        QString dirHome = QDir::homePath();
        QDir dir(dirHome + "/Explore");
        if (!dir.exists())
        {
            dir.mkpath(dirHome + "/Explore");
            LOG_ERROR("FilePath init failed");
        }
        if(setElementValue("FilePath", dir.path().toStdString(), "SystemSetting")!=0) {
            LOG_ERROR(dir.path().toStdString() +" FilePath init failed");
        }
    }
    LOG_ERROR("FilePath init succes");

}

int Microscope_Utils_Config::setTranslator(const std::string& translator) const
{
    if (!m_inited)
    {
        LOG_ERROR("Config not inited");
        return -1;
    }

    if (const int ret = isElementExist("Translator", "SystemSetting") ? setElementValue("Translator", translator, "SystemSetting") : createElement("Translator", "SystemSetting"); ret != 0)
    {
        LOG_ERROR("Failed to set translator");
        return -1;
    }
    return 0;
}

int Microscope_Utils_Config::getTranslator(std::string& translator) const
{
    return getElementValue("Translator", translator, "SystemSetting");
}

int Microscope_Utils_Config::getFilePath(std::string &filePath) const {
    return getElementValue("FilePath", filePath, "SystemSetting");
}

int Microscope_Utils_Config::setFilePath(const std::string &filePath) const {
    if (!m_inited)
    {
        LOG_ERROR("Config not inited");
        return -1;
    }

    if (const int ret = isElementExist("FilePath", "SystemSetting") ? setElementValue("FilePath", filePath, "SystemSetting") : createElement("FilePath", "SystemSetting"); ret != 0)
    {
        LOG_ERROR("Failed to set filePath");
        return -1;
    }
    return 0;
}

int Microscope_Utils_Config::createConfigFile(const std::string& configPath)
{
    const auto declaration = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>)";
    tinyxml2::XMLDocument doc;
    doc.Parse(declaration);
    tinyxml2::XMLElement* root = doc.NewElement(ROOT_ELEMENT_NAME);
    doc.InsertEndChild(root);

    tinyxml2::XMLElement* childElement = doc.NewElement("SystemSetting");
    root->InsertEndChild(childElement);

    if (const tinyxml2::XMLError ret = doc.SaveFile(configPath.c_str()); ret == tinyxml2::XML_SUCCESS)
    {
        LOG_INFO("Create config file {} success", configPath);
        return 0;
    }

    LOG_ERROR("Failed to create config file {}, error : {}", configPath, doc.ErrorName());
    return -1;
}


int Microscope_Utils_Config::createElement(const std::string& childName, const std::string& parentName) const
{
    if (childName.empty())
    {
        LOG_ERROR("Child name is empty");
        return -1;
    }

    if (parentName.empty())
    {
        LOG_ERROR("Parent name is empty");
        return -1;
    }

    if (!m_inited)
    {
        LOG_ERROR("Config not inited");
        return -1;
    }

    const std::list<tinyxml2::XMLElement*> parentElements = findNode(m_root, parentName);
    if (parentElements.empty())
    {
        LOG_ERROR("Failed to get parent element {}", parentName);
        return -1;
    }

    for (const auto parentElement : parentElements)
    {
        if (std::list<tinyxml2::XMLElement*> elements = findNode(parentElement, childName); !elements.
            empty())
        {
            LOG_WARN("Element {} already exists", childName);
            continue;
        }

        tinyxml2::XMLElement* childElement = m_doc->NewElement(childName.c_str());
        parentElement->InsertEndChild(childElement);
        LOG_INFO("Create element {} success", childName);
    }

    if (const tinyxml2::XMLError ret = m_doc->SaveFile(m_configPath.c_str()); ret != tinyxml2::XML_SUCCESS)
    {
        LOG_ERROR("Failed to create config file {}, error : {}", m_configPath, m_doc->ErrorName());
        return -1;
    }

    return 0;
}

int Microscope_Utils_Config::removeElement(const std::string& childName, const std::string& parentName) const
{
    if (childName.empty())
    {
        LOG_ERROR("Element name is empty");
        return -1;
    }

    if (parentName.empty())
    {
        LOG_ERROR("Parent name is empty");
        return -1;
    }

    if (!m_inited)
    {
        LOG_ERROR("Config not inited");
        return -1;
    }

    std::list<tinyxml2::XMLElement*> parentElements = findNode(m_root, parentName);
    if (parentElements.empty())
    {
        LOG_ERROR("Failed to get parent element {}", parentName);
        return -1;
    }

    for (const auto parentElement : parentElements)
    {
        tinyxml2::XMLElement* childElement = parentElement->FirstChildElement(childName.c_str());
        while (childElement != nullptr)
        {
            parentElement->DeleteChild(childElement);
            LOG_INFO("Remove element {} success", childName);
            childElement = parentElement->FirstChildElement(childName.c_str());
        }
    }

    if (const tinyxml2::XMLError ret = m_doc->SaveFile(m_configPath.c_str()); ret != tinyxml2::XML_SUCCESS)
    {
        LOG_ERROR("Failed to create config file {}, error : {}", m_configPath, m_doc->ErrorName());
        return -1;
    }

    return 0;
}

int Microscope_Utils_Config::getElementValue(const std::string& childName, std::string& childValue,
                                             const std::string& parentName) const
{
    if (childName.empty())
    {
        LOG_ERROR("Child name is empty");
        return -1;
    }

    if (parentName.empty())
    {
        LOG_ERROR("Parent name is empty");
        return -1;
    }

    if (!m_inited)
    {
        LOG_ERROR("Config not inited");
        return -1;
    }

    tinyxml2::XMLElement* parentNode = findNode(m_root, parentName).front();
    if (parentNode == nullptr)
    {
        LOG_ERROR("Failed to get parent element {}", parentName);
        return -1;
    }
    const tinyxml2::XMLElement* child_node = parentNode->FirstChildElement(childName.c_str());
    if (child_node == nullptr)
    {
        LOG_ERROR("Failed to get child element {}", childName);
        return -1;
    }

    if (child_node->GetText() == nullptr)
    {
        LOG_ERROR("Element {} value is empty", childName);
        return -1;
    }
    childValue = child_node->GetText();
    LOG_INFO("Get element {} value {} success", childName, child_node->GetText());
    return 0;
}

int Microscope_Utils_Config::setElementValue(const std::string& childName, const std::string& childValue,
                                             const std::string& parentName) const
{
    if (childName.empty())
    {
        LOG_ERROR("Child name is empty");
        return -1;
    }

    if (childValue.empty())
    {
        LOG_ERROR("Child value is empty");
        return -1;
    }

    if (parentName.empty())
    {
        LOG_ERROR("Parent name is empty");
        return -1;
    }

    if (!m_inited)
    {
        LOG_ERROR("Config not inited");
        return -1;
    }

    tinyxml2::XMLElement* parentElement = findNode(m_root, parentName).front();
    if (parentElement == nullptr)
    {
        LOG_ERROR("Failed to get parent element {}", parentName);
        return -1;
    }

    tinyxml2::XMLElement* childElement = parentElement->FirstChildElement(childName.c_str());
    if (childElement == nullptr)
    {
        LOG_ERROR("Failed to get child element {}", childName);
        return -1;
    }

    childElement->SetText(childValue.c_str());

    if (const tinyxml2::XMLError ret = m_doc->SaveFile(m_configPath.c_str()); ret != tinyxml2::XML_SUCCESS)
    {
        LOG_ERROR("Failed to set element value {}, error : {}", childName, m_doc->ErrorName());
        return -1;
    }

    LOG_INFO("Set element {} value {} success", childName, childValue);
    return 0;
}

bool Microscope_Utils_Config::isElementExist(const std::string& childName, const std::string& parentName) const
{
    if (childName.empty())
    {
        LOG_ERROR("childName name is empty");
        return {};
    }

    if (parentName.empty())
    {
        LOG_ERROR("parentName name is empty");
        return {};
    }


    if (!m_inited)
    {
        LOG_ERROR("Config not inited");
        return false;
    }

    bool isExist = false;
    if (parentName == ROOT_ELEMENT_NAME)
    {
        if (const tinyxml2::XMLElement* childElement = m_root->FirstChildElement(childName.c_str()); childElement !=
            nullptr)
        {
            isExist = true;
        }
    }
    else
    {
        const std::list<tinyxml2::XMLElement*> elements = findNode(m_root, parentName);
        if (elements.empty())
        {
            LOG_ERROR("Failed to get parent element {}", parentName);
            return false;
        }

        for (const auto element : elements)
        {
            if (const tinyxml2::XMLElement* childElement = element->FirstChildElement(childName.c_str()); childElement !=
                nullptr)
            {
                isExist = true;
                break;
            }
        }
    }

    return isExist;
}

std::list<tinyxml2::XMLElement*> Microscope_Utils_Config::findNode(tinyxml2::XMLElement* parent,
                                                                   const std::string& nodeName,
                                                                   const std::map<std::string, std::string>& attr)
{
    if (nodeName.empty())
    {
        LOG_ERROR("Node name is empty");
        return {};
    }

    std::queue<tinyxml2::XMLElement*> nodeList;
    nodeList.push(parent);

    std::list<tinyxml2::XMLElement*> result;
    while (!nodeList.empty())
    {
        tinyxml2::XMLElement* current = nodeList.front();
        nodeList.pop();
        if (nodeName == current->Name())
        {
            if (attr.empty())
            {
                result.push_back(current);
            }
            else
            {
                bool isMatch = true;
                for (const auto& [key, value] : attr)
                {
                    if (current->Attribute(key.c_str()) != value)
                    {
                        isMatch = false;
                        break;
                    }
                }
                if (isMatch)
                {
                    result.push_back(current);
                }
            }
        }

        for (tinyxml2::XMLElement* child = current->FirstChildElement(); child != nullptr; child = child->
             NextSiblingElement())
        {
            nodeList.push(child);
        }
    }
    return result;
}
