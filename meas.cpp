#include <iostream>
#include <fstream>
#include <cinttypes>
#include <vector>
#include <random>
#include <string>
#include <stdexcept>

#include "hashmap.hpp"
#include "helpers.hpp"

using namespace std;

inline uint64_t xor_hash(const string& str) {
	uint64_t hash = 0;
	
	size_t i = 0;
	for (const auto& c: str) {
		hash ^= static_cast<const uint64_t>(c) << 8 * i;
		i = (i + 1) % 8;
	}
	
	return hash;
}

#ifdef ASMOPTIMIZATION
extern "C" uint64_t __xor_hash(const char* str, size_t len);

inline uint64_t xor_hash_asm(const string& str) {
	return __xor_hash(str.data(), str.size());
}
#endif

class randdist {
private:
	static random_device rd;
	
	mt19937 gen;
	uniform_int_distribution<> dis;
public:
	randdist(int st, int ed) : dis(st, ed), gen(randdist::rd()) {}

	int next() {
		return dis(gen);
	}
};

random_device randdist::rd;

#ifdef ASMOPTIMIZATION
extern "C" bool __strcmp_asm(const char* s1, const char* s2, size_t len);

struct streqasm {
	bool operator()(const string& lhs, const string& rhs) const {
		if (lhs.size() != rhs.size())
			return false;

		return __strcmp_asm(lhs.data(), rhs.data(), lhs.size());
	}
};
#endif

int main() {
	auto lines = load_lines("strings");
	if (lines.size() == 0) {
		throw runtime_error("Empty strings file");
	}
	randdist dist(0, lines.size() - 1);

#ifdef ASMOPTIMIZATION
	HashMap<string, streqasm> hmap(xor_hash_asm);
#else
	HashMap<string> hmap(xor_hash);
#endif
	for (int i = 0; i < lines.size(); ++i) {
		hmap.insert(lines[dist.next()]);
		hmap.contains(lines[dist.next()]);
		hmap.erase(lines[dist.next()]);
	}
}
