#include "CropManager.h"

CropManager* CropManager::_instance = nullptr;

CropManager* CropManager::getInstance()
{
    if (!_instance)
    {
        _instance = new CropManager();
    }
    return _instance;
}

bool CropManager::canTill(const Vec2& tilePos) const
{
    if (!_gameMap)
        return false;

    // ����Ƿ���Farm��ͼ
    if (_gameMap->getMapName() != "Farm")
        return false;

    auto backLayer = _gameMap->getTileMap()->getLayer("Back");
    if (!backLayer)
        return false;

    // ��ȡ��ǰλ�õ�ͼ��ID
    int tileGID = backLayer->getTileGIDAt(tilePos);

    // ����Ƿ��ǿɿ��ѵ�ͼ��
    return std::find(TILLABLE_TILES.begin(), TILLABLE_TILES.end(), tileGID) != TILLABLE_TILES.end();
}

bool CropManager::tillSoil(const Vec2& tilePos)
{
    if (!canTill(tilePos))
        return false;

    auto backLayer = _gameMap->getTileMap()->getLayer("Back");
    if (!backLayer)
        return false;

    // ��ͼ���Ϊ���Ѻ��ͼ��
    backLayer->setTileGID(TILLED_TILE_ID, tilePos);

    return true;
}

void CropManager::onMouseDown(const Vec2& mousePos, Player* player)
{
    if (!player || !_gameMap || player->getCurrentTool() != Player::ToolType::SHOVEL)
    {
        return;
    }

    // ����Ƿ���Farm��ͼ
    if (_gameMap->getMapName() != "Farm")
    {
        return;
    }

    Vec2 playerPos = player->getPosition();
    Vec2 playerTilePos = _gameMap->convertToTileCoord(playerPos);

    if (canTill(playerTilePos))
    {
        if (tillSoil(playerTilePos))
        {
            player->performAction(mousePos);
        }
    }
}