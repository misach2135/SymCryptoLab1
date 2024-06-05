#include <iostream>
#include <vector>
#include <windows.h>
#include <cwchar>
#include <fstream>
#include <cwctype>
#include <map>
#include <algorithm>
#include <sstream>
#include <random>
#include <cmath>

// For default value in map
struct uintbox
{
	uint64_t val = 0;

	operator uint64_t() { return val; }
};

struct TextInfo
{
	uint32_t textSize = 0;
	uint32_t overlappingBigramsSize = 0;
	uint32_t nonOverlappingBigramsSize = 0;
	std::map<char, uintbox> symbolsCount;
	std::map<std::string, uintbox> overlappingBigramsCount;
	std::map<std::string, uintbox> nonOverlappingBigramsCount;

	double symbolsEntropy = 0;
	double overlappingBigramsEntropy = 0;
	double nonOverlappingBigramsEntropy = 0;

};

bool ²sCharCyrrillic(char c)
{
	return (c >= (char)192) && (c <= (char)255);
}

double CalcLetterEntropy(std::map<char, uintbox> symbolsCount, uint64_t textSize)
{
	double res = 0.0;
	for (const auto& kv : symbolsCount)
	{
		double p = static_cast<double>(kv.second.val) / textSize;
		res -= p * log2(p);
	}
	return res;
}

double CalcOverlappingBigramEntropy(std::map<std::string, uintbox> overlappingBigramsCount, uint64_t textSize)
{
	double res = 0.0;
	for (const auto& kv : overlappingBigramsCount)
	{
		double p = static_cast<double>(kv.second.val) / (textSize - 1);
		res -= p * log2(p);
	}
	return res / 2;
}

double CalcNonOverlappingBigramEntropy(std::map<std::string, uintbox> nonOverlappingBigramsCount, uint64_t textSize)
{
	double res = 0.0;
	for (const auto& kv : nonOverlappingBigramsCount)
	{
		double p = static_cast<double>(kv.second.val) / (textSize / 2);
		res -= p * log2(p);
	}
	return res / 2;
}


TextInfo TextInfoFromFileWithSpaces(std::string filename)
{
	std::setlocale(LC_ALL, "ru-Ru");
	SetConsoleOutputCP(1251);
	SetConsoleCP(1251);
	std::ifstream in(filename);

	TextInfo info;
	info.textSize = 0;

	if (in.is_open())
	{
		std::cout << "Filter input text..." << std::endl;
		
		std::stringstream ss;
		in >> std::noskipws;
		char prev = 0;
		char c = 0;
		while (in.get(c))
		{
			if ((²sCharCyrrillic(c) || (isspace(c) && !isspace(prev))) && c != 10)
			{
				c = tolower(c);
				ss << c;
				std::cout << c;
				prev = c;
			}
		}
		std::cout << std::endl;
		std::cout << "Starting counting symbols..." << std::endl;

		c = 0;
		ss >> std::noskipws;

		while (ss.get(c))
		{
				info.symbolsCount[c].val++;
				char next = ss.peek();
				if (!ss.eof())
				{
					std::string bigram{ c, next };

					info.overlappingBigramsCount[bigram].val++;
					info.overlappingBigramsSize++;

					if (info.textSize % 2 == 0)
					{
						info.nonOverlappingBigramsCount[bigram].val++;
						info.nonOverlappingBigramsSize++;
					}
				}
				info.textSize++;
			}
			
	}
	else
	{
		std::cout << "Could not open input file." << std::endl;
		return TextInfo{};
	}
	in.close();

	std::cout << "Calculating entropy..." << std::endl;

	info.symbolsEntropy = CalcLetterEntropy(info.symbolsCount, info.textSize);
	info.nonOverlappingBigramsEntropy = CalcNonOverlappingBigramEntropy(info.nonOverlappingBigramsCount, info.textSize);
	info.overlappingBigramsEntropy = CalcOverlappingBigramEntropy(info.overlappingBigramsCount, info.textSize);

	std::cout << "Done!" << std::endl;

	return info;
}

TextInfo TextInfoFromFileWithoutSpaces(std::string filename)
{
	std::setlocale(LC_ALL, "ru-Ru");
	SetConsoleOutputCP(1251);
	SetConsoleCP(1251);
	std::ifstream in(filename);

	TextInfo info;
	info.textSize = 0;

	if (in.is_open())
	{
		std::cout << "Filter input text..." << std::endl;

		std::stringstream ss;
		in >> std::skipws;
		char prev = 0;
		char c = 0;
		while (in.get(c))
		{
			if ((²sCharCyrrillic(c) && !isspace(c) && (c != 10)))
			{
				c = tolower(c);
				ss << c;
				std::cout << c;
				prev = c;
			}
		}
		std::cout << std::endl;
		std::cout << "Starting counting symbols..." << std::endl;

		c = 0;
		ss >> std::skipws;

		while (ss.get(c))
		{
			info.symbolsCount[c].val++;
			char next = ss.peek();
			if (!ss.eof())
			{
				std::string bigram{ c, next };

				info.overlappingBigramsCount[bigram].val++;
				info.overlappingBigramsSize++;

				if (info.textSize % 2 == 0)
				{
					info.nonOverlappingBigramsCount[bigram].val++;
					info.nonOverlappingBigramsSize++;
				}
			}
			info.textSize++;
		}

	}
	else
	{
		std::cout << "Could not open input file." << std::endl;
		return TextInfo{};
	}
	in.close();

	std::cout << "Calculating entropy..." << std::endl;

	info.symbolsEntropy = CalcLetterEntropy(info.symbolsCount, info.textSize);
	info.nonOverlappingBigramsEntropy = CalcNonOverlappingBigramEntropy(info.nonOverlappingBigramsCount, info.textSize);
	info.overlappingBigramsEntropy = CalcOverlappingBigramEntropy(info.overlappingBigramsCount, info.textSize);

	std::cout << "Done!" << std::endl;

	return info;
}

void SummarizeTextInfo(TextInfo info, std::string filename)
{
	std::ofstream out(filename);

	if (out.is_open())
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> d;
		out << "VERSION: " << d(gen) << "\n\n";
		out << "Text size: " << info.textSize << std::endl;
		out << "Letters: " << std::endl;

		for (const auto& kv : info.symbolsCount)
		{
			//out << "{\'" << kv.first << "\', " << (static_cast<double>(kv.second.val) / info.textSize) << "},\n"; 
			out << kv.first << " -- " << kv.second.val << ", " << (static_cast<double>(kv.second.val) / info.textSize) << ",\n";
		}

		out << std::endl;
		out << "Total letter in text: " << info.symbolsCount.size() << std::endl;
		out << "Entropy of symbols: " << info.symbolsEntropy << std::endl;
		out << std::endl;
		out << "Overlapping Bigrams: " << std::endl;
		
		for (const auto& kv : info.overlappingBigramsCount)
		{
			out << kv.first << " -- " << kv.second.val << ", " << (kv.second.val / double(info.overlappingBigramsSize)) << ",\n";
		}
		
		out << "Total overlaping bigrams in text: " << info.overlappingBigramsSize << std::endl;
		out << "Entropy of overlaping bigrams in text: " << info.overlappingBigramsEntropy << std::endl;
		out << std::endl;
		out << "Non overlapping Bigrams: " << std::endl;

		for (const auto& kv : info.nonOverlappingBigramsCount)
		{
			out << kv.first << " -- " << kv.second.val << ", " << (kv.second.val / double(info.nonOverlappingBigramsSize)) << ",\n";
		}
		
		out << "Total non overlaping bigrams in text: " << info.nonOverlappingBigramsSize << std::endl;
		out << "Entropy of non overlaping bigrams in text: " << info.nonOverlappingBigramsEntropy << std::endl;
	}
	else
	{
		std::cout << "Could not create out file." << std::endl;
	}

	out.close();

}

int main(int argc, char* argv[])
{
	std::string in_filename = "in.txt";
	std::string out_filename = "out";
	
	if (argc > 2)
	{
		in_filename = argv[1];
		out_filename = argv[2];
	}
	else if (argc > 1)
	{
		in_filename = argv[1];
	}

	TextInfo info1 = TextInfoFromFileWithSpaces(in_filename);
	TextInfo info2 = TextInfoFromFileWithoutSpaces(in_filename);
	SummarizeTextInfo(info1, out_filename + "withSpaces.txt");
	SummarizeTextInfo(info2, out_filename + "withoutSpaces.txt");

	return 0;
}