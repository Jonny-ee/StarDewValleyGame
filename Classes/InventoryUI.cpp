#include "InventoryUI.h"
#include "ItemSystem.h"

USING_NS_CC;

/*
 * ��������UI����
 * ���ܣ���̬��������ʼ��һ���µı���UIʵ��
 * @return ���س�ʼ���ɹ��ı���UI����ָ�룬ʧ���򷵻�nullptr
 */
InventoryUI* InventoryUI::create()
{
    auto* pRet = new(std::nothrow) InventoryUI();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

/*
 * ��ʼ������UI
 * ���ܣ���ʼ�������Ļ�������
 * @return ��ʼ���ɹ�����true��ʧ�ܷ���false
 */
bool InventoryUI::init()
{
    if (!Node::init())
        return false;

    this->setVisible(false);    // ��ʼʱ�ǹرյ�
    createUI();                 // ����UI����
    return true;
}

/*
 * ��������UI����
 * ���ܣ�
 * 1.������͸������
 * 2.�����������
 * 3.������Ʒ�б�
 * 4.������Ʒ��ʾ��ʽ
 */
void InventoryUI::createUI()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // ������͸����ɫ����
    _bgLayer = LayerColor::create(Color4B(0, 0, 0, 128), visibleSize.width, visibleSize.height);
    _bgLayer->setPosition(Vec2(-this->getPosition().x, -this->getPosition().y));
    this->addChild(_bgLayer);

    // ��Ʒ�б�
    std::vector<std::string> items = { "wood", "apple", "corn", "bread", "tomato", "fish", "mermaid's KISS(*)", "stone" };
    
    // ���㲼�ֲ���
    float itemHeight = 45;  // ��Сÿ����Ʒ�еĸ߶�
    float totalContentHeight = items.size() * itemHeight;  // �������ݵ��ܸ߶�
    float panelHeight = totalContentHeight + 40;  // ��С���±߾�(��80��Ϊ40)
    float panelWidth = 300;  // �����
    
    // ����������屳��
    auto panelBg = LayerColor::create(Color4B(255, 255, 255, 230), panelWidth, panelHeight);
    panelBg->setPosition(
        visibleSize.width / 2 - panelWidth / 2,     // ˮƽ����
        visibleSize.height / 2 - panelHeight / 2    // ��ֱ����
    );
    _bgLayer->addChild(panelBg);

    // �����һ����Ʒ����ʼY���꣨����嶥������20���ؿ�ʼ��
    float startY = visibleSize.height / 2 + panelHeight / 2 - 40;  // ��С�����߾�
    
    // Ϊÿ����Ʒ������ʾ��
    for (const auto& itemId : items)
    {
        // ������Ʒ�б���
        auto itemBg = LayerColor::create(Color4B(255, 255, 255, 255), panelWidth - 20, 40);
        itemBg->setPosition(
            visibleSize.width / 2 - panelWidth / 2 + 10,
            startY - 20
        );
        _bgLayer->addChild(itemBg);

        // ������Ʒ���Ʊ�ǩ
        auto nameLabel = Label::createWithSystemFont(itemId, "Arial", 24);
        nameLabel->setPosition(Vec2(visibleSize.width / 2 - panelWidth / 2 + 30, startY));
        nameLabel->setAnchorPoint(Vec2(0, 0.5f));
        nameLabel->setTextColor(Color4B::BLACK);
        _bgLayer->addChild(nameLabel);

        // ������Ʒ������ǩ
        auto countLabel = Label::createWithSystemFont("0", "Arial", 24);
        countLabel->setPosition(Vec2(visibleSize.width / 2 + panelWidth / 2 - 30, startY));
        countLabel->setAnchorPoint(Vec2(1, 0.5f));
        countLabel->setTextColor(Color4B::BLACK);
        _bgLayer->addChild(countLabel);

        _countLabels[itemId] = countLabel;
        startY -= itemHeight;  // ʹ���µ��и�
    }
}

/*
 * �л�������ʾ״̬
 * ���ܣ���ʾ/���ر�������
 */
void InventoryUI::toggleVisibility()
{
    this->setVisible(!this->isVisible());
}

/*
 * ������ʾ
 * ���ܣ�����������Ʒ����ʾ����
 */
void InventoryUI::updateDisplay()
{
    auto itemSystem = ItemSystem::getInstance();
    for (const auto& pair : _countLabels)
    {
        const std::string& itemId = pair.first;
        auto label = pair.second;
        int count = itemSystem->getItemCount(itemId);
        label->setString(std::to_string(count));
    }
}

/*
 * �����Ʒ
 * ���ܣ��򱳰������ָ��������Ʒ
 * @param itemId ��ƷID
 * @param count �������
 */
void InventoryUI::addItem(const std::string& itemId, int count)
{
    auto itemSystem = ItemSystem::getInstance();
    itemSystem->addItem(itemId, count);
    updateDisplay();
}

/*
 * �Ƴ���Ʒ
 * ���ܣ��ӱ������Ƴ�ָ��������Ʒ
 * @param itemId ��ƷID
 * @param count �Ƴ�����
 * @return �Ƴ��ɹ�����true��ʧ�ܷ���false
 */
bool InventoryUI::removeItem(const std::string& itemId, int count)
{
    auto itemSystem = ItemSystem::getInstance();
    bool success = itemSystem->removeItem(itemId, count);
    if (success)
    {
        updateDisplay();
    }
    return success;
}

/*
 * ��ȡ��Ʒ����
 * ���ܣ���ȡ������ָ����Ʒ�ĵ�ǰ����
 * @param itemId ��ƷID
 * @return ������Ʒ�����������Ʒ�����ڷ���0
 */
int InventoryUI::getItemCount(const std::string& itemId) const
{
    return ItemSystem::getInstance()->getItemCount(itemId);
}