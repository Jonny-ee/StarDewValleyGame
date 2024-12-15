#pragma once
#include "cocos2d.h"
#include "GameMap.h"
#include "Player.h"

USING_NS_CC;

/*
 * ũ���������
 * ���ܣ�������Ϸ�е�ũ��ϵͳ
 * 1.���ؿ���
 * 2.ͼ��״̬����
 * 3.ũ����������
 */
class CropManager
{
public:
    static CropManager* getInstance();                      // ��ȡ����ʵ��

    bool canTill(const Vec2& tilePos) const;               // ���ָ��λ���Ƿ���Կ���
    bool tillSoil(const Vec2& tilePos);                    // ����ָ��λ�õ�����

    bool canWater(const Vec2& tilePos) const;              // ���ָ��λ���Ƿ���Խ�ˮ
    bool waterSoil(const Vec2& tilePos);                   // ��ˮָ��λ�õ�����

    void setGameMap(GameMap* map) { _gameMap = map; }      // ���õ�ǰ��Ϸ��ͼ����
    void onMouseDown(const Vec2& mousePos, Player* player); // ��������¼�

private:
    CropManager() {}                // ˽�й��캯��������ģʽ��
    static CropManager* _instance;  // ����ʵ��ָ��
    GameMap* _gameMap = nullptr;    // ��ǰ��ͼ����

    // ��ˮЧ����س���
    const Color3B WATER_COLOR = Color3B(173, 216, 230);    // ��ˮ�����ɫ(����ɫ)
    const float WATER_DURATION = 1.0f;                      // ��ˮЧ������ʱ��
    const float FADE_DURATION = 0.3f;                       // ��ɫ�ָ�����ʱ��

    /*
     * ͼ��ID˵����
     * 1. Tiled�༭����ͼ��ID��0��ʼ��ʵ��ʹ��ʱ��Ҫ��1
     * 2. ��ΪTiled��0��ʾ��ͼ�飬����ʵ��ͼ��ID = Tiled�п��Ѻ�ID + 1
     * 3. ����ʹ�õĶ���ID��Ϊʵ��ʹ�õ�ID���Ѽ�1��
     */
    const std::vector<int> TILLABLE_TILES = { 207, 227, 228, 232, 464 };    // �ɿ��ѵ�ͼ��ID�б�
    const int TILLED_TILE_ID = 681;                                         // ���Ѻ��ͼ��ID���ѿ��ѵ�ũ��ͼ�飩

    void createWaterEffect(Sprite* tile);                   // ������ˮЧ��
    void showWateringPopup();                              // ��ʾ��ˮ��ʾ
};