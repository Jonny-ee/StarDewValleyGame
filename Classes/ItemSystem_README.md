# ItemSystem 物品系统

## 简介
这是一个简单的物品管理系统，用于管理游戏中物品的增删改查操作。

## 文件说明
- `ItemSystem.h`   - 物品系统头文件
- `ItemSystem.cpp` - 物品系统实现文件

## 使用方法

### 1. 添加文件
将 ItemSystem.h 和 ItemSystem.cpp 添加到项目中。

### 2. 基本使用
	// 获取物品系统实例
	auto itemSystem = ItemSystem::getInstance();
注：关于获取实例，建议：
1.在该类中添加私有变量（TemSystem* itemSystem）；
2.在该类的构造函数中直接加入：itemSystem = ItemSystem::getInstance()；
3.后续所有的使用只用使用该实例即可（具体实现可见5）

### 3. 当前的可用物品ID
	"wood"	 木头
	"apple"  苹果
	"corn"	 谷子
	"bread"	 面包（食谱 消耗几个谷子换来的）
	"tomato" 番茄
	"fish"     鱼
	"stone"  石头

### 4. 功能使用示例（需先添加实例itemSystem）
#### 4.1 添加物品
	// 添加5个木头
	bool success = itemSystem->addItem("wood", 5);
#### 4.2 移除物品
	// 移除2个木头
	bool success = itemSystem->removeItem("wood", 2);
#### 4.3 获取物品数量
	// 获取木头数量
	int woodCount = itemSystem->getItemCount("wood");
#### 4.4 使用物品（这个应该没啥用，还是写了个...）
	// （只能）使用一个物品（不带数量参数）
	bool success = itemSystem->useItem("wood");
#### 4.5 检查物品是否足够
	// 检查是否有足够数量的物品
	// 有3个或更多木头
	if (itemSystem->hasEnoughItems("wood", 3))
	{
		...
	}

### 5. 使用示例
	// 在某个类中使用
	class SomeClass
	{
	private:
		ItemSystem* itemSystem;

	public:
		SomeClass()
		{
			itemSystem = ItemSystem::getInstance();
			...
		}

		void someFunction()
		{
			// 添加物品
            itemSystem->addItem("wood", 5)

			// 获取数量
			int woodCount = itemSystem->getItemCount("wood");

			// 使用物品
			if (itemSystem->hasEnoughItems("wood", 3))
			{
				itemSystem->removeItem("wood", 3);
				...
			}
		}
	};