//#include "../../../Utils/Memory/Memory.hpp"
#include "../../Core/HashedString.hpp"
#include "../EntityContext.hpp"

class AttributeInstance{
public:
	// AttributeInstance::getCurrentValue
	float GetValue() {
		return hat::member_at<float>(this, GET_OFFSET("AttributeInstance::Value"));
	}
};

class Attribute {
public:
	int8_t mRedefinitionNode = 0;
	bool mSyncable = true;
	short unk = 0;
	int mIDValue;
	HashedString mName;

	Attribute(int id, std::string const& name) : mIDValue(id), mName(name) {}
};

class BaseAttributeMap {
public:
	std::unordered_map<unsigned int, AttributeInstance> instances;

private:
	char pad[0x18];

public:

	AttributeInstance* getInstance(unsigned int id) {
		using func_t = AttributeInstance & (*)(BaseAttributeMap*, unsigned int);
		static auto func = reinterpret_cast<func_t>(GET_SIG_ADDRESS("BaseAttributeMap_getInstance"));
		return &func(this, id);
	}
};

class AttributesComponent : public IEntityComponent{
public:
	BaseAttributeMap baseAttributes{};
};