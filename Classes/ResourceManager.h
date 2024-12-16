#pragma once
#include "cocos2d.h"
#include <unordered_map>
#include <string>

// ǰ������ GameMap ��
class GameMap;

class ResourceManager
{
public:
    // ��ȡ����ʵ��
    static ResourceManager* getInstance();

    // ��ʼ����Դ������������ GameMap ָ��
    bool init(GameMap* gameMap);

    // �������б�Ҫ����Դ
    void loadResources();

    // ��ȡ�ض�ͼ�������ID
    int getTileID(const std::string& tileName) const;

    // ��ȡ�滻���ͼ��ID
    int getReplacementTileID() const { return REPLACEMENT_TILE_ID; }

private:
    ResourceManager() = default;
    ~ResourceManager() = default;

    // ���ÿ�������͸�ֵ
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    // ͼ��������ID��ӳ��
    std::unordered_map<std::string, int> tileIDMap;

    // ��Ϸ��ͼ����
    GameMap* _gameMap = nullptr;

    // �滻ͼ��ID
    const int REPLACEMENT_TILE_ID = 227;

    // ���� Path.png ͼ�鼯
    void loadPathTileset();
};