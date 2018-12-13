#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

std::vector<std::string> foundVars;
std::vector<std::string> newCode;
std::vector<std::string> leadingCode;

bool checkVars(std::string newVar)
{
	bool isPresent = false;
	for(std::string fv : foundVars)
	{
		if(fv == newVar)
		{
			isPresent = true;
			break;
		}
	}

	if(isPresent)
	{
		return true;
	}
	else
	{
		foundVars.push_back(newVar);
		return false;
	}
}

void writeLabel(std::string instruction)
{
	std::string toWrite = "";

	for(char letter : instruction)
	{
		toWrite.push_back(letter);

		if(letter == ':')
		{
			break;
		}
	}

	newCode.push_back(toWrite);
}

void decVariable(std::string newVar)
{
	std::string toWrite = "";
	if(!checkVars(newVar))
	{
		toWrite = "int " + newVar + "= 0;";
	}

	leadingCode.push_back(toWrite);
}

void writeGoto(std::string instruction)
{
	std::string toWrite = "if(";
	std::string workingText = "";
	std::stringstream extractor(instruction);
	std::getline(extractor, workingText, '!');
	bool startRead = false;
	std::string switchVar = "";

	for(char letter : workingText)
	{
		if(letter == 'Z' || letter == 'X' || letter == 'Y')
		{
			startRead = true;
		}
		if(letter == ' ')
		{
			startRead = false;
		}

		if(startRead)
		{
			switchVar.push_back(letter);
			toWrite.push_back(letter);
		}
	}

	decVariable(switchVar);
	toWrite += " != 0) { goto ";
	std::getline(extractor, workingText);

	for(char letter : workingText)
	{
		if(letter == 'A' || letter == 'B' || letter == 'C' || letter == 'D')
		{
			startRead = true;
		}
		if(letter == ' ')
		{
			startRead = false;
		}

		if(startRead)
		{
			toWrite.push_back(letter);
		}
	}

	toWrite += "; }";
	newCode.push_back(toWrite);
}

void writeOther(std::string instruction)
{
	std::string toWrite = "";
	std::string checkVar = "";
	int startPos = instruction.find(':');
	if(startPos == std::string::npos)
	{
		startPos = 0;
	}
	else
	{
		startPos += 2;
	}

	for(int i = startPos; i < instruction.length(); i++)
	{
		if(instruction[i] == '<')
		{
			startPos = i + 1;
			break;
		}

		checkVar.push_back(instruction[i]);
	}

	decVariable(checkVar);

	for(int i = startPos; i < instruction.length(); i++)
	{
		if(instruction[i] == '+')
		{
			toWrite = checkVar + "++;";
			break;
		}
		if(instruction[i] == '-')
		{
			toWrite = "floorSub(" + checkVar + ");";
			break;
		}

	}

	if(toWrite != "")
	{
		newCode.push_back(toWrite);
	}

}

void processLine(std::ifstream* in)
{
	std::string instruction;
	std::getline(*in, instruction);
	bool isGoto = false;

	for(char letter : instruction)
	{
		if(letter == ':')
		{
			writeLabel(instruction);
		}

		if(letter == 'I')
		{
			writeGoto(instruction);
			isGoto = true;
			break;
		}
	}

	if(!isGoto)
	{
		writeOther(instruction);
	}

}

int main(int argc, char* argv[])
{
	if(argc == 0)
	{
		return 1;
	}

	std::ofstream* out = new std::ofstream("out.cpp");
	std::ifstream* in = new std::ifstream(argv[0]);

	*out << "#include <iostream>" << std::endl;
	*out << "void floorSub(int& num)\n{\nif(num != 0){num--;}\n}" << std::endl;
	*out << std::endl << "int main(int argc, char* argv[])\n{" << std::endl;


	while(!in->eof())
	{
		processLine(in);
	}

	for(std::string str : leadingCode)
	{
		*out << str << std::endl;
	}

	for(std::string str : newCode)
	{
		*out << str << std::endl;
	}

	*out << "E:\nreturn 0;\n}" << std::endl;

	out->close();
	in->close();
	delete out;
	delete in;

	return 0;
}