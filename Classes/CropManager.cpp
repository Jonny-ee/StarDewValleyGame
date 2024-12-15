#include "CropManager.h"

CropManager* CropManager::_instance = nullptr;  // 初始化静态成员变量

/*
 * 获取农作物管理系统单例实例
 * @return 农作物管理系统实例指针
 */
CropManager* CropManager::getInstance()
{
    if (!_instance)  // 如果实例不存在，创建新实例
    {
        _instance = new CropManager();
    }
    return _instance;
}

/*
 * 检查指定位置是否可以开垦
 * @param tilePos 要检查的瓦片坐标
 * @return 如果可以开垦返回true，否则返回false
 */
bool CropManager::canTill(const Vec2& tilePos) const
{
    if (!_gameMap)  // 检查地图是否存在
        return false;

    if (_gameMap->getMapName() != "Farm")  // 只允许在Farm地图开垦
        return false;

    auto backLayer = _gameMap->getTileMap()->getLayer("Back");  // 获取背景图层
    if (!backLayer)  // 检查图层是否存在
        return false;

    int tileGID = backLayer->getTileGIDAt(tilePos);  // 获取指定位置的图块ID

    return std::find(TILLABLE_TILES.begin(), TILLABLE_TILES.end(), tileGID) != TILLABLE_TILES.end();  // 检查是否是可开垦图块
}

/*
 * 开垦指定位置的土地
 * @param tilePos 要开垦的瓦片坐标
 * @return 开垦成功返回true，失败返回false
 */
bool CropManager::tillSoil(const Vec2& tilePos)
{
    if (!canTill(tilePos))  // 检查是否可以开垦
        return false;

    auto backLayer = _gameMap->getTileMap()->getLayer("Back");  // 获取背景图层
    if (!backLayer)  // 检查图层是否存在
        return false;

    backLayer->setTileGID(TILLED_TILE_ID, tilePos);  // 将图块更改为已开垦状态

    return true;
}

/*
 * 处理鼠标点击事件，执行开垦操作
 * @param mousePos 鼠标点击位置
 * @param player 玩家对象指针
 */
void CropManager::onMouseDown(const Vec2& mousePos, Player* player)
{
    if (!player || !_gameMap || player->getCurrentTool() != Player::ToolType::SHOVEL)  // 检查必要条件
    {
        return;
    }

    if (_gameMap->getMapName() != "Farm")  // 确保只在Farm地图中执行
    {
        return;
    }

    Vec2 playerPos = player->getPosition();  // 获取玩家位置
    Vec2 playerTilePos = _gameMap->convertToTileCoord(playerPos);  // 转换为瓦片坐标

    if (canTill(playerTilePos))  // 检查是否可以开垦
    {
        if (tillSoil(playerTilePos))  // 尝试开垦土地
        {
            player->performAction(mousePos);  // 执行开垦动作
        }
    }
}