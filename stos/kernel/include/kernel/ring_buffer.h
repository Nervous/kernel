#ifndef RING_BUFFER_H_
# define RING_BUFFER_H_

# include <kernel/early_alloc.h>
# include <kernel/errno.h>
# include <kernel/types.h>
# include <kernel/string.h>

struct ring_buffer {
	u8* buffer;
	size_t size;
	size_t start;
	size_t end;
};

static inline int ring_buffer_init(struct ring_buffer* rb, size_t size)
{
	assert(rb);

	rb->buffer = early_alloc(size);
	if (!rb->buffer)
		return -ENOMEM;

	rb->size = size;
	rb->start = 0;
	rb->end = 0;

	return 0;
}

static inline void ring_buffer_remove(struct ring_buffer* rb)
{
	assert(rb);

	early_free(rb->buffer, rb->size);
}

static inline int ring_buffer_is_empty(struct ring_buffer* rb)
{
	assert(rb);

	return rb->start == rb->end;
}

static inline size_t ring_buffer_write(struct ring_buffer* rb,
					u8* buf,
					size_t len)
{
	assert(rb);
	assert(rb->buffer);
	assert(buf);

	size_t i;
	for (i = 0; i < len; i++) {
		rb->buffer[rb->end++] = buf[i];

		if (rb->end == rb->size)
			rb->end = 0;

		if (rb->start == rb->end)
			rb->start = (rb->end + 1) % rb->size;
	}

	return i;
}

static inline size_t ring_buffer_read(struct ring_buffer* rb,
					off_t offset,
					u8* buf,
					size_t count)
{
	assert(rb);
	assert(rb->buffer);
	assert(buf);

	size_t start = (rb->start + offset) % rb->size;

	if (ring_buffer_is_empty(rb) || start == rb->end)
		return 0;

	size_t length = rb->end >= start ? rb->end - start : rb->size - start;
	length %= (count + 1);

	memcpy(buf, rb->buffer + start, length);

	size_t append = 0;
	if (rb->end < start && length < count) {
		append = rb->end > count - length ? count - length : rb->end;
		memcpy(buf + length, rb->buffer, append);
	}

	return length + append;
}

#endif /* !RING_BUFFER_H_ */
