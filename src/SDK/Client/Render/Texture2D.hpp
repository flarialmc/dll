#pragma once

class TextureData
{
private:
	char pad_0x0[0x18];
	struct PathStruct
	{
		char pad_0x0[0x18];
		TextHolder filePath;
	}*ptrToPath;
	char pad_0x0038[0x20];

public:
	TextHolder GetFilePath() const
	{
		return ptrToPath->filePath;
	}
};

class Texture2D {
private:
	char pad_0x0[0x18];
	TextureData* data;

public:
	TextureData* GetData() const
	{
		return data;
	}
};

class FontBitmap
{
private:
	char pad_0x0000[0x290];
public:
	char FontPath[32];
};

class StringHasher {
public:
	uint64_t hash;
	TextHolder text;
	const StringHasher* unk;

	StringHasher(uint64_t inputhash, std::string inputtext) {
		memset(this, 0x0, sizeof(StringHasher));
		this->hash = inputhash;
		this->text.setText(inputtext);
	}

	StringHasher(const std::string& text) {
		memset(this, 0x0, sizeof(StringHasher));
		this->text.setText(text);
		this->computeHash();
	}

	void computeHash() {
		hash = 0xCBF29CE484222325i64;
		if (this->text.getTextLength() <= 0)
			return;
		char* textP = this->text.getText();
		auto c = *textP;

		do {
			hash = c ^ 0x100000001B3i64 * hash;
			c = *++textP;
		} while (*textP);
	}

	bool operator==(StringHasher& rhs) {
		if (this->text.getText() == rhs.text.getText()) {
			return ((this->unk == &rhs) && (rhs.unk == this));
		}
		return false;
	}
	bool operator!=(StringHasher& rhs) {
		return !(*this == rhs);
	}
	bool operator<(StringHasher& rhs) {
		if (this->hash < rhs.hash) {
			return true;
		}
		if (this->hash <= rhs.hash) {
			return (strcmp(this->text.getText(), rhs.text.getText()) < 0);
		}
		return false;
	}
};

class HashedString {
public:
	uint64_t hash;
	TextHolder text;
	const HashedString* unk;

	HashedString(uint64_t inputhash, std::string inputtext) {
		memset(this, 0x0, sizeof(HashedString));
		this->hash = inputhash;
		this->text.setText(inputtext);
	}

	HashedString(const std::string& text) {
		memset(this, 0x0, sizeof(HashedString));
		this->text.setText(text);
		this->computeHash();
	}

	void computeHash() {
		hash = 0xCBF29CE484222325i64;
		if (this->text.getTextLength() <= 0)
			return;
		char* textP = this->text.getText();
		auto c = *textP;

		do {
			hash = c ^ 0x100000001B3i64 * hash;
			c = *++textP;
		} while (*textP);
	}

	bool operator==(HashedString& rhs) {
		if (this->text.getText() == rhs.text.getText()) {
			return ((this->unk == &rhs) && (rhs.unk == this));
		}
		return false;
	}
	bool operator!=(HashedString& rhs) {
		return !(*this == rhs);
	}
	bool operator<(HashedString& rhs) {
		if (this->hash < rhs.hash) {
			return true;
		}
		if (this->hash <= rhs.hash) {
			return (strcmp(this->text.getText(), rhs.text.getText()) < 0);
		}
		return false;
	}
};