#include "StdAfx.h"
#include "UnicodeConv.h"

#include <vector>

namespace
{
	void SwapByte(char* buf, size_t len)
	{
		for (char *p = buf, *e = buf + len; p < e; p +=2)
		{
			char t = p[0];
			p[0] = p[1];
			p[1] = t;
		}
	}
}

std::string ConvertUnicodeToMultiBytes(const std::wstring& wstr, UINT cp)
{
	if (wstr.empty())
		return std::string();

	if (cp == CP_UNICODE_BE)
	{
		size_t len = wstr.size() * sizeof(wchar_t);
		const char* buf_w = (const char*)wstr.c_str();
		std::vector<char> buf(buf_w, buf_w + len);
		SwapByte(&buf.front(), len);
		return std::string(buf.begin(), buf.end());
	}
	else if (cp == CP_UNICODE)
	{
		size_t len = wstr.size() * sizeof(wchar_t);
		const char* buf_w = (const char*)wstr.c_str();
		return std::string(buf_w, buf_w + len);
	}

	const wchar_t* u_str = wstr.c_str();
	int u_len = (int)wstr.size();

    int a_len = WideCharToMultiByte(cp, 0, u_str, u_len, NULL, 0, NULL, NULL);
	if (a_len == 0)
		return std::string();

	std::vector<char> astr(a_len);
    char* a_str = &astr.front();

    WideCharToMultiByte(cp, 0, u_str, u_len, a_str, a_len, NULL, NULL);

	return std::string(astr.begin(), astr.end());
}

std::wstring ConvertMultiBytesToUnicode(const std::string& astr, UINT cp)
{
	if (astr.empty())
		return std::wstring();

	if (cp == CP_UNICODE_BE)
	{
		size_t len = astr.size() / sizeof(wchar_t);
		std::vector<wchar_t> buf(len);
		char* buf_w = (char*)&buf.front();
		memcpy(buf_w, astr.c_str(), len * sizeof(wchar_t));
		SwapByte(buf_w, len * sizeof(wchar_t));
		return std::wstring(buf.begin(), buf.end());
	}
	else if (cp == CP_UNICODE)
	{
		size_t len = astr.size() / sizeof(wchar_t);
		std::vector<wchar_t> buf(len);
		char* buf_w = (char*)&buf.front();
		memcpy(buf_w, astr.c_str(), len * sizeof(wchar_t));
		return std::wstring(buf.begin(), buf.end());
	}

	const char* a_str = astr.c_str();
	int a_len = (int)astr.size();

	int u_len = MultiByteToWideChar(cp, 0, a_str, a_len, NULL, 0);
	if (u_len == 0)
		return std::wstring();

	std::vector<wchar_t> wstr(u_len);
    wchar_t* u_str = &wstr.front();

	MultiByteToWideChar(cp, 0, a_str, a_len, u_str, u_len);

	return std::wstring(wstr.begin(), wstr.end());
}

std::string ConvertUtf8ToMultiBytes( const std::string &utf8Str )
{
	std::wstring szUnicode = ConvertMultiBytesToUnicode(utf8Str, CP_UTF8);
	return ConvertUnicodeToMultiBytes(szUnicode);
}

std::string ConvertMultiBytesToUtf8( const std::string &ansiStr )
{
	std::wstring szUnicode = ConvertMultiBytesToUnicode(ansiStr);
	return ConvertUnicodeToMultiBytes(szUnicode, CP_UTF8);
}