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

```shell
make dists
```
Как входные данные используется 50000 случайных английских слов из словаря.

![dummy](https://github.com/InversionSpaces/HashMap/blob/master/results/dists/dummy_dist.jpg "dummy")
 
![len](https://github.com/InversionSpaces/HashMap/blob/master/results/dists/len_dist.jpg "len")

![sum](https://github.com/InversionSpaces/HashMap/blob/master/results/dists/sum_dist.jpg "sum")

![sumoverlen](https://github.com/InversionSpaces/HashMap/blob/master/results/dists/sumoverlen_dist.jpg "sumoverlen")

![xor](https://github.com/InversionSpaces/HashMap/blob/master/results/dists/xor_dist.jpg "xor")

Приемлимое распределение показывает 'xor'.

## Профилирование

Для нахождения узких мест программы совершим по 5 миллионов операций `insert`, `erase`, `contains` над теми же 50 тысячами строк в случайном порядке. Профилирование проводём при помощи [gprof](https://sourceware.org/binutils/docs/gprof/). Результаты визуализируем при помощи [gprof2dot](https://github.com/jrfonseca/gprof2dot).

```shell
make prof
```

Результаты:
![prof](https://github.com/InversionSpaces/HashMap/blob/master/results/profs/prof.jpg)

Видно, что больше всего времени выполнения программы занимают функция хеширования и сравнения строк.

## Использование ассемблерного кода

Функция хеширование на ассемблере:
```nasm
	global __xor_hash

	section .text
__xor_hash:
	xor rax, rax 		; rax = 0

	mov rcx, rsi 		; rcx = len
	shr rcx, 0x3 		; rcx = len / 8
	
	test rcx, rcx
	je .process_back 	; if rcx == 0 - skip

.process_front_loop:
	xor rax, [rdi] 		; rax ^= [rdi] (64 bits)
	add rdi, 0x8 		; rdi += 8
	
	loop .process_front_loop

.process_back:
	mov rcx, rsi 		; rcx = len
	and rcx, 0b111 		; rcx = len % 8

	test rcx, rcx
	je .end 		; if rcx == 0 - return

	add rdi, rcx
	dec rdi 		; rdi += rcx - 1

	xor rdx, rdx 		; rdx = 0

.process_back_loop:
	shl rdx, 0x8 		; rdx <<= 8
	or dl, [rdi] 		; rdx |= *rdi
	dec rdi 		; --rdi

	loop .process_back_loop
	
	xor rax, rdx 		; rax ^= rdx
.end:
	ret
```

Её использование в c++:
```cpp
extern "C" uint64_t __xor_hash(const char* str, size_t len);
 
inline uint64_t xor_hash_asm(const string& str) {
        return __xor_hash(str.data(), str.size());
}
```

По соглашению о вызовах, аргументы передаются в `rdi` и `rsi`, возвращаемое значение находится в `rax`.

Аналогично для функции сравнения строк:

```nasm
	global __strcmp_asm

	section .text
__strcmp_asm:
	mov rcx, rdx
	shr rcx, 0x3 		; rcx = len / 8

	test rcx, rcx 		; if !rcx - process_back
	je .process_back

	repe cmpsq 		; compare strings
	je .process_back

	xor rax, rax 		; if not equal - return 0
	ret

.process_back:
	mov rcx, rdx      
	and rcx, 0b111 		; rcx = len % 8

	repe cmpsb 		; compare strings
	je .end

	xor rax, rax 		; if not equal - return 0
	ret
.end:
	mov rax, 1
	ret
```

Её использование в c++:

```cpp
extern "C" bool __strcmp_asm(const char* s1, const char* s2, size_t len);

struct streqasm {
	bool operator()(const string& lhs, const string& rhs) const {
		if (lhs.size() != rhs.size())
                	return false;
			
                return __strcmp_asm(lhs.data(), rhs.data(), lhs.size());
	}
};
```

Её аргументы лежат в `rdi`, `rsi` и `rdx`, а возвращаемое значение так же в `rax`.

## Профилирование с ассемблерным кодом

```shell
make asmprof
```

Результат:

![asmprof](https://github.com/InversionSpaces/HashMap/blob/master/results/profs/asmprof.jpg)

## Результаты оптимизации

При компилировании без оптимизаций процент проводимого в критических функция времени сильно снизился:

```
| Function | Time spent in it without optimization | Time in it with optimization |
|:--------:|:-------------------------------------:|:----------------------------:|
| xor_hash |           19.42% of run time          |       2.93% of run time      |
|   streq  |           10.81% of run time          |       8.93% of run time      |
```

Выполним тесты: проведём по 10 миллионов операций `insert`, `contains` и `erase` над 100 тысячами случайных строк из словаря в случайном порядке.

Машина, на которой проводились тесты:
```
OS: Manjaro 19.0.2 Kyria
Kernel: x86_64 Linux 5.4.30-1-MANJARO
CPU: Intel Core i5-8250U @ 8x 3.4GHz
RAM: 7872MiB
```

Версия `g++`:
```
g++ (Arch Linux 9.3.0-1) 9.3.0
```

Замеряем:

```shell
make meas asmmeas OLVL=-O0 clean
make meas asmmeas OLVL=-O1 clean
make meas asmmeas OLVL=-O2 clean
make meas asmmeas OLVL=-O3 clean
```

Результаты:

```
| Optimization level | Time without optimization  | Time with optimization | Time without optimization/Time with optimization |
|:------------------:|:--------------------------:|:----------------------:|:------------------------------------------------:|
| -O0                |           61.57s           |         50.88s         |                       1.21                       |
| -O1                |           26.50s           |         27.70s         |                       0.96                       |
| -O2                |           26.25s           |         27.30s         |                       0.96                       |
| -O3                |           25.36s           |         27.81s         |                       0.91                       |
```
