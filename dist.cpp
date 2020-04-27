#include <iostream>
#include <fstream>
#include <functional>
#include <cinttypes>
#include <cstring>
#include <vector>
#include <chrono>
#include <algorithm>
#include <random>
#include <stdexcept>
#include <utility>

#include "hashmap.hpp"

using namespace std;

uint64_t dummy_hash(const string& str) {
	return 1;
}

uint64_t len_hash(const string& str) {
	return str.size();
}

uint64_t sum_hash(const string& str) {
	uint64_t sum = 0;
	
	for (const char& c: str)
		sum += static_cast<uint64_t>(c);

	return sum;
}

uint64_t sumoverlen_hash(const string& str) {
	return str.size() ? sum_hash(str) / str.size() : 0;
}

uint64_t xor_hash(const string& str) {
	uint64_t hash = 0;
	
	size_t i = 0;
	for (const auto& c: str) {
		hash ^= static_cast<uint64_t>(c) << 8 * i;
		i = (i + 1) % 4;
	}
	
	return hash;
}

vector<string> load_lines(const char* filename) {
	ifstream input(filename);

	vector<string> retval;
	for (	retval.emplace_back(); 
		getline(input, retval.back()); 
		retval.emplace_back());
	retval.pop_back();

	return retval;
}

template<typename time_t = chrono::microseconds>
struct measure {
	template<typename F, typename ...Args>
	static typename time_t::rep exec(F func, Args&& ...args) {
		auto st = chrono::high_resolution_clock::now();
		func(forward<Args>(args)...);
		auto ed = chrono::high_resolution_clock::now();
		auto duration = chrono::duration_cast<time_t>(ed - st);

		return duration.count();
	}
};

typedef function<uint64_t(const string&)> hashfunc_t;

void test_hash(vector<string>& lines, const char* name, hashfunc_t func) {
	HashMap<string> hmap(func);

	auto insertion = [] (const vector<string>& lines, HashMap<string>& hmap) {
		for (const auto& l: lines)
			hmap.insert(l);
	};
	size_t full_ins_time = measure<>::exec(insertion, lines, hmap);

	auto seed = chrono::system_clock::now().time_since_epoch().count();
	auto engine = default_random_engine(seed);
	shuffle(lines.begin(), lines.end(), engine);
	
	auto contains = [] (const vector<string>& lines, HashMap<string>& hmap) {
		for (const auto& l: lines)
			if (!hmap.contains(l))
				throw logic_error("There must be such key");
	};
	size_t full_contains_time = measure<>::exec(contains, lines, hmap);

	auto dist = hmap.get_stats();
	uint64_t dist_sum = 0;
	for (const auto& i: dist)
		dist_sum += i;

	auto erase = [] (const vector<string>& lines, HashMap<string>& hmap) {
		for (const auto& l: lines)
			hmap.erase(l);
	};
	size_t full_erase_time = measure<>::exec(erase, lines, hmap);

	string base(name);
	size_t count = lines.size();
	
	ofstream distout(base + "_dist.csv");
	distout << "count\n";
	for (const auto& i: dist)
		distout << i << '\n';
	distout.close();

	ofstream avgout(base + "_avg.csv");
	avgout << "count;ins_avg;erase_avg;cotains_avg;dist_avg\n";
	avgout 	<< count << ";"
	       	<< full_ins_time / count << ";"
		<< full_erase_time / count << ";"
		<< full_contains_time / count << ";"
		<< dist_sum / dist.size() << "\n";
	avgout.close();
}

int main() {
	auto lines = load_lines("strings");
	const vector<pair<const char*, hashfunc_t>> hashes = {
		{"len", len_hash},
		{"sum", sum_hash},
		{"xor", xor_hash},
		{"sumoverlen", sumoverlen_hash}, 
		{"dummy", dummy_hash}
	};

	for (auto& [name, hashfunc]: hashes) {
		cout 	<< name << " time in ms: "
			<< measure<chrono::milliseconds>::exec(test_hash, lines, name, hashfunc) << endl;
	}
}
