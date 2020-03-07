/**
 *  Copyright (C) 2019 freedmjs@miamiOH.edu
 * A program to demonstrate:
 *   create a custom shell
 *
 * Author - freedmjs
 * 
 *fixes for base case: quoted dont work, exit codes
 */
#include <unistd.h>
#include<sys/wait.h>
#include <cstdlib>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <fstream>

using namespace std;
using StrVec = std::vector<std::string>;
/*
 * 
 */
StrVec splitString(std::string line) {
    // splits string into string vector
    std::istringstream is(line);
    StrVec retVal;
    std::string word;
    while (is >> std::quoted(word)) {
        retVal.push_back(word);
    }
    return retVal;
}
void myExec(StrVec argList) {
std::vector<char*> args;  // list of pointers to args
for(auto& s : argList) {
    args.push_back(&s[0]);
}
// address of 1st character
// nullptris very important
args.push_back(nullptr);
// Make execvp system call to run desired process
execvp(args[0], &args[0]);
}
void serial(StrVec args) {
     int stat;
     std::ifstream in = std::ifstream(args[1]);
     std::string line;
     std::string cmd;
     std::string val;   
     // reads in line
     while (std::getline(in, line)) {
        StrVec a = StrVec(); 
        std::istringstream is(line);
        is >> cmd >> val;
        a.push_back(cmd); a.push_back(val);
        // checks line isnt a comment or blank
        if (cmd == "#" || line == "") { 
            continue;
        } else {
            std::cout << "Running: " << a.at(0) << " " << a.at(1) << std::endl;
             const int pid = fork();
             if (pid == 0) {
                 // executes process
                 myExec(a);    
            } else {
                 // waits for process to finish 
                waitpid(pid, &stat, 0);
                std::cout << "Exit code: " << stat << std::endl;
            }
        }
     }  
}
void wait(std::vector<int> pids) {
    int stat;
    // executes  waitpid() for all processes
    for (auto pid : pids) {
       waitpid(pid, &stat, 0);
       std::cout << "Exit code: " << stat << std::endl; 
    }
}
void parallel(StrVec args) {
    std::ifstream in = std::ifstream(args[1]);
    std::string line, cmd, val;
    std::vector<int> pids;
    // reads line into vector
    while (std::getline(in, line)) {
        StrVec a = StrVec(); 
        std::istringstream is(line);
        is >> cmd >> val;
        a.push_back(cmd); a.push_back(val);
        // checks if line is comment or blank
        if (cmd == "#" || line == "") { 
            continue;
        } else {
            std::cout << "Running: " << a.at(0) << " " << a.at(1) << std::endl;
            // forks
             const int pid = fork();
             if (pid == 0) {
                 // executes
                 myExec(a);    
             } else { 
                 pids.push_back(pid); 
             }  // adds pids to vector
        }
     } 
        wait(pids);  // executes waitpid() commands
}

void process(std::string line) {
    int stat;
    StrVec input = splitString(line);
    // checks if it runs serial, parallel or just command line arg
    if (input.at(0) == "exit") {
       return;
    } else if (input.at(0)  == "SERIAL") { 
       serial(input);
    } else if (input.at(0)  == "PARALLEL") {    
       parallel(input);
    } else if (input.at(0)  == "#" || line == "" || line == " ") {  
    } else {        
         // Create a child process
        std::string wordArg; cout << "Running:";
    for (auto s : input) {
        std::cout << " " <<  s;
    }
        std::cout << std::endl;
        // forks
        const int pid = fork();
         if (pid == 0) {
             // executes
             myExec(input);    
         } else {
             // waits for processes to finish
             waitpid(pid, &stat, 0);
         }
        std::cout << "Exit code: " << stat << std::endl;
    }
}


int main(int argc, char** argv) {
    std::string line;
    // error with blank space, exits with exit
while (std::cout << "> ", std::getline(std::cin, line)) {
    if (line != "" && line != "exit") {
        process(line);
    } else if (line == "exit") {
        break;
    }
}
    return 0;
}

