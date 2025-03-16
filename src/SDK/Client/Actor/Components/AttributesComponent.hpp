//#include "../../../Utils/Memory/Memory.hpp"
#include "../../Core/HashedString.hpp"
#include "../EntityContext.hpp"

class AttributeInstance{
public:
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
	char pad[0x20];

public:

	AttributeInstance* getInstance(unsigned int id) {
		using func_t = AttributeInstance & (*)(BaseAttributeMap*, unsigned int);
		static auto func = reinterpret_cast<func_t>(Memory::offsetFromSig(GET_SIG_ADDRESS("BaseAttributeMap_getInstance"), 1));
		return &func(this, id);
	}
};

struct AttributesComponent : IEntityComponent{
	BaseAttributeMap baseAttributes{};
};
static_assert(sizeof(AttributesComponent) == 96);