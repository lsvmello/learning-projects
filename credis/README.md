# Basic Redis copy

Implementation of the book [Build Your Own Redis with C/C++](https://build-your-own.org/redis).

## Compile

### Server

`g++ -Wall -Wextra -O2 -g server.cpp hashtable.cpp zset.cpp avl.cpp heap.cpp thread_pool.cpp -pthread -
o server`

### Client

`g++ -Wall -Wextra -O2 -g client.cpp -o client`

### Test Heap

`g++ -Wall -Wextra -O2 -g test_heap.cpp -o test_heap`

### Test AVL

`g++ -Wall -Wextra -O2 -g test_avl.cpp -o test_avl`

### Test Offset

`g++ -Wall -Wextra -O2 -g test_offset.cpp -o test_offset`
