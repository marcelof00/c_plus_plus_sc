// Project: VOIP Gatekeeper Simulation in C++
// Author: Marcelo Fernandez
// Description: This program simulates a simplified SIP gatekeeper for managing VoIP sessions.
// It demonstrates socket programming, session tracking, and lightweight memory/resource control.

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <ctime>
#include <thread>
#include <chrono>
#include <mutex>

struct SipSession
{
    std::string caller;
    std::string callee;
    std::time_t startTime;
    bool active;
};

std::unordered_map<int, SipSession> sessionTable;
std::mutex sessionMutex;
int sessionCounter = 1000;

int startSession(const std::string &caller, const std::string &callee)
{
    std::lock_guard<std::mutex> lock(sessionMutex);
    int sessionId = sessionCounter++;
    sessionTable[sessionId] = {caller, callee, std::time(nullptr), true};
    std::cout << "[INFO] Session started: ID=" << sessionId
              << ", Caller=" << caller << ", Callee=" << callee << std::endl;
    return sessionId;
}

void endSession(int sessionId)
{
    std::lock_guard<std::mutex> lock(sessionMutex);
    auto it = sessionTable.find(sessionId);
    if (it != sessionTable.end() && it->second.active)
    {
        it->second.active = false;
        std::time_t now = std::time(nullptr);
        std::cout << "[INFO] Session ended: ID=" << sessionId
                  << ", Duration=" << (now - it->second.startTime) << " sec" << std::endl;
    }
    else
    {
        std::cerr << "[ERROR] Session ID " << sessionId << " not found or already inactive." << std::endl;
    }
}

void printActiveSessions()
{
    std::lock_guard<std::mutex> lock(sessionMutex);
    std::cout << "\n[INFO] Active Sessions:" << std::endl;
    for (const auto &[id, session] : sessionTable)
    {
        if (session.active)
        {
            std::cout << "  ID=" << id << " | "
                      << "Caller=" << session.caller << " | "
                      << "Callee=" << session.callee << " | "
                      << "Start=" << std::ctime(&session.startTime);
        }
    }
}

int main()
{
    std::cout << "\n--- VoIP Gatekeeper Simulation ---\n"
              << std::endl;

    int session1 = startSession("Marcelo", "Javier");
    int session2 = startSession("Rogelio", "Victor");

    std::this_thread::sleep_for(std::chrono::seconds(2));
    printActiveSessions();

    std::this_thread::sleep_for(std::chrono::seconds(2));
    endSession(session1);

    std::this_thread::sleep_for(std::chrono::seconds(2));
    printActiveSessions();

    return 0;
}
