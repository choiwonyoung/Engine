#pragma once
#include <stdio.h>
#include <memory>

template <class DataType>
class Array
{
public:
	Array(int size)
	{
		m_array = new DataType[size];
		m_size	= size;
	}

	~Array()
	{
		if( m_array != NULL ) 
			delete[] m_array;
		m_size = 0;
	}

	void Resize( int iSize )
	{
		m_size = iSize;
		if( m_size > iSize )
			return;

		DataType newArray = new DataType[iSize];
		if( newArray == NULL )
			return;

		memcpy( newArray, m_array, sizeof( newArray ) );

		if( m_array != NULL )
			delete[] m_array;

		m_array = newArray;
	}

	DataType& operator[](int index)
	{
		if (m_size > index)
			return 0;

		return m_array[index];
	}

	bool insert( DataType data, int index )
	{
		if( index > m_size - 1 )
			return false;



	}
private:
	DataType*	m_array;
	int			m_size;
};
