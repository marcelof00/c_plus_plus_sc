#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <random>
#include <fstream>
#include <cstdlib>

bool driverWrite(int id, int payload)
{
    return (rand() % 100) > 2; // Randomly simulate occasional write failure -> 98% success rate
}

bool driverRead(int id, int &outPayload)
{
    // Simulate reading from hardware with 97% success
    if ((rand() % 100) > 3)
    {
        outPayload = rand() % 1000;
        return true;
    }
    return false;
}

void logToFile(const std::string &message)
{
    std::ofstream log("driver_test_log.txt", std::ios_base::app);
    if (log.is_open())
    {
        log << message << std::endl;
    }
}

void stressTestDriver(int deviceId, int iterations, int delayMs)
{
    int readSuccess = 0, writeSuccess = 0;
    int readFail = 0, writeFail = 0;
    auto startTime = std::chrono::steady_clock::now();

    for (int i = 0; i < iterations; ++i)
    {
        int payload = rand() % 1000;
        bool w = driverWrite(deviceId, payload);
        if (w)
            writeSuccess++;
        else
        {
            writeFail++;
            logToFile("[WARN] Write failed at iteration " + std::to_string(i));
        }

        int readVal = 0;
        bool r = driverRead(deviceId, readVal);
        if (r)
            readSuccess++;
        else
        {
            readFail++;
            logToFile("[WARN] Read failed at iteration " + std::to_string(i));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
    }

    auto endTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = endTime - startTime;

    std::cout << "\n--- Driver Stress Test Report ---\n";
    std::cout << "Device ID: " << deviceId << std::endl;
    std::cout << "Iterations: " << iterations << ", Delay: " << delayMs << "ms" << std::endl;
    std::cout << "Total time: " << elapsed.count() << " seconds" << std::endl;
    std::cout << "Writes: Success = " << writeSuccess << ", Fail = " << writeFail << std::endl;
    std::cout << "Reads : Success = " << readSuccess << ", Fail = " << readFail << std::endl;
    std::cout << "Failures logged to driver_test_log.txt" << std::endl;
}

int main()
{
    std::cout << "Driver QA Stress Test Simulation\n";
    int deviceId = 42;
    int iterations = 100;
    int delayMs = 50;

    stressTestDriver(deviceId, iterations, delayMs);

    return 0;
}