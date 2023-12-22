#include "InstanceManager.h"

using ConfFilePair = std::pair<String, jamd::structs::ConfObj>;
struct MemoryLimit {
    int xms;
    int xmx;
};

struct jamd::structs::ConfObj {
    String name;
    String executor;
    MemoryLimit memoryLimit;
    InstanceParams params;
};
