#include "InventoryUI.h"

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
 * ���ܣ���ʼ����������Ļ�������
 * @return ��ʼ���ɹ�����true��ʧ�ܷ���false
 */
bool InventoryUI::init()
{
    if (!Node::init())
        return false;

    this->setVisible(false);    // ��ʼʱ���ر���
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

    // ����������屳��
    auto panelBg = LayerColor::create(Color4B(255, 255, 255, 230), 300, 400);
    panelBg->setPosition(
        visibleSize.width / 2 - 150,    // ������ʾ
        visibleSize.height / 2 - 200
    );
    _bgLayer->addChild(panelBg);

    // ������Ʒ�б�
    std::vector<std::string> items = { "wood", "apple", "corn", "bread", "tomato", "fish", "stone" };
    float y = visibleSize.height / 2 + 150;  // ��ʼ������

    // Ϊÿ����Ʒ������ʾ��
    for (const auto& itemId : items)
    {
        // ������Ʒ�б���
        auto itemBg = LayerColor::create(Color4B(255, 255, 255, 255), 280, 40);
        itemBg->setPosition(
            visibleSize.width / 2 - 140,
            y - 20
        );
        _bgLayer->addChild(itemBg);

        // ������Ʒ���Ʊ�ǩ
        auto nameLabel = Label::createWithSystemFont(itemId, "Arial", 24);
        nameLabel->setPosition(Vec2(visibleSize.width / 2 - 120, y));
        nameLabel->setAnchorPoint(Vec2(0, 0.5f));
        nameLabel->setTextColor(Color4B::BLACK);
        _bgLayer->addChild(nameLabel);

        // ������Ʒ������ǩ
        auto countLabel = Label::createWithSystemFont("0", "Arial", 24);
        countLabel->setPosition(Vec2(visibleSize.width / 2 + 120, y));
        countLabel->setAnchorPoint(Vec2(1, 0.5f));
        countLabel->setTextColor(Color4B::BLACK);
        _bgLayer->addChild(countLabel);

        _countLabels[itemId] = countLabel;
        y -= 50;  // ��һ�е�λ��
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
 * �����Ʒ
 * ���ܣ��򱳰������ָ����������Ʒ
 * @param itemId ��ƷID
 * @param count �������
 */
void InventoryUI::addItem(const std::string& itemId, int count)
{
    _items[itemId] += count;
    if (_countLabels.find(itemId) != _countLabels.end())
    {
        _countLabels[itemId]->setString(std::to_string(_items[itemId]));
    }
}

/*
 * �Ƴ���Ʒ
 * ���ܣ��ӱ������Ƴ�ָ����������Ʒ
 * @param itemId ��ƷID
 * @param count �Ƴ�����
 * @return �Ƴ��ɹ�����true��ʧ�ܷ���false
 */
bool InventoryUI::removeItem(const std::string& itemId, int count)
{
    if (_items.find(itemId) != _items.end() && _items[itemId] >= count)
    {
        _items[itemId] -= count;
        _countLabels[itemId]->setString(std::to_string(_items[itemId]));
        if (_items[itemId] == 0)
        {
            _items.erase(itemId);
        }
        return true;
    }
    return false;
}

/*
 * ��ȡ��Ʒ����
 * ���ܣ���ȡ������ָ����Ʒ������
 * @param itemId ��ƷID
 * @return ������Ʒ�����������Ʒ�����ڷ���0
 */
int InventoryUI::getItemCount(const std::string& itemId) const
{
    auto it = _items.find(itemId);
    return (it != _items.end()) ? it->second : 0;
}