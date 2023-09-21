#include <iostream>
#include <string>
#include <unistd.h>
#define DELIM " \t\r\n"  //the condition of spilt the string

std::string line;
std::string read(void) // read
{
    // In c++ we dont need  buffersize and reacllocate and staff for using string
    getline(std::cin, line);
}

std::string token;
std::string arg[];
std::string command;
std::string parse()
{
    command = strtok(line, DELIM); 
    while(line!=NULL)
    {
        
    }
}

bool execute(std::string command)
{
}

void loop() //  basic lifetime
{
    int STATUS; // save the condition of loop in a variable
    do          // keep(?) the prompt while active
    {
        std::string prompt = "";
        std::cout << prompt << ">" << std::endl;
    } while (STATUS);
    read();
    parse();
    execute();
}

int main()
{
    // initialize
    loop();
    // quit
    return 0;
}