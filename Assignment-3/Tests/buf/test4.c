void mem_insert(void *buffer, const void *data, int data_size, int position);
extern void OVERFLOW(void* data, int size);
int main() {
	char buffer[10] = {0};
	mem_insert(buffer, "123456", 6, 5);
	OVERFLOW(buffer, 11);
}