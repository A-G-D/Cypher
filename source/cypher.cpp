/*
*	<cypher.cpp>
*
*
*	Copyright (C) 2020 Aloever Dulay
*
*	This program is free software: you can redistribute it and/or modify it under the terms
*	of the GNU General Public License as published by the Free Software Foundation, version 3.
*
*	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
*	without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*	See the GNU General Public License for more details.
*
*	You should have received a copy of the GNU General Public License along with this program.
*	If not, see <https://www.gnu.org/licenses/>.
*
*/
#include "cypher.hpp"
#include <unordered_map>

using namespace std;

#define table unordered_map
#define LETTER_SEPARATOR ' '

table<wstring, wchar_t> alpha_table;
table<wchar_t, wstring> translation_table;
table<wchar_t, bool> translation_flag_table;

wchar_t letterSeparator = 0;
unsigned short fixedLength = 0;

bool IsWhitespace(wchar_t c)
{
	return c == (wchar_t)' ' || c == (wchar_t)'\t';
}
bool IsTranslationChar(wchar_t c)
{
	return translation_flag_table.find(c) != translation_flag_table.end();
}
int *LengthStorage()
{
	static int length[0xFFFF];
	return length;
}
void InitLengths(int length[])
{
	for (int i(0xFFFF); i--; length[i] = Cypher::Translate((wchar_t)i).size());
}
void ClearLengths(int length[])
{
	for (int i(0xFFFF); i--; length[i] = 0);
}
bool LoadFromString(wstring const &text, table<wchar_t, wstring> &tb_translation, table<wstring, wchar_t> &tb_alpha, table<wchar_t, bool> &tb_translation_flag, int lengths[], wchar_t &separator, unsigned short &length)
{
	wstring str(L"");
	short phase(0);
	bool lengthFixed(false);
	/*
	*	Look for the user-definition of the letter separator character
	*/
	for (unsigned int i(0); i < text.size(); ++i)
	{
		switch (phase)
		{
			case 0:
				if (text[i] == (wchar_t)'#')
					phase = 1;
				break;

			case 1:
				if (text[i] == (wchar_t)'\n')
					phase = 0;
				else
				{
					str += text[i];
					if (str == L"LETTER_SEPARATOR")
					{
						phase = 2;
						str = L"";
					}
					else if (str == L"TRANSLATION_LENGTH")
					{
						lengthFixed = true;
						phase = 2;
						str = L"";
					}
				}
				break;

			case 2:
				if (text[i] == (wchar_t)'=')
				{
					phase = (lengthFixed) ? 5 : 3;
				}
				else if (text[i] != (wchar_t)' ' && text[i] != (wchar_t)'\t')
					phase = 0;
				break;

			case 3:
				if (text[i] == (wchar_t)'\'')
					phase = 4;
				else if (text[i] != (wchar_t)' ' && text[i] != (wchar_t)'\t')
					phase = 0;
				break;

			case 4:
				if ((i + 1) < text.size() && text[i + 1] == (wchar_t)'\'')
					separator = text[i];
				phase = 0;
				break;

			case 5:
				if (text[i] == (wchar_t)' ' || text[i] == (wchar_t)'\t')
				{
					if ((i + 1) < text.size() && text[i + 1] != (wchar_t)' ' && text[i + 1] != (wchar_t)'\t' && text[i + 1] != (wchar_t)'\n')
						phase = 6;
				}
				else if (text[i] == (wchar_t)'\n')
					phase = 0;
				break;

			case 6:
				if (text[i] == (wchar_t)' ' || text[i] == (wchar_t)'\t' || text[i] == (wchar_t)'\n')
				{
					length = stoi(str);
					str = L"";
					phase = 0;
				}
				else
					str += text[i];
				break;
		}
	}

	if (separator == 0 && !lengthFixed)
		separator = LETTER_SEPARATOR;

	phase = 0;
	str = L"";
	/*
	*	Load the translation data from the translation file
	*/
	wchar_t c(0);
	for (unsigned int i(0); i < text.size(); ++i)
	{
		switch (phase)
		{
			/*
			*	Phase 1: Look for the '@' marker
			*/
			case 0:
				if (text[i] == (wchar_t)'@')
					phase = 1;
				break;
			/*
			*	Phase 2: Save the character next to the '@' marker
			*/
			case 1:
				c = text[i];
				phase = 2;
				break;
			/*
			*	Phase 3: Save the translation string found inside the '{' and '}' delimiters
			*/
			case 2:
				if (text[i] == (wchar_t)'{')
					phase = 3;
				else if (text[i] == (wchar_t)'@')
					phase = 1;
				else if (!IsWhitespace(text[i]))
					phase = 0;
				break;

			case 3:
				if (text[i] == (wchar_t)'}')
				{
					if (&tb_translation != nullptr)
						tb_translation[c] = str;
					if (str != L"")
					{
						tb_alpha[str] = c;
						str = L"";
					}
					c = 0;
					phase = 0;
				}
				else if (text[i] == separator)
				{
					//MessageBox::Show(L"Separator Character '" + text[i] + L"' is also used as a translation character.", L"Translation File Error", MessageBoxButtons::OK);
					Cypher::Clear();
					return false;
				}
				else
				{
					str += text[i];
					if (&tb_translation_flag != nullptr)
						tb_translation_flag[text[i]] = true;
				}
				break;
		}
	}
	if (lengths != nullptr)
		InitLengths(lengths);
	return true;
}
bool Cypher::IsEqual(wstring const &str_a, wstring const &str_b)
{
	table<wstring, wchar_t>
		tb_alpha_a,
		tb_alpha_b;
	wchar_t
		separator_a(0),
		separator_b(0);
	unsigned short
		fixed_length_a(0),
		fixed_length_b(0);

	::LoadFromString(str_a, *(table<wchar_t, wstring>*)nullptr, tb_alpha_a, *(table<wchar_t, bool>*)nullptr, nullptr, separator_a, fixed_length_a);
	::LoadFromString(str_b, *(table<wchar_t, wstring>*)nullptr, tb_alpha_b, *(table<wchar_t, bool>*)nullptr, nullptr, separator_b, fixed_length_b);

	if ((separator_a != separator_b) || (tb_alpha_a.size() != tb_alpha_b.size()))
		return false;

	for (auto node(tb_alpha_a.begin()); node != tb_alpha_a.end(); ++node)
		if ((tb_alpha_b.find(node->first) == tb_alpha_b.end()) || (tb_alpha_a.at(node->first) != tb_alpha_b.at(node->first)))
			return false;
	
	return true;
}
void Pair(wchar_t c, wstring const &str)
{
	translation_table[c] = str;
	alpha_table[str] = c;
	if (c != (wchar_t)'{' && c != (wchar_t)' ')
		translation_flag_table[c] = true;
}
wchar_t Cypher::LetterSeparator()
{
	return letterSeparator;
}
unsigned short Cypher::FixedLength()
{
	return fixedLength;
}
wchar_t Cypher::ToAlpha(wstring const &key)
{
	return (alpha_table.find(key) != alpha_table.end()) ? (wchar_t)(alpha_table[key]) : (wchar_t)'\0';
}
wstring Cypher::Translate(wchar_t c)
{
	return (translation_table.find(c) != translation_table.end()) ? (wstring&)(translation_table[c]) : L"";
}
void Cypher::Clear()
{
	alpha_table.clear();
	translation_table.clear();
	translation_flag_table.clear();
	ClearLengths(LengthStorage());
	letterSeparator = 0;
	fixedLength = 0;
}
bool Cypher::LoadFromString(wstring const &text)
{
	Clear();
	return ::LoadFromString(text, translation_table, alpha_table, translation_flag_table, LengthStorage(), letterSeparator, fixedLength);
}
void Cypher::LoadInternal()
{
	Clear();
	letterSeparator = 31;

	wstring str_translation;
	for (int i(0xFFFF); --i;)
	{
		str_translation = L"";
		str_translation += (wchar_t)((i - 0xA0 > 0) ? (i - 0xA0) : (0xFFFF - 0x9F + i));
		Pair((wchar_t)i, str_translation);
	}
}
int Cypher::GetTranslationLength(wchar_t c)
{
	return LengthStorage()[c];
}
wstring Cypher::ReadAllText(wstring const &text)
{
	wstring
		output(L""),
		letter(L"");
	wchar_t alpha;

	if (fixedLength)
	{
		unsigned short length(0);
		for (unsigned int i(0); i < text.size(); ++i)
		{
			letter += text[i];
			if (++length == fixedLength || text[i] == (wchar_t)'\n')
			{
				if (letter != L"")
				{
					alpha = ToAlpha(letter);
					if (alpha != (wchar_t)'\0')
						output += alpha;
				}
				if (text[i] == (wchar_t)'\n')
					output += ToAlpha(L"\n");
				letter = L"";
				length = 0;
			}
		}
	}
	else
	{
		for (unsigned int i(0); i < text.size(); ++i)
		{
			if (!IsTranslationChar(text[i]) || text[i] == letterSeparator || text[i] == (wchar_t)'\n')
			{
				if (letter != L"")
				{
					alpha = ToAlpha(letter);
					if (alpha != (wchar_t)'\0')
						output += alpha;
				}
				if (text[i] == (wchar_t)'\n')
					output += ToAlpha(L"\n");
				letter = L"";
			}
			else
				letter += text[i];
		}
	}

	if (letter != L"")
		output += ToAlpha(letter);

	return output;
}
wstring Cypher::TranslateAllText(wstring const &text)
{
	wstring
		output(L""),
		s;

	for (unsigned int i(0); i < text.size(); ++i)
	{
		s = Cypher::Translate(text[i]);
		if (s != L"")
		{
			if (output == L"" || output[output.size() - 1] == (wchar_t)'\n' || s == L"\n")
				output += (s == L"\n") ? L"\r\n" : s;
			else if (fixedLength)
				output += s;
			else
				(output += letterSeparator) += s;
		}
	}

	return output;
}