#pragma once

#include <chrono>

namespace CG2 {

class Timer {
public:
	Timer() { startPoint = endPoint = std::chrono::system_clock::now();};
	~Timer() {};

	void start() { startPoint = std::chrono::system_clock::now(); }
	void stop() { endPoint = std::chrono::system_clock::now(); }

	/// Returns the elapsed time in seconds
	float elapsedMilliseconds() {
		std::chrono::milliseconds total_ms = std::chrono::duration_cast<std::chrono::milliseconds>(endPoint - startPoint);
		return total_ms.count() * 0.001f;
	}

private:
	/// Disallow evil contructors
	Timer(const Timer&);
	Timer& operator=(const Timer&);

	std::chrono::system_clock::time_point startPoint;
	std::chrono::system_clock::time_point endPoint;
};

}
