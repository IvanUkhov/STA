#ifndef __ARRAY_H__
#define __ARRAY_H__

#include <string.h>

#define __ALLOC(type, size) \
	(type *)malloc(sizeof(type) * size)

#define __FREE(some) \
	do { \
		if (some) free(some); \
		some = NULL; \
	} while(0)

#define __NULLIFY(some, type, size) \
	memset(some, 0, sizeof(type) * size)

#define __MEMCPY(dst, src, type, size) \
	memcpy(dst, src, sizeof(type) * size)

template <class T>
struct array_t
{
	size_t _size;
	T *_data;

	array_t() : _size(0), _data(NULL)
	{
	}

	array_t(size_t __size) : _size(__size)
	{
		_data = __ALLOC(T, _size);
	}

	array_t(size_t __size, T value) : _size(__size)
	{
		_data = __ALLOC(T, _size);

		if (value == 0) nullify();
		else {
			for (size_t i = 0; i < _size; i++)
				_data[i] = value;
		}
	}

	array_t(const array_t &another) : _size(another._size)
	{
		_data = __ALLOC(T, _size);
		__MEMCPY(_data, another._data, T, _size);
	}

	~array_t()
	{
		__FREE(_data);
	}

	inline void nullify()
	{
		__NULLIFY(_data, T, _size);
	}

	inline void resize(size_t __size)
	{
		if (_size == __size) return;

		__FREE(_data);

		_size = __size;
		_data = __ALLOC(T, _size);
	}

	inline void extend(size_t __size)
	{
		if (_size == __size) {
			return;
		}
		else if (__size < _size) {
			shrink(__size);
			return;
		}

		T *new_data = __ALLOC(T, __size);

		__MEMCPY(new_data, _data, T, _size);

		__FREE(_data);
		_size = __size;
		_data = new_data;
	}

	inline void shrink(size_t __size)
	{
		_size = __size;
	}

	inline T &operator[](unsigned int i)
	{
		return _data[i];
	}

	inline const T &operator[](unsigned int i) const
	{
		return _data[i];
	}

	inline array_t &operator=(const array_t &another)
	{
		if (_size != another._size) {
			__FREE(_data);

			_size = another._size;
			_data = __ALLOC(T, _size);
		}

		__MEMCPY(_data, another._data, T, _size);
		return *this;
	}

	inline operator T *()
	{
		return _data;
	}

	inline operator const T *() const
	{
		return _data;
	}

	inline T *point()
	{
		return _data;
	}

	inline const T *point() const
	{
		return _data;
	}

	inline size_t size() const
	{
		return _size;
	}

	inline bool empty() const
	{
		return _size == 0;
	}
};

typedef array_t<bool> bit_string_t;

#endif
