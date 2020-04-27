# Исследование распределения значений хэш-функций и оптимизирование HashMap написанием критических функций на ассемблере.

## Распределение значений хэш-функций

### Рассматриваемые хэш-функции:

```cpp
uint64_t dummy_hash(const string& str) {
	return 1;
}
```

```cpp
uint64_t len_hash(const string& str) {
	return str.size();
}
```

```cpp
uint64_t sum_hash(const string& str) {
	uint64_t sum = 0;
	
	for (const char& c: str)
		sum += static_cast<uint64_t>(c);

	return sum;
}
```

```cpp
uint64_t sumoverlen_hash(const string& str) {
	return str.size() ? sum_hash(str) / str.size() : 0;
}
```

```cpp
uint64_t xor_hash(const string& str) {
	uint64_t hash = 0;
	
	size_t i = 0;
	for (const auto& c: str) {
		hash ^= static_cast<uint64_t>(c) << 8 * i;
		i = (i + 1) % 8;
	}
	
	return hash;
}
```

### Строим распределения: 

`make dist`

![alt text](https://github.com/InversionSpaces/HashMap/blob/master/results/dist/dummy_dist.jpg "dummy")
![alt text](https://github.com/InversionSpaces/HashMap/blob/master/results/dist/len_dist.jpg "len")
![alt text](https://github.com/InversionSpaces/HashMap/blob/master/results/dist/sum_dist.jpg "sum")
![alt text](https://github.com/InversionSpaces/HashMap/blob/master/results/dist/sumoverlen_dist.jpg "sumoverlen")
![alt text](https://github.com/InversionSpaces/HashMap/blob/master/results/dist/xor_dist.jpg "xor")
