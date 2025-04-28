//
// Created by liu_zongchang on 2025/1/11 20:01.
// Email 1439797751@qq.com
// 
//

#ifndef MICROSCOPE_UTILS_CONFIG_H
#define MICROSCOPE_UTILS_CONFIG_H

#pragma once
#include <list>
#include <map>
#include <mutex>
#include "tinyxml2.h"

#define configApp Microscope_Utils_Config::getInstance()
#define DEFAULT_CONFIG_PATH "./config.xml"
#define ROOT_ELEMENT_NAME "Config"
#define CONFIG configApp

class Microscope_Utils_Config
{
public:

    static Microscope_Utils_Config* getInstance();
    std::shared_ptr<tinyxml2::XMLDocument> getDoc() const;

    void init(const std::string& configPath = DEFAULT_CONFIG_PATH);

    int setTranslator(const std::string& translator) const;
    int getTranslator(std::string& translator) const;
    int getFilePath(std::string& filePath) const;
    int setFilePath(const std::string&  filePath) const;

    int createElement(const std::string& childName, const std::string& parentName = ROOT_ELEMENT_NAME) const;
    int removeElement(const std::string& childName, const std::string& parentName = ROOT_ELEMENT_NAME) const;
    int getElementValue(const std::string& childName, std::string& childValue,
                        const std::string& parentName = ROOT_ELEMENT_NAME) const;
    int setElementValue(const std::string& childName, const std::string& childValue,
                        const std::string& parentName = ROOT_ELEMENT_NAME) const;
    bool isElementExist(const std::string& childName, const std::string& parentName = ROOT_ELEMENT_NAME) const;
    static std::list<tinyxml2::XMLElement*> findNode(tinyxml2::XMLElement* parent, const std::string& nodeName,
                                                     const std::map<std::string, std::string>& attr = std::map<
                                                         std::string, std::string>());
    static int createConfigFile(const std::string& configPath);

private:
    static Microscope_Utils_Config* m_instance;
    static std::mutex m_mutex;
    Microscope_Utils_Config();
    ~Microscope_Utils_Config();
    Microscope_Utils_Config(const Microscope_Utils_Config&) = delete;
    Microscope_Utils_Config& operator=(const Microscope_Utils_Config&) = delete;

    bool m_inited{false};
    std::shared_ptr<tinyxml2::XMLDocument> m_doc;
    tinyxml2::XMLElement* m_root{nullptr};
    std::string m_configPath{DEFAULT_CONFIG_PATH};
};


#endif //MICROSCOPE_UTILS_CONFIG_H
