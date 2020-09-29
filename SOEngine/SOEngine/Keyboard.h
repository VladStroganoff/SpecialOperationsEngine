#pragma once
#include <queue>
#include <bitset>


class Keyboard
{
	friend class Window;
public:
	class Event 
	{
	public:
		enum class Type
		{
			Press,
			Release,
			Invalid
		};
	private:
		Type type;
		unsigned char code;
	public:
		Event() : type(Type::Invalid), code(0u) 
		{}
		Event(Type _type, unsigned char _code) noexcept :
			type(_type),
			code(_code)
		{}
		bool IsPressed() const noexcept
		{
			return type == Type::Press;
		}
		bool IsRealised() const noexcept
		{
			return type == Type::Release;
		}
		bool IsValid() const noexcept
		{
			return type != Type::Invalid;
		}
		unsigned char GetCode() const noexcept
		{
			return code;
		}
	};
	
public:
	Keyboard() = default;
	Keyboard(const Keyboard&) = delete;
	Keyboard& operator = (const Keyboard&) = delete;

	bool KeyIsPressed(unsigned char keycode) const noexcept;
	Event ReadKey() noexcept;
	bool KeyIsEmpty() const noexcept;
	void FlushKey() noexcept;

	char ReadChar() noexcept;
	bool CharIsEmpty() const noexcept;
	void FlushChar() noexcept;
	void Flush() noexcept;

	void EnableAutorepeat() noexcept;
	void DisableAutoRepeat() noexcept;
	bool AutoreapeatIsEnabled() noexcept;
private: // Window private stuff
	void OnKeyIsPressed(unsigned char keycode) noexcept;
	void OnKeyRealised(unsigned char keycode) noexcept;
	void OnChar(char character) noexcept;
	void ClearState() noexcept;
	template<typename T>
	static void TrimBuffer(std::queue<T>& buffer) noexcept;
private:// private variables
	static constexpr unsigned int nKeys = 256u;
	static constexpr unsigned int bufferSize = 16u;
	bool autorepeatEnabled = false;
	std::bitset<nKeys> keystates;
	std::queue<Event> keybuffer;
	std::queue<char> charbuffer;


};
