# ItemSystem ��Ʒϵͳ

## ���
����һ���򵥵���Ʒ����ϵͳ�����ڹ�����Ϸ����Ʒ����ɾ�Ĳ������

## �ļ�˵��
- `ItemSystem.h`   - ��Ʒϵͳͷ�ļ�
- `ItemSystem.cpp` - ��Ʒϵͳʵ���ļ�

## ʹ�÷���

### 1. ����ļ�
�� ItemSystem.h �� ItemSystem.cpp ��ӵ���Ŀ�С�

### 2. ����ʹ��
	// ��ȡ��Ʒϵͳʵ��
	auto itemSystem = ItemSystem::getInstance();
ע�����ڻ�ȡʵ�������飺
1.�ڸ��������˽�б�����TemSystem* itemSystem����
2.�ڸ���Ĺ��캯����ֱ�Ӽ��룺itemSystem = ItemSystem::getInstance()��
3.�������е�ʹ��ֻ��ʹ�ø�ʵ�����ɣ�����ʵ�ֿɼ�5��

### 3. ��ǰ�Ŀ�����ƷID
	"wood"	 ľͷ
	"apple"  ƻ��
	"corn"	 ����
	"bread"	 �����ʳ�� ���ļ������ӻ����ģ�
	"tomato" ����
	"fish"     ��
	"stone"  ʯͷ

### 4. ����ʹ��ʾ�����������ʵ��itemSystem��
#### 4.1 �����Ʒ
	// ���5��ľͷ
	bool success = itemSystem->addItem("wood", 5);
#### 4.2 �Ƴ���Ʒ
	// �Ƴ�2��ľͷ
	bool success = itemSystem->removeItem("wood", 2);
#### 4.3 ��ȡ��Ʒ����
	// ��ȡľͷ����
	int woodCount = itemSystem->getItemCount("wood");
#### 4.4 ʹ����Ʒ�����Ӧ��ûɶ�ã�����д�˸�...��
	// ��ֻ�ܣ�ʹ��һ����Ʒ����������������
	bool success = itemSystem->useItem("wood");
#### 4.5 �����Ʒ�Ƿ��㹻
	// ����Ƿ����㹻��������Ʒ
	// ��3�������ľͷ
	if (itemSystem->hasEnoughItems("wood", 3))
	{
		...
	}

### 5. ʹ��ʾ��
	// ��ĳ������ʹ��
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
			// �����Ʒ
            itemSystem->addItem("wood", 5)

			// ��ȡ����
			int woodCount = itemSystem->getItemCount("wood");

			// ʹ����Ʒ
			if (itemSystem->hasEnoughItems("wood", 3))
			{
				itemSystem->removeItem("wood", 3);
				...
			}
		}
	};