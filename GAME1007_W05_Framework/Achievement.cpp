#include "Achievement.h"
#include "tinyxml2.h" // Include the necessary header for saving/loading XML data

namespace Achievements
{
    // Define the array to store achievement statuses
    std::array<bool, 5> mUnlocked = { false, false, false, false, false };

    void Initialize()
    {
        Set(TRIGGER_HAPPY, false);
        Set(EXCAVATOR, false);
        Set(OVERTIME, false);
        Set(WARP_DRIVE, false);
        Set(SPACE_ADMIRAL, false);
    }

    bool Get(Achievement achievement)
    {
        return mUnlocked[achievement];
    }

    void Set(Achievement achievement, bool unlocked)
    {
        mUnlocked[achievement] = unlocked;
    }

    void Save()
    {
        tinyxml2::XMLDocument doc;
        tinyxml2::XMLElement* root = doc.NewElement("Achievements");

        for (size_t i = 0; i < mUnlocked.size(); ++i)
        {
            tinyxml2::XMLElement* achievementElem = doc.NewElement("Achievement");
            achievementElem->SetAttribute("id", static_cast<int>(i));
            achievementElem->SetAttribute("unlocked", mUnlocked[i]);
            root->InsertEndChild(achievementElem);
        }

        doc.InsertEndChild(root);

        doc.SaveFile("achievements.xml");
    }

    void Load()
    {
        tinyxml2::XMLDocument doc;
        if (doc.LoadFile("achievements.xml") == tinyxml2::XML_SUCCESS)
        {
            tinyxml2::XMLElement* root = doc.FirstChildElement("Achievements");
            if (root)
            {
                for (tinyxml2::XMLElement* achievementElem = root->FirstChildElement("Achievement");
                    achievementElem;
                    achievementElem = achievementElem->NextSiblingElement("Achievement"))
                {
                    int id = 0;
                    bool unlocked = false;
                    achievementElem->QueryIntAttribute("id", &id);
                    achievementElem->QueryBoolAttribute("unlocked", &unlocked);
                    mUnlocked[id] = unlocked;
                }
            }
        }
    }

} 
