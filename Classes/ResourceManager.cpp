#include "ResourceManager.h"
#include "GameMap.h"  // ȷ������ GameMap ��ͷ�ļ�
#include "cocos2d.h"

ResourceManager* ResourceManager::getInstance()
{
    static ResourceManager instance;
    return &instance;
}

bool ResourceManager::init(GameMap* gameMap)
{
    if (!gameMap)
    {
        cocos2d::log("��ʼ�� ResourceManager ʧ�ܣ�gameMap Ϊ nullptr");
        return false;
    }

    _gameMap = gameMap;

    loadResources();

    return true;
}

void ResourceManager::loadResources()
{
    // ���� Path.png ͼ�鼯
    loadPathTileset();

    // ���������Դ���أ�����еĻ�
}

void ResourceManager::loadPathTileset()
{
    // ���� Path.plist λ�� TileSheets �ļ���
    std::string plistPath = "TileSheets/Path.plist";
    std::string pngPath = "TileSheets/Path.png";

    // ���ͼ�鼯�Ƿ��Ѽ��أ������ظ�����
    if (!cocos2d::SpriteFrameCache::getInstance()->isSpriteFramesWithFileLoaded(plistPath))
    {
        cocos2d::SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plistPath, pngPath);
    }

    // ����ͼ��������ID��ӳ��
    // ��ȷ�� Path.plist �д�����Щ���Ƶ�֡
    tileIDMap["grass"] = 14;
    tileIDMap["stone"] = 17;
    tileIDMap["replacement"] = REPLACEMENT_TILE_ID; // 227

    cocos2d::log("Path.png ͼ�鼯�Ѽ��ز�ӳ��");
}

int ResourceManager::getTileID(const std::string& tileName) const
{
    auto it = tileIDMap.find(tileName);
    if (it != tileIDMap.end())
    {
        return it->second;
    }
    else
    {
        cocos2d::log("δ�ҵ�ͼ�����ƣ�%s ��ID", tileName.c_str());
        return -1; // �����׳��쳣������������
    }
}