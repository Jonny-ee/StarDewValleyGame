#include "CropManager.h"

CropManager* CropManager::_instance = nullptr;  // ��ʼ����̬��Ա����

/*
 * ��ȡũ�������ϵͳ����ʵ��
 * @return ũ�������ϵͳʵ��ָ��
 */
CropManager* CropManager::getInstance()
{
    if (!_instance)  // ���ʵ�������ڣ�������ʵ��
    {
        _instance = new CropManager();
    }
    return _instance;
}

/*
 * ���ָ��λ���Ƿ���Կ���
 * @param tilePos Ҫ������Ƭ����
 * @return ������Կ��ѷ���true�����򷵻�false
 */
bool CropManager::canTill(const Vec2& tilePos) const
{
    if (!_gameMap)  // ����ͼ�Ƿ����
        return false;

    if (_gameMap->getMapName() != "Farm")  // ֻ������Farm��ͼ����
        return false;

    auto backLayer = _gameMap->getTileMap()->getLayer("Back");  // ��ȡ����ͼ��
    if (!backLayer)  // ���ͼ���Ƿ����
        return false;

    int tileGID = backLayer->getTileGIDAt(tilePos);  // ��ȡָ��λ�õ�ͼ��ID

    return std::find(TILLABLE_TILES.begin(), TILLABLE_TILES.end(), tileGID) != TILLABLE_TILES.end();  // ����Ƿ��ǿɿ���ͼ��
}

/*
 * ����ָ��λ�õ�����
 * @param tilePos Ҫ���ѵ���Ƭ����
 * @return ���ѳɹ�����true��ʧ�ܷ���false
 */
bool CropManager::tillSoil(const Vec2& tilePos)
{
    if (!canTill(tilePos))  // ����Ƿ���Կ���
        return false;

    auto backLayer = _gameMap->getTileMap()->getLayer("Back");  // ��ȡ����ͼ��
    if (!backLayer)  // ���ͼ���Ƿ����
        return false;

    backLayer->setTileGID(TILLED_TILE_ID, tilePos);  // ��ͼ�����Ϊ�ѿ���״̬

    return true;
}

/*
 * ����������¼���ִ�п��Ѳ���
 * @param mousePos �����λ��
 * @param player ��Ҷ���ָ��
 */
void CropManager::onMouseDown(const Vec2& mousePos, Player* player)
{
    if (!player || !_gameMap || player->getCurrentTool() != Player::ToolType::SHOVEL)  // ����Ҫ����
    {
        return;
    }

    if (_gameMap->getMapName() != "Farm")  // ȷ��ֻ��Farm��ͼ��ִ��
    {
        return;
    }

    Vec2 playerPos = player->getPosition();  // ��ȡ���λ��
    Vec2 playerTilePos = _gameMap->convertToTileCoord(playerPos);  // ת��Ϊ��Ƭ����

    if (canTill(playerTilePos))  // ����Ƿ���Կ���
    {
        if (tillSoil(playerTilePos))  // ���Կ�������
        {
            player->performAction(mousePos);  // ִ�п��Ѷ���
        }
    }
}