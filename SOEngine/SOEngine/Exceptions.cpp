#include "Exceptions.h"
#include <sstream>

GreatException::GreatException(int line, const char* file) noexcept
	:
	line(line),
	file(file)
{}

const char* GreatException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}


const char* GreatException::GetType() const noexcept
{
	return "Classic Exception: ";
}


int GreatException::GetLine() const noexcept
{ 
	return line;
}

const std::string& GreatException::GetFile() const noexcept
{
	return file;
}

std::string GreatException::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << "[File] " << file << std::endl
		<< "[Line] " << line;
	return oss.str();
}
