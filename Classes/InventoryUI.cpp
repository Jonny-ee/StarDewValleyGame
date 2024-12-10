#include "InventoryUI.h"

USING_NS_CC;

/*
 * 创建背包UI对象
 * 功能：动态创建并初始化一个新的背包UI实例
 * @return 返回初始化成功的背包UI对象指针，失败则返回nullptr
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
 * 初始化背包UI
 * 功能：初始化背包界面的基本设置
 * @return 初始化成功返回true，失败返回false
 */
bool InventoryUI::init()
{
    if (!Node::init())
        return false;

    this->setVisible(false);    // 初始时隐藏背包
    createUI();                 // 创建UI界面
    return true;
}

/*
 * 创建背包UI界面
 * 功能：
 * 1.创建半透明背景
 * 2.创建背包面板
 * 3.创建物品列表
 * 4.设置物品显示样式
 */
void InventoryUI::createUI()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // 创建半透明黑色背景
    _bgLayer = LayerColor::create(Color4B(0, 0, 0, 128), visibleSize.width, visibleSize.height);
    _bgLayer->setPosition(Vec2(-this->getPosition().x, -this->getPosition().y));
    this->addChild(_bgLayer);

    // 创建背包面板背景
    auto panelBg = LayerColor::create(Color4B(255, 255, 255, 230), 300, 400);
    panelBg->setPosition(
        visibleSize.width / 2 - 150,    // 居中显示
        visibleSize.height / 2 - 200
    );
    _bgLayer->addChild(panelBg);

    // 创建物品列表
    std::vector<std::string> items = { "wood", "apple", "corn", "bread", "tomato", "fish", "stone" };
    float y = visibleSize.height / 2 + 150;  // 起始纵坐标

    // 为每个物品创建显示行
    for (const auto& itemId : items)
    {
        // 创建物品行背景
        auto itemBg = LayerColor::create(Color4B(255, 255, 255, 255), 280, 40);
        itemBg->setPosition(
            visibleSize.width / 2 - 140,
            y - 20
        );
        _bgLayer->addChild(itemBg);

        // 创建物品名称标签
        auto nameLabel = Label::createWithSystemFont(itemId, "Arial", 24);
        nameLabel->setPosition(Vec2(visibleSize.width / 2 - 120, y));
        nameLabel->setAnchorPoint(Vec2(0, 0.5f));
        nameLabel->setTextColor(Color4B::BLACK);
        _bgLayer->addChild(nameLabel);

        // 创建物品数量标签
        auto countLabel = Label::createWithSystemFont("0", "Arial", 24);
        countLabel->setPosition(Vec2(visibleSize.width / 2 + 120, y));
        countLabel->setAnchorPoint(Vec2(1, 0.5f));
        countLabel->setTextColor(Color4B::BLACK);
        _bgLayer->addChild(countLabel);

        _countLabels[itemId] = countLabel;
        y -= 50;  // 下一行的位置
    }
}

/*
 * 切换背包显示状态
 * 功能：显示/隐藏背包界面
 */
void InventoryUI::toggleVisibility()
{
    this->setVisible(!this->isVisible());
}

/*
 * 添加物品
 * 功能：向背包中添加指定数量的物品
 * @param itemId 物品ID
 * @param count 添加数量
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
 * 移除物品
 * 功能：从背包中移除指定数量的物品
 * @param itemId 物品ID
 * @param count 移除数量
 * @return 移除成功返回true，失败返回false
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
 * 获取物品数量
 * 功能：获取背包中指定物品的数量
 * @param itemId 物品ID
 * @return 返回物品数量，如果物品不存在返回0
 */
int InventoryUI::getItemCount(const std::string& itemId) const
{
    auto it = _items.find(itemId);
    return (it != _items.end()) ? it->second : 0;
}