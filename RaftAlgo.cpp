
#include <iostream>
#include <vector>
#include <map>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>

class RaftNode {
public:
    RaftNode(int id) : id(id), leader(-1), term(0), running(false), electionTimeout(1000) {}

    void start() {
        running = true;
        std::thread([this] { run(); }).detach();
    }

    void stop() {
        running = false;
    }

    void run() {
        while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(electionTimeout));
            // Start election
            startElection();
        }
    }

    void startElection() {
        std::unique_lock<std::mutex> lock(mutex);
        if (state == State::Follower && leader == -1) {
            term++;
            state = State::Candidate;
            // Send RequestVote RPC to other nodes
            std::cout << "Node " << id << " started election for term " << term << std::endl;
            // Simulate election process
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            if (state == State::Candidate) {
                becomeLeader();
            }
        }
    }

    void becomeLeader() {
        std::lock_guard<std::mutex> lock(mutex);
        state = State::Leader;
        leader = id;
        std::cout << "Node " << id << " became Leader for term " << term << std::endl;
        // Start sending heartbeat messages to other nodes
    }

private:
    enum class State { Follower, Candidate, Leader };
    int id;
    int leader;
    int term;
    bool running;
    State state;
    int electionTimeout;
    std::mutex mutex;
};

// main.cpp
int main() {
    std::vector<RaftNode> nodes;
    for (int i = 0; i < 5; ++i) {
        nodes.emplace_back(i + 1);
        nodes.back().start();
    }
    std::this_thread::sleep_for(std::chrono::seconds(10));
    for (auto& node : nodes) {
        node.stop();
    }
    return 0;
}
