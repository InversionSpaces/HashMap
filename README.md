# Исследование распределения значений хэш-функций и оптимизирование HashMap написанием критических функций на ассемблере.

## Распределение значений хэш-функций

Хэш-функции:

'''
uint64_t dummy_hash(const string& str) {
	return 1;
}
'''

'''
uint64_t len_hash(const string& str) {
	return str.size();
}
'''

'''
uint64_t sum_hash(const string& str) {
	uint64_t sum = 0;
	
	for (const char& c: str)
		sum += static_cast<uint64_t>(c);

	return sum;
}
'''

'''
uint64_t sumoverlen_hash(const string& str) {
	return str.size() ? sum_hash(str) / str.size() : 0;
}
'''

'''
uint64_t xor_hash(const string& str) {
	uint64_t hash = 0;
	
	size_t i = 0;
	for (const auto& c: str) {
		hash ^= static_cast<uint64_t>(c) << 8 * i;
		i = (i + 1) % 4;
	}
	
	return hash;
}
'''
