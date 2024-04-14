#include "Series.h"

namespace framework
{
    namespace dataframe
    {
        template <typename T>
        Series<T>::Series(const vector<T> &data)
            : data(data)
        {
        }

        template <typename T>
        Series<T>::Series(const Series<T> &other)
            : data(other.data)
        {
        }

        template <typename T>
        T &Series<T>::operator[](size_t index)
        {
            return data[index];
        }

        template <typename T>
        const T &Series<T>::operator[](size_t index) const
        {
            return data[index];
        }
    }
}