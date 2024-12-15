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

    // 检查是否在Farm地图
    if (_gameMap->getMapName() != "Farm")
        return false;

    auto backLayer = _gameMap->getTileMap()->getLayer("Back");
    if (!backLayer)
        return false;

    // 获取当前位置的图块ID
    int tileGID = backLayer->getTileGIDAt(tilePos);

    // 检查是否是可开垦的图块
    return std::find(TILLABLE_TILES.begin(), TILLABLE_TILES.end(), tileGID) != TILLABLE_TILES.end();
}

bool CropManager::tillSoil(const Vec2& tilePos)
{
    if (!canTill(tilePos))
        return false;

    auto backLayer = _gameMap->getTileMap()->getLayer("Back");
    if (!backLayer)
        return false;

    // 将图块改为开垦后的图块
    backLayer->setTileGID(TILLED_TILE_ID, tilePos);

    return true;
}

void CropManager::onMouseDown(const Vec2& mousePos, Player* player)
{
    if (!player || !_gameMap || player->getCurrentTool() != Player::ToolType::SHOVEL)
    {
        return;
    }

    // 检查是否在Farm地图
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