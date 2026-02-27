#include<stdio.h>
#include<iostream>
#include<fstream>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include <windows.h>
#include <psapi.h>
#endif

#pragma once



int getMemoryCost(){
    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
        HANDLE hProcess = GetCurrentProcess();
        PROCESS_MEMORY_COUNTERS pmc;
        int workingSetSize;
        if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
            workingSetSize = pmc.WorkingSetSize / 1024;
        } else {
            printf("fail to get process mem_cost\n");
            workingSetSize = -1;
        }
        CloseHandle(hProcess);
        return workingSetSize;

    #elif __linux__
        std::ifstream statFile("/proc/self/status");
        if (!statFile.is_open()) {
            std::cerr << "Failed to open /proc/self/status to get memory cost" << std::endl;
            return -1;
        }

        std::string line;
        int memoryUsage = 0;

        while (std::getline(statFile, line)) {
            if (line.find("VmRSS:") != std::string::npos) {
                std::string value = line.substr(line.find(':') + 1);
                memoryUsage = std::stoi(value);
                return memoryUsage;
            }
        }
    #elif
        printf("OS except Windows or Linux is not supported\n");
        return -1;
    #endif
}


int getMemoryCost(int pid){
    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
        HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
        PROCESS_MEMORY_COUNTERS pmc;
        int workingSetSize;
        if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
            workingSetSize = pmc.WorkingSetSize / 1024;
        } else {
            printf("fail to get process mem_cost\n");
            workingSetSize = -1;
        }
        CloseHandle(hProcess);
        return workingSetSize;

    #elif __linux__
        char file_name[64];
        sprintf(file_name, "/proc/%d/status", pid);
        std::ifstream statFile(file_name);
        if (!statFile.is_open()) {
            std::cerr << "Failed to open /proc/self/status to get memory cost" << std::endl;
            return -1;
        }

        std::string line;
        int memoryUsage = 0;

        while (std::getline(statFile, line)) {
            if (line.find("VmRSS:") != std::string::npos) {
                std::string value = line.substr(line.find(':') + 1);
                memoryUsage = std::stoi(value);
                return memoryUsage;
            }
        }
    #elif
        printf("OS except Windows or Linux is not supported\n");
        return -1;
    #endif
}