#include "InventoryUI.h"
#include "ItemSystem.h"

USING_NS_CC;

/*
 * 创建背包UI对象
 * 功能：静态方法，初始化一个新的背包UI实例
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
 * 功能：初始化背包的基础布局
 * @return 初始化成功返回true，失败返回false
 */
bool InventoryUI::init()
{
    if (!Node::init())
        return false;

    this->setVisible(false);    // 初始时是关闭的
    createUI();                 // 创建UI布局
    return true;
}

/*
 * 创建背包UI布局
 * 功能：
 * 1.创建半透明背景
 * 2.创建背包面板
 * 3.创建物品列表
 * 4.设置物品显示格式
 */
void InventoryUI::createUI()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // 创建半透明黑色背景
    _bgLayer = LayerColor::create(Color4B(0, 0, 0, 128), visibleSize.width, visibleSize.height);
    _bgLayer->setPosition(Vec2(-this->getPosition().x, -this->getPosition().y));
    this->addChild(_bgLayer);

    // 物品列表
    std::vector<std::string> items = { "wood", "apple", "corn", "bread", "tomato", "fish", "mermaid's KISS(*)", "stone" };
    
    // 计算布局参数
    float itemHeight = 45;  // 减小每个物品行的高度
    float totalContentHeight = items.size() * itemHeight;  // 所有内容的总高度
    float panelHeight = totalContentHeight + 40;  // 减小上下边距(从80改为40)
    float panelWidth = 300;  // 面板宽度
    
    // 创建背包面板背景
    auto panelBg = LayerColor::create(Color4B(255, 255, 255, 230), panelWidth, panelHeight);
    panelBg->setPosition(
        visibleSize.width / 2 - panelWidth / 2,     // 水平居中
        visibleSize.height / 2 - panelHeight / 2    // 垂直居中
    );
    _bgLayer->addChild(panelBg);

    // 计算第一个物品的起始Y坐标（从面板顶部向下20像素开始）
    float startY = visibleSize.height / 2 + panelHeight / 2 - 40;  // 减小顶部边距
    
    // 为每个物品创建显示行
    for (const auto& itemId : items)
    {
        // 创建物品列表行
        auto itemBg = LayerColor::create(Color4B(255, 255, 255, 255), panelWidth - 20, 40);
        itemBg->setPosition(
            visibleSize.width / 2 - panelWidth / 2 + 10,
            startY - 20
        );
        _bgLayer->addChild(itemBg);

        // 创建物品名称标签
        auto nameLabel = Label::createWithSystemFont(itemId, "Arial", 24);
        nameLabel->setPosition(Vec2(visibleSize.width / 2 - panelWidth / 2 + 30, startY));
        nameLabel->setAnchorPoint(Vec2(0, 0.5f));
        nameLabel->setTextColor(Color4B::BLACK);
        _bgLayer->addChild(nameLabel);

        // 创建物品数量标签
        auto countLabel = Label::createWithSystemFont("0", "Arial", 24);
        countLabel->setPosition(Vec2(visibleSize.width / 2 + panelWidth / 2 - 30, startY));
        countLabel->setAnchorPoint(Vec2(1, 0.5f));
        countLabel->setTextColor(Color4B::BLACK);
        _bgLayer->addChild(countLabel);

        _countLabels[itemId] = countLabel;
        startY -= itemHeight;  // 使用新的行高
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
 * 更新显示
 * 功能：更新所有物品的显示数量
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
 * 添加物品
 * 功能：向背包中添加指定数量物品
 * @param itemId 物品ID
 * @param count 添加数量
 */
void InventoryUI::addItem(const std::string& itemId, int count)
{
    auto itemSystem = ItemSystem::getInstance();
    itemSystem->addItem(itemId, count);
    updateDisplay();
}

/*
 * 移除物品
 * 功能：从背包中移除指定数量物品
 * @param itemId 物品ID
 * @param count 移除数量
 * @return 移除成功返回true，失败返回false
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
 * 获取物品数量
 * 功能：获取背包中指定物品的当前数量
 * @param itemId 物品ID
 * @return 返回物品数量，如果物品不存在返回0
 */
int InventoryUI::getItemCount(const std::string& itemId) const
{
    return ItemSystem::getInstance()->getItemCount(itemId);
}