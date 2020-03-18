/*
*	<cypher.hpp>
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
#pragma once

#ifndef __CYPHER_HPP__
#define __CYPHER_HPP__

#ifdef CYPHER_EXPORTS
#define CYPHER_API __declspec(dllexport)
#else
#define CYPHER_API __declspec(dllimport)
#endif

#include <string>

namespace Cypher
{
	CYPHER_API wchar_t LetterSeparator();
	CYPHER_API unsigned short FixedLength();

	CYPHER_API wchar_t ToAlpha(std::wstring const &key);
	CYPHER_API std::wstring Translate(wchar_t value);

	CYPHER_API void Clear();

	CYPHER_API bool LoadFromString(std::wstring const &text);

	CYPHER_API void LoadInternal();

	CYPHER_API int GetTranslationLength(wchar_t c);

	CYPHER_API std::wstring ReadAllText(std::wstring const &text);

	CYPHER_API std::wstring TranslateAllText(std::wstring const &text);

	CYPHER_API bool IsEqual(std::wstring const &str_a, std::wstring const &str_b);
}
#endif

#pragma endregion