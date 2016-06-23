#include "stdafx.h"
#include <iostream>
#include "WordCutter.h"

static regex PERFECTIVEGROUND = regex("((��|����|������|��|����|������)|([��](�|���|�����)))$");
static regex PERFECTIVEGROUND_CUT = regex("((��|����|������|��|����|������)|(�|���|�����))$");
static regex REFLEXIVE = regex("(�[��])$");
static regex ADJECTIVE = regex("(��|��|��|��|���|���|��|��|��|��|��|��|��|��|���|���|���|���|��|��|��|��|��|��|��|��)$");
static regex VERB = regex("((���|���|���|����|����|���|���|���|��|��|��|��|��|��|��|���|���|���|��|���|���|��|��|���|���|���|���|��|�)|(([��])(��|��|���|���|��|�|�|��|�|��|��|��|��|��|��|���|���)))$");
static regex VERB_CUT = regex("((���|���|���|����|����|���|���|���|��|��|��|��|��|��|��|���|���|���|��|���|���|��|��|���|���|���|���|��|�)|(��|��|���|���|��|�|�|��|�|��|��|��|��|��|��|���|���))$");
static regex NOUN = regex("(�|��|��|��|��|�|����|���|���|��|��|�|���|��|��|��|�|���|��|���|��|��|��|�|�|��|���|��|�|�|��|��|�|��|��|�)$");
static regex RVRE = regex("[���������]");
static regex DER = regex("����?$");
static regex SUPERLATIVE = regex("(����|���)$");
static regex I = regex("�$");
static regex P = regex("�$");
static regex NN = regex("��$");
static regex N = regex("�$");


string LowerCase(string s)
{
	int dif = '�' - '�';
	for (int i = 0; i<s.length(); i++)
	{
		if ((s[i] >= '�') && (s[i] <= '�'))
			s[i] += dif;
	}
	return s;
}


bool CutEnding(string *s, regex pattern)
{
	bool a = regex_search(*s, pattern);
	if (a)
	{
		//�������� ������� - ����� ��
		*s = regex_replace(*s, pattern, "");
	}
	return a;
}


bool CutEndingWithCheck(string *s, regex PatternCheck, regex PatternCut)
{
	bool a = regex_search(*s, PatternCheck);
	if (a)
	{
		//�������� ������� - ����� ��
		*s = regex_replace(*s, PatternCut, "");
	}
	return a;
}


bool GetPreAndRV(string source,string* Pre, string* RV)
{
	smatch match;
	//������� ������ �������
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
	//��������� �������������
	word = LowerCase(word);

	word = regex_replace(word, regex("�"), string("�"));

	//����� RV � ����, ��� ����� ���
	string *Pre = new string();
	string *RV = new string();
	bool haveVowel = GetPreAndRV(word, Pre, RV);


	
	if (haveVowel) {
		//��� 1
		if (!CutEndingWithCheck(RV, PERFECTIVEGROUND, PERFECTIVEGROUND_CUT))
		{
			//�� ����������, ������� ������� REFLEXIVE
			bool a = CutEnding(RV, REFLEXIVE);
			if (!CutEnding(RV, ADJECTIVE) || a)
			{
				if (!CutEndingWithCheck(RV, VERB, VERB_CUT))
				{
					CutEnding(RV, NOUN);
				}
			}
		}

		//��� 2
		CutEnding(RV, I);

		//��� 3
		CutEnding(RV, P);
		CutEnding(RV, SUPERLATIVE);
		CutEndingWithCheck(RV, NN, N);
		
		word = *Pre + *RV;
	}
	
	//std::cout << word << endl;
	return word;
}



