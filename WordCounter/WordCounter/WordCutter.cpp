#include "stdafx.h"
#include <iostream>
#include "WordCutter.h"

static regex PERFECTIVEGROUND = regex("((ив|ивши|ившись|ыв|ывши|ывшись)|([а€](в|вши|вшись)))$");
static regex PERFECTIVEGROUND_CUT = regex("((ив|ивши|ившись|ыв|ывши|ывшись)|(в|вши|вшись))$");
static regex REFLEXIVE = regex("(с[€ь])$");
static regex ADJECTIVE = regex("(ее|ие|ые|ое|ими|ыми|ей|ий|ый|ой|ем|им|ым|ом|его|ого|ему|ому|их|ых|ую|юю|а€|€€|ою|ею)$");
static regex VERB = regex("((ила|ыла|ена|ейте|уйте|ите|или|ыли|ей|уй|ил|ыл|им|ым|ен|ило|ыло|ено|€т|ует|уют|ит|ыт|ены|ить|ыть|ишь|ую|ю)|(([а€])(ла|на|ете|йте|ли|й|л|ем|н|ло|но|ет|ют|ны|ть|ешь|нно)))$");
static regex VERB_CUT = regex("((ила|ыла|ена|ейте|уйте|ите|или|ыли|ей|уй|ил|ыл|им|ым|ен|ило|ыло|ено|€т|ует|уют|ит|ыт|ены|ить|ыть|ишь|ую|ю)|(ла|на|ете|йте|ли|й|л|ем|н|ло|но|ет|ют|ны|ть|ешь|нно))$");
static regex NOUN = regex("(а|ев|ов|ие|ье|е|и€ми|€ми|ами|еи|ии|и|ией|ей|ой|ий|й|и€м|€м|ием|ем|ам|ом|о|у|ах|и€х|€х|ы|ь|ию|ью|ю|и€|ь€|€)$");
static regex RVRE = regex("[аеиоуыэю€]");
static regex DER = regex("ость?$");
static regex SUPERLATIVE = regex("(ейше|ейш)$");
static regex I = regex("и$");
static regex P = regex("ь$");
static regex NN = regex("нн$");
static regex N = regex("н$");


string LowerCase(string s)
{
	int dif = 'а' - 'ј';
	for (int i = 0; i<s.length(); i++)
	{
		if ((s[i] >= 'ј') && (s[i] <= 'я'))
			s[i] += dif;
	}
	return s;
}


bool CutEnding(string *s, regex pattern)
{
	bool a = regex_search(*s, pattern);
	if (a)
	{
		//концовка нашлась - режем ее
		*s = regex_replace(*s, pattern, "");
	}
	return a;
}


bool CutEndingWithCheck(string *s, regex PatternCheck, regex PatternCut)
{
	bool a = regex_search(*s, PatternCheck);
	if (a)
	{
		//концовка нашлась - режем ее
		*s = regex_replace(*s, PatternCut, "");
	}
	return a;
}


bool GetPreAndRV(string source,string* Pre, string* RV)
{
	smatch match;
	//находим первую гласную
	bool haveVowel = regex_search(source, match, RVRE);

	if (!haveVowel)
	{
		*Pre = source;
		*RV = string("");
		return false;
	}
	else
	{
		int position = match.position();// .position(0);
		*Pre = source.substr(0, position+1);
		*RV = source.substr(position + 1, source.length() - position);
		return true;
	}

}


string WordCutter::CutWord(string word)
{
	//начальные приготовлени€
	word = LowerCase(word);

	word = regex_replace(word, regex("Є"), string("е"));

	//поиск RV и того, что перед ним
	string *Pre = new string();
	string *RV = new string();
	bool haveVowel = GetPreAndRV(word, Pre, RV);


	
	if (haveVowel) {
		//шаг 1
		if (!CutEndingWithCheck(RV, PERFECTIVEGROUND, PERFECTIVEGROUND_CUT))
		{
			//не обрезалось, пробуем удалить REFLEXIVE
			bool a = CutEnding(RV, REFLEXIVE);
			if (!CutEnding(RV, ADJECTIVE) || a)
			{
				if (!CutEndingWithCheck(RV, VERB, VERB_CUT))
				{
					CutEnding(RV, NOUN);
				}
			}
		}

		//шаг 2
		CutEnding(RV, I);

		//шаг 3
		CutEnding(RV, P);
		CutEnding(RV, SUPERLATIVE);
		CutEndingWithCheck(RV, NN, N);
		
		word = *Pre + *RV;
	}
	
	//std::cout << word << endl;
	return word;
}



