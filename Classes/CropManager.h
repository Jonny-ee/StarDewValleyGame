#pragma once
#include "cocos2d.h"
#include "GameMap.h"
#include "Player.h"
#include "GameScene.h" 
#include "ItemSystem.h"
#include "Corn.h"
#include "GameTime.h"

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

    bool canTill(const Vec2& tilePos) const;                // ���ָ��λ���Ƿ���Կ���
    bool tillSoil(const Vec2& tilePos);                     // ����ָ��λ�õ�����

    bool canWater(const Vec2& tilePos) const;               // ���ָ��λ���Ƿ���Խ�ˮ
    bool waterSoil(const Vec2& tilePos);                    // ��ˮָ��λ�õ�����

    bool canHarvest(const Vec2& tilePos) const;             // ���ָ��λ���Ƿ�����ջ�
    bool harvestCrop(const Vec2& tilePos);                  // �ջ�ָ��λ�õ�����

    bool resourceCanRemove(const Vec2& tilePos) const;      // ���ָ��λ����Դ�Ƿ�����Ƴ�
    bool removeResource(const Vec2& tilePos);               // �Ƴ�ָ��λ�õ���Դ

    void setGameMap(GameMap* map)                           // ���õ�ǰ��Ϸ��ͼ����
    {
        // ����Ѿ��м����������Ƴ�
        if (_keyboardListener)
        {
            Director::getInstance()->getEventDispatcher()->removeEventListener(_keyboardListener);
            _keyboardListener = nullptr;
        }

        _gameMap = map;
        initKeyboardListener();
    }
    void onMouseDown(const cocos2d::Vec2& mousePos, Player* player);
    void setGameScene(GameScene* scene) { _gameScene = scene; } 

    // ������ֲ��ط���
    bool canPlant(const Vec2& tilePos) const;               // ���ָ��λ���Ƿ������ֲ
    bool plantCorn(const Vec2& tilePos);                    // ��ָ��λ����ֲ����
    void initKeyboardListener();                            // ��ʼ�����̼���

    // ����ͼ������
    void saveCrops();
    void loadCrops();
    void clearCrops();                  // �л���ͼʱ�������ﾫ��

    const std::vector<int> RESOURCE_TILES = { 258,182,162 };    // �ɳ��ݻ���ʯ����Դ����ͼ��ID�б�
    const int RESOURCE_REMOVED_TILE_ID = 473;            // �Ƴ���Դ���ͼ��ID

private:
    friend class GameScene;

    CropManager() {}                            // ˽�й��캯��������ģʽ��              
    bool _isInitialized = false;                // ��ӳ�ʼ����־
    bool hasCropAt(const Vec2& tilePos) const;  // ���ָ��λ���Ƿ���������

    static CropManager* _instance;                              // ����ʵ��ָ��
    GameMap* _gameMap = nullptr;                                // ��ǰ��ͼ����
    GameScene* _gameScene = nullptr;                            // ��ӳ�������

    // ��ʾ
    mutable Label* tipLabel = nullptr;  // ���mutable�ؼ��֣�������const������ı�ֵ��
    void showTip(const std::string& text, const Vec2& tilePos, float duration = 1.5f) const;
    void hideTip() const;

    // ��ˮЧ����س���
    const Color3B WATER_COLOR = Color3B(173, 216, 230);     // ��ˮ�����ɫ(����ɫ)
    const float WATER_DURATION = 1.0f;                      // ��ˮЧ������ʱ��
    const float FADE_DURATION = 0.3f;                       // ��ɫ�ָ�����ʱ��

    // ����λ��ƫ�Ƴ���
    // ע������ƫ����������Ҳ��֪��Ϊʲô������������Գ������˵�ڲ����м��~
    const float CROP_OFFSET_X = 20.0f;     // ����ˮƽƫ����
    const float CROP_OFFSET_Y = -12.0f;    // ���ﴹֱƫ����
    cocos2d::EventListenerKeyboard* _keyboardListener = nullptr;
    // ���������׶ε�ͼ��ID
    const std::vector<cocos2d::Rect> CORN_GROWTH_RECTS = {
        cocos2d::Rect(0, 0, 16, 16),    // ����
        cocos2d::Rect(16, 0, 16, 16),   // �����׶�1
        cocos2d::Rect(32, 0, 16, 16),   // �����׶�2
        cocos2d::Rect(48, 0, 16, 16),   // �����׶�3
        cocos2d::Rect(64, 0, 16, 16)    // ����
    };
    // ������Ϣ�ṹ��
    struct CropInfo
    {
        Vec2 position;          // ��ֲλ��
        Vec2 tilePos;           // ��Ƭ����
        int growthStage;        // �����׶�
        std::string type;       // ��������("corn"��)
        int plantDay;           // ��ֲʱ����Ϸ����
        int plantMonth;         // ��ֲʱ����Ϸ�·�
        int plantYear;          // ��ֲʱ����Ϸ���
        int waterLevel = 2;     // ˮ��״̬: 2-����, 1-��ȱ, 0-����ȱˮ
        bool isWatered = false; // �����Ƿ񽽹�ˮ
    };

    std::vector<CropInfo> _cropInfos;   // �洢����������Ϣ
    std::vector<Node*> _crops;          // ��ǰ��ʾ�����ﾫ��
    void updateCrops();                 // ��������״̬��ʵ����������GameScene����ã�

    void createHarvestDrop(const Vec2& position);   // �����ջ������
    const int FINAL_GROWTH_STAGE = 3;               // ���������׶�
    /*
     * ͼ��ID˵����
     * 1. Tiled�༭����ͼ��ID��0��ʼ��ʵ��ʹ��ʱ��Ҫ��1
     * 2. ��ΪTiled��0��ʾ��ͼ�飬����ʵ��ͼ��ID = Tiled�п��Ѻ�ID + 1
     * 3. ����ʹ�õĶ���ID��Ϊʵ��ʹ�õ�ID���Ѽ�1��
     */
    const std::vector<int> TILLABLE_TILES = { 207, 227, 228, 232, 464,473 };    // �ɿ��ѵ�ͼ��ID�б�
    const int TILLED_TILE_ID = 681;                                         // ���Ѻ��ͼ��ID���ѿ��ѵ�ũ��ͼ�飩

    void createWaterEffect(Sprite* tile);   // ������ˮЧ��
    void showWateringPopup();               // ��ʾ��ˮ��ʾ

}; 